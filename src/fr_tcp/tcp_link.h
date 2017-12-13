/**********************************************************
 *  \file tcp_link.h
 *  \brief
 *  \note	注意事项： 
 * 			1,类中的成员函数中的同名参数的含义完全相同。仅会注释其中一个函数，其他函数则不再重复注释。重名的参数意义不同时，会独立注解。 
 * 			2,第1条的规则同样适用于返回值的含义。 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _tcp_link_H
#define _tcp_link_H

#include "fr_tcp/tcp_define.h"
#include "fr_public/pub_memory.h"

//! \brief	tcp连接基类。
//! \note	确定 server 和client 共有的行为和数据。
class FrTcpLinker{
	public:
		FrTcpLinker();
		FrTcpLinker(const std::string &_ip, int _port);
		FrTcpLinker(const FrTcpLinker &ref);
		FrTcpLinker& operator=(const FrTcpLinker &ref);
		virtual ~FrTcpLinker()=default;
	public:
		//! \brief	开始连接：操作意义需要派生类定义
		virtual bool start(const std::string &ip, int port);
		//! \brief	结束连接：操作意义需要派生类定义(要与start函数对应)
		virtual bool stop();

		inline std::string ip()const{ return ip_; }
		inline int port()const{ return port_; }
	protected:
		//! \brief	注意继承时，不要继承错了
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

