/**********************************************************
 *  \file net_client.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "net_client.h"

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

bool FrTcpClient::Start(const string &ip, int port){
	bool bAsynConn(false);
	if(!m_pClient->Start(ip.c_str(), port, bAsynConn)){
		DEBUG_E("连接失败[" << ip << ":" << port << "]");
		return false;
	}
	return true;
}

bool FrTcpClient::Stop(){
	return m_pClient->Stop();
}

bool FrTcpClient::Disconnect(Socket socket){
	return Stop();
}

bool FrTcpClient::Send(Socket socket, const BinaryMemory &binary){
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
					DEBUG_E("发送包失败。打包之前的长度" << binary.size() << "包长度为" << binary.size() << " 数据包头的命令号为 [" << *(short*)binary.buffer() << "]");
					return false;
				}
			}
		}
		else{
			DEBUG_E("发送包头失败,请检查链接是否已经断开.");
			return false;
		}
	}
	else{
		DEBUG_I("丢弃包,包长度[" << binary.size() << "].");
		return false;
	}
	return true;
}

bool FrTcpClient::SendGroup(const vector<Socket>& socket, const BinaryMemory& binary){
	return Send(0, binary);
}

bool GetRemoteAddress(Socket socket, std::string& ip, Port& port){
	return false;
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
	DEBUG_I("连接服务器成功。 [" << string(sAddress, iAddressLen - 1) << ":" << port << "]");

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
		uint32_t size(0);
		while(iLength > 0 && iLength >= sizeof(size) && HR_OK == pClient->Peek((Byte*)&size, sizeof(size))){
			if(size >= m_pClient->GetSocketBufferSize()){
				stop(); 
				DEBUG_E("尺寸大小TCP缓存 自动断开连接.");
				return HR_ERROR;
			}

			if((size + sizeof(size)) <= (uint32_t)iLength){
				if(HR_OK == pClient->Fetch((Byte*)&size, sizeof(size))){
					iLength -= sizeof(size);
				}

				BinaryMemoryPtr binary(new BinaryMemory());
				binary->resize(size);
				if(HR_OK == pClient->Fetch((Byte*)binary->buffer(), size)){
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

EnHandleResult FrTcpClient::OnClose(ITcpClient* pSender, Socket socket, EnSocketOperation enOperation, int iErrorCode){
	char sAddress[20];
	int iAddressLen = sizeof(sAddress) / sizeof(char);
	unsigned short port;

	pSender->GetRemoteHost(sAddress, iAddressLen, port);
	DEBUG_I("客户端[" << string(sAddress, iAddressLen - 1) << ":" << port << "]连接断开 HPScoket errorCode [" << iErrorCode << "]");

	m_Connect = false;
	OnDisconnect(socket);
	return HR_OK;
}

