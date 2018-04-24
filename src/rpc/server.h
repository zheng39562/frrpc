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
#include <functional>

#include "frtemplate/lock_queue.hpp"
#include "frpublic/pub_memory.h"
#include "frpublic/pub_tool.h"
#include "closure.h"
#include "net_server.h"

#include "frrpc_function.h"
#include "rpc_base_net.h"

namespace frrpc{

// class ServerOption{{{1
class ServerOption{
	public:
		// Default : single thread and gate module.
		ServerOption():work_thread_num(1),service_addr() { ; }
		ServerOption(size_t _work_thread_num):work_thread_num(_work_thread_num),service_addr() { ; }
		ServerOption(const std::string& _service_addr):work_thread_num(1),service_addr(_service_addr) { ; }
		ServerOption(size_t _work_thread_num, const std::string& _service_addr):work_thread_num(_work_thread_num),service_addr(_service_addr) { ; }
		~ServerOption()=default;
	public:
		size_t work_thread_num;
		std::string service_addr;
};
// }}}1


// class RpcMessage{{{1
// Rpc Message is used to CallMethod.
class RpcMessage{
	public:
		RpcMessage(){ Clear(); }
		~RpcMessage(){ Clear(); }
	public:
		inline bool IsCompleted(){ return rpc_meta.rpc_request_meta().request_id() != RPC_REQUEST_ID_NULL && method_descriptor != NULL && request != NULL && response != NULL; }

		// clear all.
		inline void Clear(){
			rpc_meta.Clear();
			method_descriptor = NULL;
			DELETE_POINT_IF_NOT_NULL(request);
			DELETE_POINT_IF_NOT_NULL(response);
		}
	public:
		RpcMeta rpc_meta;
		const ::google::protobuf::MethodDescriptor* method_descriptor;
		::google::protobuf::Message* request;
		::google::protobuf::Message* response;
};

std::shared_ptr<RpcMessage> RpcMessagePtr;

// }}}1


// class Server {{{1
class Server{
	public:
		Server(ServerOption& option);
		~Server();
	public:
		bool AddService(::google::protobuf::Service* service);

		bool StartServer(const std::string& ip, Port port);
		bool StartGate(const std::vector<std::tuple<std::string, Port> >& gate_list);
		bool StartMQ(const std::vector<std::tuple<const std::string&, Port> >& mq_list);

		bool Stop();

		bool RunUntilQuit();

		// Is you want receive network event. You need register a callback.
		// Event does not include method of rpc.
		inline void RegisterNetEvent(std::function<void(const Controller* cntl)> net_event_cb){ net_event_cb_ = net_event_cb; }

		bool SendRpcMessage(frrpc::Controller* cntl, const std::string& service_name, const std::string& method_name, const ::google::protobuf::Message& response);
	private:
		GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(Server);

		bool InitThreads(ServerOption& option);
		
		void ReleaseRpcResource(Controller* cntl, RpcMessage* rpc_message);
		
		google::protobuf::Service* GetServiceFromName(const std::string& service_name);
		bool ParseBinary(const frrpc::RpcPacketPtr& packet, RpcMessage& rpc_message, google::protobuf::Service** service);
	private:
		frrpc::network::RpcNetServer* rpc_net_server_;
		ServerOption option_;
		std::map<std::string, ::google::protobuf::Service*> name_2service_;
		std::vector<std::thread> work_threads_;
		eCompressType compress_type_; 
		std::function<void(const Controller* cntl)> net_event_cb_;
};
// Server }}}1

}// namespace frrpc{

#endif 

