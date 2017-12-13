/**********************************************************
 *  \file net_server.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _net_server_H
#define _net_server_H

#include "frrpc_define.h"
#include "net_link.h"

// 职责：
//	监听端口。
//	收发数据包。保证数据包的完整性和正确性。
//继承于 CTcpPullServerListener
//
//TODO:
//	还未增加数据验证，如果需要增加私有协议，需要和客户端协商验证结构。
class NetServer : public NetLink, public CTcpPullServerListener{
	public:
		NetServer();
		NetServer(const NetServer &ref)=delete;
		NetServer& operator=(const NetServer &ref)=delete;
		~NetServer();
	public:
		virtual bool Start(const std::string &ip, Port port);
		virtual bool Stop();

		//断开某个连接。
		virtual bool Disconnect(Socket socket);

		// 发送数据。
		// 消息发送不需要封包. 封包行为在内部进行（当前为3字节的头）。
		virtual bool Send(Socket socket, const universal::BinaryMemory &binary);
		// 根据socket群发。
		virtual bool SendGroup(const std::vector<Socket> &sockets, const universal::BinaryMemory &binary);
		virtual bool GetRemoteAddress(Socket socket, std::string& ip, Port& port);
	protected:
		virtual int OnConnect(Socket socket);
		virtual int OnDisconnect(Socket socket);
		virtual int OnSend(Socket socket);
		virtual int OnReceive(Socket socket, const universal::BinaryMemoryPtr &pBinary);
	private:
		// 回调函数都是基于HPSocket的，具体机制请查看HPSocket文档。
		virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen);
		virtual EnHandleResult OnAccept(ITcpServer* pSender, Socket socket, SOCKET soClient);
		virtual EnHandleResult OnSend(ITcpServer* pSender, Socket socket, const BYTE* pData, int iLength);
		virtual EnHandleResult OnReceive(ITcpServer* pSender, Socket socket, int iLength);
		virtual EnHandleResult OnClose(ITcpServer* pSender, Socket socket, EnSocketOperation enOperation, int iErrorCode);
		virtual EnHandleResult OnShutdown(ITcpServer* pSender);

	private:
		CTcpPullServerPtr server_;
		int max_packet_size_;

};

#endif 

