/*
 * author zheng39562@163.com
 * date 2016/10/26
 * version v.1.0
 */
#ifndef _tcp_server_H
#define _tcp_server_H

#include <iostream>
#include "fr_tcp/tcp_link.h"

//! \brief	tcp	������
//! \note	ְ��
//! \note		* �����˿ڡ�
//! \note		* �շ����ݰ�����֤���ݰ��������Ժ���ȷ�ԡ�
//! \note	�̳��� CTcpPullServerListener
//! \todo	��δ����������֤�������Ҫ����˽��Э�飬��Ҫ�Ϳͻ���Э����֤�ṹ��
class FrTcpServer : public FrTcpLinker, public CTcpPullServerListener{
	public:
		FrTcpServer();
		FrTcpServer(const FrTcpServer &ref);
		FrTcpServer& operator=(const FrTcpServer &ref);
		~FrTcpServer();
	public:
		virtual bool start(const std::string &ip, unsigned int port);
		virtual bool stop();

		//! \brief	�Ͽ�ĳ�����ӡ�
		bool Disconnect(Socket socket);

		//! \brief	�������ݡ�
		//! \note	��Ϣ���Ͳ���Ҫ���. �����Ϊ���ڲ����У���ǰΪ3�ֽڵ�ͷ����
		bool Send(Socket socket, const universal::BinaryMemory &binary);
		//! \brief	����socketȺ����
		bool SendGroup(const std::vector<Socket> &sockets, const universal::BinaryMemory &binary);
		//! \brief	�㲥
		bool sendAll(const universal::BinaryMemory &binary);
		//! \brief	
		bool GetRemoteAddress(Socket socket, std::string& ip, Port& port);
	protected:
		virtual int OnConnect(Socket socket);
		virtual int OnDisconnect(Socket socket);
		virtual int OnSend(Socket socket);
		virtual int OnReceive(Socket socket, const universal::BinaryMemoryPtr &pBinary);
	private:
		//! \brief	�ص��������ǻ���HPSocket�ģ����������鿴HPSocket�ĵ���
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


