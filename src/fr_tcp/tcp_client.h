/**********************************************************
 *  \!file tcp_client.h
 *  \!brief
 *  \!note	注意事项： 
 * 			1,类中的成员函数中的同名参数的含义完全相同。仅会注释其中一个函数，其他函数则不再重复注释。重名的参数意义不同时，会独立注解。 
 * 			2,第1条的规则同样适用于返回值的含义。 
 * 
 * \!version 
 * * \!author zheng39562@163.com
**********************************************************/
#ifndef _tcp_client_H
#define _tcp_client_H

#include "fr_tcp/tcp_link.h"
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

