/**********************************************************
 *  \!file tcp_client.h
 *  \!brief
 *  \!note	ע����� 
 * 			1,���еĳ�Ա�����е�ͬ�������ĺ�����ȫ��ͬ������ע������һ���������������������ظ�ע�͡������Ĳ������岻ͬʱ�������ע�⡣ 
 * 			2,��1���Ĺ���ͬ�������ڷ���ֵ�ĺ��塣 
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
//! \note	ְ��
//				* ��������
//				* ���ͺͽ������ݰ�����֤���ݰ��������Ժ���ȷ�ԡ�
//	\note	����HPSocket��client��server���� �޷������̳С�
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

		//! \brief	�������ݡ�
		bool Send(const universal::BinaryMemory &binary);
	protected:
		virtual int OnConnect(Socket socket);
		virtual int OnDisconnect(Socket socket);
		virtual int OnSend(Socket socket);
		virtual int OnReceive(Socket socket, const universal::BinaryMemoryPtr &pBinary);
	private:
		//! \brief	�ص��������ǻ���HPSocket�ģ����������鿴HPSocket�ĵ���
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

