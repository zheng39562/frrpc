/**********************************************************
 *  \file tcp_link.cpp
 *  \brief
 *  \note	ע����� 
 * 			1,���еĳ�Ա�����е�ͬ�������ĺ�����ȫ��ͬ������ע������һ���������������������ظ�ע�͡������Ĳ������岻ͬʱ�������ע�⡣ 
 * 			2,��1���Ĺ���ͬ�������ڷ���ֵ�ĺ��塣 
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
		TCP_DEBUG_E("��ͷδ�����ڴ档");
		return false;
	}
	short pSize = binary.size();
	memcpy(pHead, &pSize, sizeof(proto_size));
	// Verification
	pHead[PROTO_HEAD_VERIFY_INDEX] = ((Byte*)binary.buffer())[binary.size() - 1];
	return true;
}

