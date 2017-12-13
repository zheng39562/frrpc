/**********************************************************
 *  \file net_client.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _net_client_H
#define _net_client_H

#include <mutex>
#include <thread>

//! \brief	TCP client
//! \note	职责：
//				* 主动连接
//				* 发送和接收数据包。保证数据包的完整性和正确性。
//	\note	基于HPSocket的client和server分离 无法公共继承。
class FrTcpClient : public FrTcpLinker, public CTcpPullClientListener{
	public:
		FrTcpClient();
		FrTcpClient(const std::string &_ip, int port);
		FrTcpClient(const FrTcpClient& ref)=delete;
		FrTcpClient& operator=(const FrTcpClient& ref)=delete;
		~FrTcpClient();
	public:
		inline bool start(){ return start(ip(), port()); }
		virtual bool start(const std::string &ip, int port);
		virtual bool stop();

		inline bool isConnect()const{ return m_Connect; }
		inline Socket socket()const{ return socket_; }

		//! \brief	发送数据。
		bool Send(const universal::BinaryMemory &binary);
	protected:
		virtual int OnConnect(Socket socket);
		virtual int OnDisconnect(Socket socket);
		virtual int OnSend(Socket socket);
		virtual int OnReceive(Socket socket, const universal::BinaryMemoryPtr &pBinary);
	private:
		//! \brief	回调函数都是基于HPSocket的，具体机制请查看HPSocket文档。
		virtual EnHandleResult OnConnect(ITcpClient* pSender, Socket socket);
		virtual EnHandleResult OnSend(ITcpClient* pSender, Socket socket, const BYTE* pData, int iLength);
		virtual EnHandleResult OnReceive(ITcpClient* pSender, Socket socket, int iLength);
		virtual EnHandleResult OnClose(ITcpClient* pSender, Socket socket, EnSocketOperation enOperation, int iErrorCode);
	private:
		CTcpPullClientPtr m_pClient;
		bool m_Connect;
		int max_packet_size_;
		std::mutex mutex_;
		Socket socket_;
};

#endif 

