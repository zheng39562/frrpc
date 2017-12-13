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

// 职责：
//	主动连接
//	发送和接收数据包。保证数据包的完整性和正确性。
// 基于HPSocket的client和server分离 无法公共继承。
class FrTcpClient : public FrTcpLinker, public CTcpPullClientListener{
	public:
		FrTcpClient();
		FrTcpClient(const FrTcpClient& ref)=delete;
		FrTcpClient& operator=(const FrTcpClient& ref)=delete;
		~FrTcpClient();
	public:
		virtual bool Start(const std::string& ip, Port port)=0;
		virtual void Stop()=0;
		virtual bool Disconnect(Socket socket);

		virtual bool Send(Socket socket, const BinaryMemory& binary)=0;
		virtual bool SendGroup(const vector<Socket>& socket, const BinaryMemory& binary)=0;
		virtual bool GetRemoteAddress(Socket socket, std::string& ip, Port& port)=0;
	protected:
		virtual int OnConnect(Socket socket);
		virtual int OnDisconnect(Socket socket);
		virtual int OnSend(Socket socket);
		virtual int OnReceive(Socket socket, const universal::BinaryMemoryPtr &pBinary);
	private:
		// 回调函数都是基于HPSocket的，具体机制请查看HPSocket文档。
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

