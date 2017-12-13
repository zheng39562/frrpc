/**********************************************************
 *  \!file tcp_client.cpp
 *  \!brief
 *  \!note	注意事项： 
 * 			1,类中的成员函数中的同名参数的含义完全相同。仅会注释其中一个函数，其他函数则不再重复注释。重名的参数意义不同时，会独立注解。 
 * 			2,第1条的规则同样适用于返回值的含义。 
 * 
 * \!version 
 * * \!author zheng39562@163.com
**********************************************************/
#include "fr_tcp/tcp_client.h"

#include "fr_tcp/protocol.h"

using namespace std;
using namespace universal;

FrTcpClient::FrTcpClient()
	:FrTcpLinker(),
	 m_pClient(this),
	 m_Connect(false),
	 max_packet_size_(2048),
	 mutex_(),
	 socket_(-1)
{ ; }

FrTcpClient::FrTcpClient(const std::string &_ip, int _port)
	:FrTcpLinker(_ip, _port),
	 m_pClient(this),
	 m_Connect(false),
	 max_packet_size_(2048),
	 mutex_()
{ ; }

FrTcpClient::~FrTcpClient(){
}

bool FrTcpClient::start(const string &ip, int port){
	bool bAsynConn(false);
	if(!m_pClient->Start(ip.c_str(), port, bAsynConn)){
		TCP_DEBUG_E("连接失败[" << ip << ":" << port << "]");
		return false;
	}
	return FrTcpLinker::start(ip, port);
}

bool FrTcpClient::stop(){
	if(m_pClient->Stop()){
		return FrTcpLinker::stop();
	}
	return false;
}

bool FrTcpClient::Send(const BinaryMemory &binary){
	if(isConnect() && !binary.empty() && binary.size() <= m_pClient->GetSocketBufferSize()){
		std::lock_guard<std::mutex> localLock(mutex_);
		Byte pHead[PROTO_HEAD_SIZE] = {0};
		if(GetHead(pHead, binary) && m_pClient->Send(pHead, 3)){
			int offset(0);
			int curSize(0);
			while(offset < binary.size()){
				curSize = (binary.size() - offset) > max_packet_size_ ? max_packet_size_ : (binary.size() - offset);
				if(m_pClient->Send((const Byte*)binary.buffer(), curSize, offset)){
					offset += max_packet_size_;
				}
				else{
					TCP_DEBUG_E("发送包失败。打包之前的长度" << binary.size() << "包长度为" << binary.size() << " 数据包头的命令号为 [" << *(short*)binary.buffer() << "]");
					return false;
				}
			}
		}
		else{
			TCP_DEBUG_E("发送包头失败,请检查链接是否已经断开.");
			return false;
		}
	}
	else{
		TCP_DEBUG_I("丢弃包,包长度[" << binary.size() << "].");
		return false;
	}
	return true;
}

int FrTcpClient::OnConnect(Socket socket){ return 0; }
int FrTcpClient::OnDisconnect(Socket socket){ return 0; }
int FrTcpClient::OnSend(Socket socket){ return 0; }
int FrTcpClient::OnReceive(Socket socket, const universal::BinaryMemoryPtr &pBinary){ return 0; }

EnHandleResult FrTcpClient::OnConnect(ITcpClient* pSender, Socket socket){
	char sAddress[20];
	int iAddressLen = sizeof(sAddress) / sizeof(char);
	unsigned short port;

	pSender->GetRemoteHost(sAddress, iAddressLen, port);
	TCP_DEBUG_I("连接服务器成功。 [" << string(sAddress, iAddressLen - 1) << ":" << port << "]");

	m_Connect = true;
	socket_ = socket;
	OnConnect(socket);
	return HR_OK;
}

EnHandleResult FrTcpClient::OnSend(ITcpClient* pSender, Socket socket, const BYTE* pData, int iLength){
	OnSend(socket);
	return HR_OK;
}

EnHandleResult FrTcpClient::OnReceive(ITcpClient* pSender, Socket socket, int iLength){
	ITcpPullClient* pClient	= ITcpPullClient::FromS(pSender);
	if(pClient != NULL){
		Byte pHead[PROTO_HEAD_SIZE] = {0};
		while((unsigned int)iLength >= PROTO_HEAD_SIZE && HR_OK == pClient->Peek(pHead, PROTO_HEAD_SIZE)){
			size_t size = proto_bypeToSize(pHead) + PROTO_HEAD_SIZE;
			if(size < PROTO_HEAD_SIZE || size >= m_pClient->GetSocketBufferSize()){
				stop(); 
				TCP_DEBUG_E("尺寸大小TCP缓存 自动断开连接.");
				return HR_ERROR;
			}

			if(size <= (size_t)iLength){
				BinaryMemoryPtr pBinary(new BinaryMemory());
				pBinary->resize(size);
				if(HR_OK == pClient->Fetch((Byte*)pBinary->buffer(), size)){
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

EnHandleResult FrTcpClient::OnClose(ITcpClient* pSender, Socket socket, EnSocketOperation enOperation, int iErrorCode){
	char sAddress[20];
	int iAddressLen = sizeof(sAddress) / sizeof(char);
	unsigned short port;

	pSender->GetRemoteHost(sAddress, iAddressLen, port);
	TCP_DEBUG_I("客户端[" << string(sAddress, iAddressLen - 1) << ":" << port << "]连接断开 HPScoket errorCode [" << iErrorCode << "]");

	m_Connect = false;
	OnDisconnect(socket);
	return HR_OK;
}

