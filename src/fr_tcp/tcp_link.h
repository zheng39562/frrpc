/**********************************************************
 *  \file tcp_link.h
 *  \brief
 *  \note	ע����� 
 * 			1,���еĳ�Ա�����е�ͬ�������ĺ�����ȫ��ͬ������ע������һ���������������������ظ�ע�͡������Ĳ������岻ͬʱ�������ע�⡣ 
 * 			2,��1���Ĺ���ͬ�������ڷ���ֵ�ĺ��塣 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _tcp_link_H
#define _tcp_link_H

#include "fr_tcp/tcp_define.h"
#include "fr_public/pub_memory.h"

//! \brief	tcp���ӻ��ࡣ
//! \note	ȷ�� server ��client ���е���Ϊ�����ݡ�
class FrTcpLinker{
	public:
		FrTcpLinker();
		FrTcpLinker(const std::string &_ip, int _port);
		FrTcpLinker(const FrTcpLinker &ref);
		FrTcpLinker& operator=(const FrTcpLinker &ref);
		virtual ~FrTcpLinker()=default;
	public:
		//! \brief	��ʼ���ӣ�����������Ҫ�����ඨ��
		virtual bool start(const std::string &ip, int port);
		//! \brief	�������ӣ�����������Ҫ�����ඨ��(Ҫ��start������Ӧ)
		virtual bool stop();

		inline std::string ip()const{ return ip_; }
		inline int port()const{ return port_; }
	protected:
		//! \brief	ע��̳�ʱ����Ҫ�̳д���
		virtual int OnConnect(Socket socket);
		virtual int OnDisconnect(Socket socket);
		virtual int OnSend(Socket socket);
		virtual int OnReceive(Socket socket, const universal::BinaryMemoryPtr &pBinary);

		bool GetHead(Byte* pHead, const universal::BinaryMemory &binary);
	private:
		std::string ip_;
		int port_;
};


#endif 

