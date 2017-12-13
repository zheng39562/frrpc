/**********************************************************
 *  \file net_server.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "net_server.h"


#include "fr_tcp/protocol.h"
#include "fr_public/pub_memory.h"
#include "fr_public/pub_tool.h"

using namespace std;
using namespace universal;

NetServer::NetServer()
	:server_(this),
	 max_packet_size_(2048)
{ ; }

NetServer::NetServer(const NetServer &ref)
	:server_(this)
{ ; }

NetServer& NetServer::operator=(const NetServer &ref){
	return *this;
}

NetServer::~NetServer(){ 
}

bool NetServer::Start(const string &ip, Port port){
	if(!server_->Start(ip.c_str(), port)){
		DEBUG_I("监听失败");
		return false;
	}

	DEBUG_I("启动端口监听 [" << ip << ":" << port << "]");
	return true;
}

bool NetServer::Stop(){
	if(server_->Stop()){
		DEBUG_I("停止server.");
		return FrTcpLinker::stop();
	}
	return false;
}

bool NetServer::Disconnect(Socket socket){
	if(int(socket) < 0){
		DEBUG_E("错误的socket uint值为[" << socket << "] int值为[" << (int)socket << "]");
		return false;
	}
	DEBUG_I("服务端主动断开 socket[" << socket << "]连接");
	return server_->Disconnect(socket);
}

bool NetServer::Send(Socket socket, const BinaryMemory &binary){
	if(socket <= 0){
#ifdef _DEBUG
		assert(1);
#else
		return false;
#endif
	}
	if(!binary.empty() && binary.size() <= server_->GetSocketBufferSize()){
		Byte pHead[PROTO_HEAD_SIZE] = {0};
		if(GetHead(pHead, binary) && server_->Send(socket, pHead, head_length())){
			int offset(0);
			int curSize(0);
			while(offset < binary.size()){
				curSize = (binary.size() - offset) > max_packet_size_ ? max_packet_size_ : (binary.size() - offset);
				if(server_->Send(socket, (const Byte*)binary.buffer(), curSize, offset)){
					offset += max_packet_size_;
				}
				else{
					DEBUG_E("发送包失败。打包之前的长度" << binary.size() << "包长度为" << binary.size() << " 数据包头的命令号为 [" << *(short*)binary.buffer() << "]");
					return false;
				}
			}
		}
		else{
			DEBUG_E("发送包头失败(size is "<< binary.size() << "),请检查socket[" << socket << "]链接是否已经断开.");
			return false;
		}
	}
	else{
		DEBUG_I("丢弃包,包长度[" << binary.size() << "].");
		return false;
	}
	return true;
}

bool NetServer::SendGroup(const vector<Socket> &sockets, const BinaryMemory &binary){
	bool bRet(true);
	for(vector<Socket>::const_iterator citer = sockets.begin(); citer != sockets.end(); ++citer){
		if(!NetServer::Send(*citer, binary)){
			DEBUG_W("向[" << *citer << "]发送消息失败。请检查连接是否可用。");
			bRet = false;
		}
	}
	return bRet;
}

bool NetServer::GetRemoteAddress(Socket socket, std::string& ip, Port& port){
	char sAddress[20];
	int iAddressLen = sizeof(sAddress) / sizeof(char);
	if(server_->GetRemoteAddress(socket, sAddress, iAddressLen, port)){
		ip = string(sAddress, iAddressLen);
		return true;
	}
	return false;
}

int NetServer::OnConnect(Socket socket){ return 0; }
int NetServer::OnDisconnect(Socket socket){ return 0; }
int NetServer::OnSend(Socket socket){ return 0; }
int NetServer::OnReceive(Socket socket, const universal::BinaryMemoryPtr &pBinary){ return 0; }

EnHandleResult NetServer::OnPrepareListen(ITcpServer* pSender, SOCKET soListen){
	char sAddress[20];
	int iAddressLen = sizeof(sAddress) / sizeof(char);
	unsigned short port;
			
	pSender->GetListenAddress(sAddress, iAddressLen, port);
	DEBUG_I("开始监听端口 [" << string(sAddress, iAddressLen - 1) << ":" << port << "]");
	return HR_OK;
}

EnHandleResult NetServer::OnAccept(ITcpServer* pSender, Socket socket, SOCKET soClient){
	char sAddress[20];
	int iAddressLen = sizeof(sAddress) / sizeof(char);
	unsigned short port;

	pSender->GetRemoteAddress(socket, sAddress, iAddressLen, port);
	DEBUG_I("接收客户端连接请求.[" << string(sAddress, iAddressLen - 1) << ":" << port << "]");

	OnConnect(socket);
	return HR_OK;
}

EnHandleResult NetServer::OnSend(ITcpServer* pSender, Socket socket, const BYTE* pData, int iLength){
	OnSend(socket);
	return HR_OK;
}

EnHandleResult NetServer::OnReceive(ITcpServer* pSender, Socket socket, int iLength){
	ITcpPullServer* pServer	= ITcpPullServer::FromS(pSender);
	if(pServer != NULL){
		uint32_t size(0); 
		while(iLength > 0 && iLength >= sizeof(size) && HR_OK == pServer->Peek(socket, (Byte*)&size, sizeof(size))){
			if(size >= server_->GetSocketBufferSize()){
				char sAddress[20];
				int iAddressLen = sizeof(sAddress) / sizeof(char);
				unsigned short port;
				pSender->GetRemoteAddress(socket, sAddress, iAddressLen, port);
				DEBUG_E("收到的包尺寸过大，自动断开连接。连接 ip[" << string(sAddress, iAddressLen - 1) << ":" << port << "]");
				Disconnect(socket);
				return HR_ERROR;
			}

			if((size + sizeof(size)) <= (uint32_t)iLength){
				if(HR_OK == pServer->Fetch(socket, (Byte*)&size, sizeof(size))){
					iLength -= sizeof(size);
				}

				BinaryMemoryPtr binary(new BinaryMemory());
				binary->resize(sizeof(Socket) + size);
				binary->add((void*)&socket, sizeof(socket));
				if(HR_OK == pServer->Fetch(socket, (Byte*)binary->buffer() + sizeof(Socket), size)){
					iLength -= size;

					PushMessageToQueue(binary);
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

EnHandleResult NetServer::OnClose(ITcpServer* pSender, Socket socket, EnSocketOperation enOperation, int iErrorCode){
	char sAddress[20];
	int iAddressLen = sizeof(sAddress) / sizeof(char);
	unsigned short port;

	pSender->GetRemoteAddress(socket, sAddress, iAddressLen, port);
	DEBUG_I("服务端[" << string(sAddress, iAddressLen - 1) << ":" << port << "]连接断开 HPScoket errorCode [" << iErrorCode << "]");
	OnDisconnect(socket);
	return HR_OK;
}

EnHandleResult NetServer::OnShutdown(ITcpServer* pSender){
	DEBUG_I("连接关闭");
	return HR_OK;
}



