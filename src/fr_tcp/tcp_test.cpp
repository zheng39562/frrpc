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
	TCP_DEBUG_D("客户端成功连接到服务端. connid is " << dwConnID);
}

void cr_CB(Socket socket, const BinaryMemoryPtr &binary, void* etc){
	//TCP_DEBUG_D("客户端从[" << dwConnID << "]中接收数据.");
	//TCP_DEBUG_D("(client)开始打印数据.");
	TCP_DEBUG_D("Client recv size [" << binary->size() << "] opcode [" << *(short*)binary->buffer() << "]");
}

void sl_CB(CONNID dwConnID){
	TCP_DEBUG_D("服务端成功连接到客户端. connid is " << dwConnID);
}

void sr_CB(CONNID dwConnID, const BinaryMemory &binary){
	TCP_DEBUG_D("服务端从[" << dwConnID << "]中接收数据.");
	TCP_DEBUG_D("(server)开始打印数据.");
	binary.print();

	TCP_DEBUG_D("服务端回发数据.");
	pServer->Send(dwConnID, binary);
}

void doTcpTest(string arg0, string arg1, string arg2, string arg3, string arg4){
	TCP_DEBUG_D("测试开始.");
	
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

	TCP_DEBUG_D("测试结束.");
	TCP_DEBUG_D("等待输入任意字符后结束.");

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

