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
#include "rpc_base_net.h"

namespace frrpc{


// class ServerOption{{{1
class ServerOption{
	public:
		// Default : single thread and gate module.
		ServerOption():work_thread_num(1), receive_type(eLinkType_Gate) { ; }
		~ServerOption()=default;
	public:
		uint32_t work_thread_num;
};
// }}}1

// class RpcMessage{{{1
// Rpc Message is used to CallMethod.
class RpcMessage{
	public:
		RpcMessage(){ Clear(); }
		~RpcMessage(){ Clear(); }
	public:
		inline bool IsCompleted(){ return request_id != RPC_REQUEST_ID_NULL && rpc_meta != NULL && method_descriptor != NULL && request != NULL && response != NULL; }

		// clear all.
		inline void Clear(){
			request_id = RPC_REQUEST_ID_NULL;
			DELETE_POINT_IF_NOT_NULL(rpc_meta);
			method_descriptor = NULL;
			DELETE_POINT_IF_NOT_NULL(request);
			DELETE_POINT_IF_NOT_NULL(response);
		}
	public:
		int32_t request_id;
		const ::google::protobuf::MethodDescriptor* method_descriptor;
		const ::google::protobuf::Message* request;
		::google::protobuf::Message* response;
};

std::shared_ptr<RpcMessage> RpcMessagePtr;

// }}}1

// class Server {{{1
class Server{
	public:
		Server();
		Server(ServerOption& option);
		~Server();
	public:
		// Functions : Start {{{2
		bool AddService(::google::protobuf::Service* service_);

		bool StartServer(const std::string& ip, Port port);
		bool StartGate(const vector<tuple<const std::string&, Port> >& gate_list);
		bool StartMQ(const vector<tuple<const std::string&, Port> >& mq_list);

		void Stop();
		/// }}}2

		// Functions : Send(Public) {{{2
		//
		inline bool SendRpcMessage(LinkID link_id, const std::string& service_name, const std::string& method_name, const ::google::protobuf::Message& response){
			return SendRpcMessage({link_id}, service_name, method_name, response);
		}
		bool SendRpcMessage(const vector<LinkID>& link_ids, const std::string& service_name, const std::string& method_name, const ::google::protobuf::Message& response);
		/// }}}2
	private:
		GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(Server);

		// Functions : Initialization. {{{2
		bool InitThreads(ServerOption& option);
		NetLink* CreateNetLink(eLinkType link_type);
		/// }}}2
		
		// Functions : Unclassified {{{2
		void ReleaseRpcResource(Controller* cntl, RpcMessage* rpc_message);
		/// }}}2
		
		// Functions : Parse binary data{{{2
		const ::google::protobuf::Service* GetServiceFromName(const std::string& service_name);
		const ::google::protobuf::Message* CreateRequest(::google::protobuf::MethodDescriptor* method_descriptor, const char* buffer, uint32_t size);
		::google::protobuf::Message* CreateResponse(::google::protobuf::MethodDescriptor* method_descriptor);
		bool ParseBinary(const RpcServerPacketPtr& package, RpcMessage& rpc_message, google::protobuf::Service* service);
		/// }}}2
	private:
		RpcBaseNet* rpc_net_;
		ServerOption option_;
		std::map<std::string, ::google::protobu::Service*> name_2service_;
		std::vector<std::thread> work_threads_;
		eCompressType compress_type_; 
};
// Server }}}1

}// namespace frrpc{

#endif 

