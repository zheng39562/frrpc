/**********************************************************
 *  \file server.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _server_H
#define _server_H

#include <thread>

#include "fr_template/lock_queue.hpp"
#include "fr_public/pub_memory.h"

#include "frrpc_function.h"
#include "protocol.h"
#include "net_link.h"

namespace frrpc{

class ServerOption{
	public:
		ServerOption();
		~ServerOption()=default;
	public:
		uint32_t work_thread_num;
};

/// Server {{{1
//! \brief	
//! \note	
class Server{
	public:
		friend bool frrpc::RunUntilStop();
	public:
		Server();
		Server(ServerOption& option);
		~Server();
	public:
		//! \brief	
		bool AddService(::google::protobuf::Service* service_);
		//! \brief	
		bool Start(const string& ip, Port port);
	private:
		/// 初始化函数{{{2
		//! \brief	
		void Init(ServerOption& option);
		/// }}}2
		
		/// 操作函数{{{2
		//! \brief	
		void Stop();
		/// }}}2
		
		/// 二进制消息解析函数集{{{2
		//! \brief	
		const ::google::protobuf::Service* GetServiceFromName(const std::string& service_name);
		//! \brief	
		const ::google::protobuf::Message* CreateRequest(::google::protobuf::MethodDescriptor* method_descriptor, const char* buffer, uint32_t size);
		//! \brief	
		::google::protobuf::Message* CreateResponse(::google::protobuf::MethodDescriptor* method_descriptor);
		//! \brief	
		bool ParseBinary(const BinaryMemory& binary, RpcMessage& rpc_message, ::google::protobuf::Service* Service);
		/// }}}2
	private:
		NetLink* net_link_;
		ServerOption option_;
		std::map<std::string, ::google::protobu::Service*> name_2service_;
		std::vector<std::thread> work_threads_;
		DynamicMessageFactory message_factory_;
};
/// Server }}}1

}// namespace frrpc{

#endif 

