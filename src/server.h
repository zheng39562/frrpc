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
		// Default : single thread and gate module.
		ServerOption():work_thread_num(1), receive_type(eLinkType_Gate) { ; }
		~ServerOption()=default;
	public:
		uint32_t work_thread_num;
		eLinkType link_type;
};

/// Server {{{1
class Server{
	public:
		Server();
		Server(ServerOption& option);
		~Server();
	public:
		/// Functions : Start {{{2
		bool AddService(::google::protobuf::Service* service_);
		bool Start(const string& ip, Port port);
		/// }}}2

		/// Functions : Send(Public) {{{2
		//
		inline bool SendRpcMessage(Socket socket, const std::string& service_name, const std::string& method_name, const ::google::protobuf::Message& response){
			return SendRpcMessage({socket}, service_name, method_name, response);
		}
		bool SendRpcMessage(const vector<Socket>& sockets, const std::string& service_name, const std::string& method_name, const ::google::protobuf::Message& response);
		/// }}}2
	private:
		/// Functions : Initialization. {{{2
		bool Init(ServerOption& option);
		NetLink* CreateNetLink(eLinkType link_type);
		/// }}}2
		
		/// Functions : Unclassified {{{2
		void Stop();
		//
		void ReleaseRpcResource(RpcMessage* rpc_message);
		/// }}}2
		
		/// Functions : Original send {{{2
		// Build binary from meta and response
		BinaryMemoryPtr BuildBinaryFromResponse(const ::google::protobuf::Message* meta, const ::google::protobuf::Message* response);
		// Send data.
		bool SendRpcMessage(const RpcMeta* meta, const BinaryMemoryPtr& binary);
		/// }}}2
		
		/// Functions : Parse binary data{{{2
		const ::google::protobuf::Service* GetServiceFromName(const std::string& service_name);
		const ::google::protobuf::Message* CreateRequest(::google::protobuf::MethodDescriptor* method_descriptor, const char* buffer, uint32_t size);
		::google::protobuf::Message* CreateResponse(::google::protobuf::MethodDescriptor* method_descriptor);
		bool ParseBinary(const BinaryMemory& binary, RpcMessage& rpc_message, google::protobuf::Service* service);
		/// }}}2
	private:
		NetLink* net_link_;
		ServerOption option_;
		std::map<std::string, ::google::protobu::Service*> name_2service_;
		std::vector<std::thread> work_threads_;
		eCompressType compress_type_; 
};
/// Server }}}1

}// namespace frrpc{

#endif 

