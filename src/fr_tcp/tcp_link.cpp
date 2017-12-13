/**********************************************************
 *  \file tcp_link.cpp
 *  \brief
 *  \note	注意事项： 
 * 			1,类中的成员函数中的同名参数的含义完全相同。仅会注释其中一个函数，其他函数则不再重复注释。重名的参数意义不同时，会独立注解。 
 * 			2,第1条的规则同样适用于返回值的含义。 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "fr_tcp/tcp_link.h"
#include "fr_tcp/protocol.h"

FrTcpLinker::FrTcpLinker() 
	:ip_(),
	 port_(0)
{ ; }

FrTcpLinker::FrTcpLinker(const std::string &_ip, int _port) 
	:ip_(_ip),
	 port_(_port)
{ ; }

FrTcpLinker::FrTcpLinker(const FrTcpLinker &ref)
	:ip_(ref.ip_),
	 port_(ref.port_)
{ ; }

FrTcpLinker& FrTcpLinker::operator=(const FrTcpLinker &ref)
{ 
	ip_ = ref.ip_;
	port_ = ref.port_;
	return *this; 
}

bool FrTcpLinker::start(const std::string &ip, int port){
	ip_ = ip;
	port_ = port;
	return true;
}
bool FrTcpLinker::stop(){
	return true;
}

int FrTcpLinker::OnConnect(Socket socket){ return 0; }
int FrTcpLinker::OnDisconnect(Socket socket){ return 0; }
int FrTcpLinker::OnSend(Socket socket){ return 0; }
int FrTcpLinker::OnReceive(Socket socket, const universal::BinaryMemoryPtr &pBinary){ return 0; }

bool FrTcpLinker::GetHead(Byte* pHead, const universal::BinaryMemory &binary){
	if(binary.size() >= 4096){
#ifdef _DEBUG
		assert(false);
#else
		return false;
#endif
	}

	if(pHead == NULL){
		TCP_DEBUG_E("包头未分配内存。");
		return false;
	}
	short pSize = binary.size();
	memcpy(pHead, &pSize, sizeof(proto_size));
	// Verification
	pHead[PROTO_HEAD_VERIFY_INDEX] = ((Byte*)binary.buffer())[binary.size() - 1];
	return true;
}

