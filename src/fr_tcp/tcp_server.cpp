#include "fr_tcp/tcp_server.h"

#include "fr_tcp/protocol.h"
#include "fr_public/pub_memory.h"
#include "fr_public/pub_tool.h"

using namespace std;
using namespace universal;

FrTcpServer::FrTcpServer()
	:server_(this),
	 max_packet_size_(2048)
{ ; }

FrTcpServer::FrTcpServer(const FrTcpServer &ref)
	:server_(this)
{ ; }

FrTcpServer& FrTcpServer::operator=(const FrTcpServer &ref){
	return *this;
}

FrTcpServer::~FrTcpServer(){ 
}

bool FrTcpServer::start(const string &ip, unsigned int port){
	if(server_->Start(ip.c_str(), port)){
		TCP_DEBUG_I("启动端口监听 [" << ip << ":" << port << "]");
		return FrTcpLinker::start(ip, port);
	}
	else{
		TCP_DEBUG_I("监听失败");
		return false;
	}
	return true;
}

bool FrTcpServer::stop(){
	if(server_->Stop()){
		TCP_DEBUG_I("停止server.");
		return FrTcpLinker::stop();
	}
	return false;
}

bool FrTcpServer::Disconnect(Socket socket){
	if(int(socket) < 0){
		TCP_DEBUG_E("错误的socket uint值为[" << socket << "] int值为[" << (int)socket << "]");
		return false;
	}
	TCP_DEBUG_I("服务端主动断开 socket[" << socket << "]连接");
	return server_->Disconnect(socket);
}

bool FrTcpServer::Send(Socket socket, const BinaryMemory &binary){
	if(socket <= 0){
#ifdef _DEBUG
		assert(1);
#else
		return false;
#endif
	}
	if(!binary.empty() && binary.size() <= server_->GetSocketBufferSize()){
		Byte pHead[PROTO_HEAD_SIZE] = {0};
		if(GetHead(pHead, binary) && server_->Send(socket, pHead, PROTO_HEAD_SIZE)){
			int offset(0);
			int curSize(0);
			while(offset < binary.size()){
				curSize = (binary.size() - offset) > max_packet_size_ ? max_packet_size_ : (binary.size() - offset);
				if(server_->Send(socket, (const Byte*)binary.buffer(), curSize, offset)){
					offset += max_packet_size_;
				}
				else{
					TCP_DEBUG_E("发送包失败。打包之前的长度" << binary.size() << "包长度为" << binary.size() << " 数据包头的命令号为 [" << *(short*)binary.buffer() << "]");
					return false;
				}
			}
		}
		else{
			TCP_DEBUG_E("发送包头失败(size is "<< binary.size() << "),请检查socket[" << socket << "]链接是否已经断开.");
			return false;
		}
	}
	else{
		TCP_DEBUG_I("丢弃包,包长度[" << binary.size() << "].");
		return false;
	}
	return true;
}

bool FrTcpServer::SendGroup(const vector<Socket> &sockets, const BinaryMemory &binary){
	bool bRet(true);
	for(vector<Socket>::const_iterator citer = sockets.begin(); citer != sockets.end(); ++citer){
		if(!FrTcpServer::Send(*citer, binary)){
			TCP_DEBUG_W("向[" << *citer << "]发送消息失败。请检查连接是否可用。");
			bRet = false;
		}
	}
	return bRet;
}

bool FrTcpServer::sendAll(const universal::BinaryMemory &binary){
	assert(1);
	/*
	Socket* pSockets = new Socket[server_->GetConnectionCount()];
	uint count(0);
	if(server_->GetAllConnectionIDs(pSockets, count)){
		for(uint i=0; i<count; ++i){
			if(!FrTcpServer::Send(pSockets[i], binary)){
				TCP_DEBUG_W("向[" << pSockets[i] << "]发送消息失败。请检查连接是否可用。");
			}
		}
	}
	if(pSockets != NULL){
		delete pSockets; pSockets = NULL;
	}
	*/
	return true;
}

bool FrTcpServer::GetRemoteAddress(Socket socket, std::string& ip, Port& port){
	char sAddress[20];
	int iAddressLen = sizeof(sAddress) / sizeof(char);
	if(server_->GetRemoteAddress(socket, sAddress, iAddressLen, port)){
		ip = string(sAddress, iAddressLen);
		return true;
	}
	return false;
}

int FrTcpServer::OnConnect(Socket socket){ return 0; }
int FrTcpServer::OnDisconnect(Socket socket){ return 0; }
int FrTcpServer::OnSend(Socket socket){ return 0; }
int FrTcpServer::OnReceive(Socket socket, const universal::BinaryMemoryPtr &pBinary){ return 0; }

EnHandleResult FrTcpServer::OnPrepareListen(ITcpServer* pSender, SOCKET soListen){
	char sAddress[20];
	int iAddressLen = sizeof(sAddress) / sizeof(char);
	unsigned short port;
			
	pSender->GetListenAddress(sAddress, iAddressLen, port);
	TCP_DEBUG_I("开始监听端口 [" << string(sAddress, iAddressLen - 1) << ":" << port << "]");
	return HR_OK;
}

EnHandleResult FrTcpServer::OnAccept(ITcpServer* pSender, Socket socket, SOCKET soClient){
	char sAddress[20];
	int iAddressLen = sizeof(sAddress) / sizeof(char);
	unsigned short port;

	pSender->GetRemoteAddress(socket, sAddress, iAddressLen, port);
	TCP_DEBUG_I("接收客户端连接请求.[" << string(sAddress, iAddressLen - 1) << ":" << port << "]");

	OnConnect(socket);
	return HR_OK;
}

EnHandleResult FrTcpServer::OnSend(ITcpServer* pSender, Socket socket, const BYTE* pData, int iLength){
	OnSend(socket);
	return HR_OK;
}

EnHandleResult FrTcpServer::OnReceive(ITcpServer* pSender, Socket socket, int iLength){
	ITcpPullServer* pServer	= ITcpPullServer::FromS(pSender);
	if(pServer != NULL){
		Byte pHead[PROTO_HEAD_SIZE] = {0};
		while((unsigned int)iLength >= PROTO_HEAD_SIZE && HR_OK == pServer->Peek(socket, pHead, PROTO_HEAD_SIZE)){
			size_t size = proto_bypeToSize(pHead) + PROTO_HEAD_SIZE; 
			if(size < PROTO_HEAD_SIZE || size >= server_->GetSocketBufferSize()){
				char sAddress[20];
				int iAddressLen = sizeof(sAddress) / sizeof(char);
				unsigned short port;
				pSender->GetRemoteAddress(socket, sAddress, iAddressLen, port);
				TCP_DEBUG_E("收到的包尺寸过大，自动断开连接。连接 ip[" << string(sAddress, iAddressLen - 1) << ":" << port << "]");
				Disconnect(socket);
				return HR_ERROR;
			}

			if(size <= (size_t)iLength){
				BinaryMemoryPtr pBinary(new BinaryMemory());
				pBinary->resize(size);
				if(HR_OK == pServer->Fetch(socket, (Byte*)pBinary->buffer(), size)){
					iLength -= size;

					pBinary->del(0, PROTO_HEAD_SIZE);
					OnReceive(socket, pBinary);
				}
				else{
					return HR_ERROR;
				}
			}
			else{
				break;
			}
		}
	}
	return HR_OK;
}

EnHandleResult FrTcpServer::OnClose(ITcpServer* pSender, Socket socket, EnSocketOperation enOperation, int iErrorCode){
	char sAddress[20];
	int iAddressLen = sizeof(sAddress) / sizeof(char);
	unsigned short port;

	pSender->GetRemoteAddress(socket, sAddress, iAddressLen, port);
	TCP_DEBUG_I("服务端[" << string(sAddress, iAddressLen - 1) << ":" << port << "]连接断开 HPScoket errorCode [" << iErrorCode << "]");
	OnDisconnect(socket);
	return HR_OK;
}

EnHandleResult FrTcpServer::OnShutdown(ITcpServer* pSender){
	TCP_DEBUG_I("连接关闭");
	return HR_OK;
}



