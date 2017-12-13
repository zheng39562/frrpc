/**********************************************************
 *  \file tcp_test.cpp
 *  \brief
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "fr_tcp/tcp_test.h"

#include "fr_tcp/tcp_server.h"
#include "fr_tcp/tcp_client.h"
#include "fr_public/pub_memory.h"
#include "fr_public/pub_timer.h"

using namespace std;
using namespace universal;

FrTcpServer* pServer;
FrTcpClient* pClient;

void cl_CB(CONNID dwConnID){
	TCP_DEBUG_D("�ͻ��˳ɹ����ӵ������. connid is " << dwConnID);
}

void cr_CB(Socket socket, const BinaryMemoryPtr &binary, void* etc){
	//TCP_DEBUG_D("�ͻ��˴�[" << dwConnID << "]�н�������.");
	//TCP_DEBUG_D("(client)��ʼ��ӡ����.");
	TCP_DEBUG_D("Client recv size [" << binary->size() << "] opcode [" << *(short*)binary->buffer() << "]");
}

void sl_CB(CONNID dwConnID){
	TCP_DEBUG_D("����˳ɹ����ӵ��ͻ���. connid is " << dwConnID);
}

void sr_CB(CONNID dwConnID, const BinaryMemory &binary){
	TCP_DEBUG_D("����˴�[" << dwConnID << "]�н�������.");
	TCP_DEBUG_D("(server)��ʼ��ӡ����.");
	binary.print();

	TCP_DEBUG_D("����˻ط�����.");
	pServer->Send(dwConnID, binary);
}

void doTcpTest(string arg0, string arg1, string arg2, string arg3, string arg4){
	TCP_DEBUG_D("���Կ�ʼ.");
	
	string ip("0.0.0.0");
	//string ip("192.168.0.212");
	//unsigned int port(5000);
	unsigned int port(5555);

	pServer = new FrTcpServer();
	pServer->start(ip, port);

	/*
	pClient = new FCTcpClient();
	pClient->setLinkCB(cl_CB);
	pClient->setReceiveCB(cr_CB);
	pClient->start(ip, port);

	string str("123456789abc");
	BinaryMemory binary((void*)str.c_str(), str.size());
	binary.print();
	pClient->Send(binary);
	*/
	
	FrSleep(200000000);
	//pClient->stop();
	pServer->stop();

	TCP_DEBUG_D("���Խ���.");
	TCP_DEBUG_D("�ȴ����������ַ������.");

	delete pServer; pServer = NULL;
	//delete pClient; pClient = NULL;

	int i;
	cin >> i;

}

void doTcpClientTest(string arg0, string arg1, string arg2, string arg3, string arg4){
	pClient = new FrTcpClient();
//	pClient->setCallBack(NULL, NULL, NULL, cr_CB, NULL);
	pClient->start("192.168.0.104", 30001);
}

