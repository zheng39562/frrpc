/*
 * author zheng39562@163.com
 * date 2016/10/26
 * version v.1.0
 */
#ifndef _tcp_server_H
#define _tcp_server_H

#include <iostream>
#include "fr_tcp/tcp_link.h"

//! \brief	tcp	服务类
//! \note	职责：
//! \note		* 监听端口。
//! \note		* 收发数据包。保证数据包的完整性和正确性。
//! \note	继承于 CTcpPullServerListener
//! \todo	还未增加数据验证，如果需要增加私有协议，需要和客户端协商验证结构。
class FrTcpServer : public FrTcpLinker, public CTcpPullServerListener{
	public:
		FrTcpServer();
		FrTcpServer(const FrTcpServer &ref);
		FrTcpServer& operator=(const FrTcpServer &ref);
		~FrTcpServer();
	public:
		virtual bool start(const std::string &ip, unsigned int port);
		virtual bool stop();

		//! \brief	断开某个连接。
		bool Disconnect(Socket socket);

		//! \brief	发送数据。
		//! \note	消息发送不需要封包. 封包行为在内部进行（当前为3字节的头）。
		bool Send(Socket socket, const universal::BinaryMemory &binary);
		//! \brief	根据socket群发。
		bool SendGroup(const std::vector<Socket> &sockets, const universal::BinaryMemory &binary);
		//! \brief	广播
		bool sendAll(const universal::BinaryMemory &binary);
		//! \brief	
		bool GetRemoteAddress(Socket socket, std::string& ip, Port& port);
	protected:
		virtual int OnConnect(Socket socket);
		virtual int OnDisconnect(Socket socket);
		virtual int OnSend(Socket socket);
		virtual int OnReceive(Socket socket, const universal::BinaryMemoryPtr &pBinary);
	private:
		//! \brief	回调函数都是基于HPSocket的，具体机制请查看HPSocket文档。
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


