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

enum eLinkType{
	eLinkType_Server = 0,
	eLinkType_Gate,
	eLinkType_MQ,
	eLinkType_End
}

class ServerOption{
	public:
		// 默认为单线和网关模式.
		ServerOption():work_thread_num(1), receive_type(eLinkType_Gate) { ; }
		~ServerOption()=default;
	public:
		uint32_t work_thread_num;
		eLinkType link_type;
};

/// Server {{{1
class Server{
	public:
		friend bool frrpc::RunUntilStop();
	public:
		Server();
		Server(ServerOption& option);
		~Server();
	public:
		bool AddService(::google::protobuf::Service* service_);
		bool Start(const string& ip, Port port);
	private:
		/// 初始化函数{{{2
		bool Init(ServerOption& option);
		NetLink* CreateNetLink(eLinkType link_type);
		/// }}}2
		
		/// 操作函数{{{2
		void Stop();
		/// }}}2
		
		/// 二进制消息解析函数集{{{2
		const ::google::protobuf::Service* GetServiceFromName(const std::string& service_name);
		const ::google::protobuf::Message* CreateRequest(::google::protobuf::MethodDescriptor* method_descriptor, const char* buffer, uint32_t size);
		::google::protobuf::Message* CreateResponse(::google::protobuf::MethodDescriptor* method_descriptor);
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

