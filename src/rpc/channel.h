/**********************************************************
 *  \file channel.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _channel_H
#define _channel_H

#include <vector>
#include <map>
#include <atomic>
#include <queue>
#include <mutex>

#include <google/protobuf/service.h>
#include "public/rpc_serializable.h"
#include "rpc/frrpc_define.h"
#include "rpc/callback.h"
#include "rpc/net_channel.h"

namespace frrpc{

class RequestTimeoutEvent{
	public:
		RequestTimeoutEvent(time_t _timeout, RpcRequestId _request_id):timeout(_timeout), request_id(_request_id){ }
		RequestTimeoutEvent(const RequestTimeoutEvent& ref) = delete;
	public:
		time_t timeout;
		RpcRequestId request_id;
};

class ChannelOption{
	public:
		ChannelOption(std::string _channel_name): channel_name(_channel_name), compress_type(eCompressType_Not) { DefaultInit(); }
		ChannelOption(std::string _channel_name, eCompressType _compress_type): channel_name(_channel_name), compress_type(_compress_type) { DefaultInit(); }
		ChannelOption(const ChannelOption& ref){
			this->channel_name = ref.channel_name;
			this->compress_type = ref.compress_type;
			this->request_timeout = ref.request_timeout;

			this->parse_thread_num = ref.parse_thread_num;
			this->parse_num_once = ref.parse_num_once;
			
			this->work_thread_num = ref.work_thread_num;
			this->deal_msg_num_once = ref.deal_msg_num_once;
			this->thread_sleep_time = ref.thread_sleep_time;
		}
		~ChannelOption() = default;
	private:
		inline void DefaultInit(){
			request_timeout = 1000;

			parse_thread_num = 1;
			parse_num_once = 1000;

			work_thread_num = 1;
			deal_msg_num_once = 20;
			thread_sleep_time = 3;
		}
	public:
		std::string channel_name; 
		eCompressType compress_type;
		int32_t request_timeout;

		int32_t parse_thread_num;	// 解析线程
		int32_t parse_num_once;

		int32_t work_thread_num;	// 运行线程
		int32_t deal_msg_num_once;
		int32_t thread_sleep_time;
};

// Abstract interface for an RPC channel.  An RpcChannel represents a
// communication line to a Service which can be used to call that Service's
// methods.  The Service may be running on another machine.  Normally, you
// should not call an RpcChannel directly, but instead construct a stub Service
// wrapping it.  
// Example :
//   RpcChannel* channel = new MyRpcChannel("remotehost.example.com:1234");
//   MyService* service = new MyService::Stub(channel);
//   service->MyMethod(request, &response, callback);
//
// Example (req-res async): if you want use async solution.You must call RunCallback();
//	 void callback(response, arg1);
//
//   MyService* service = new MyService::Stub(channel);
//   RpcChannel* channel = new MyRpcChannel("remotehost.example.com:1234");
//   service->MyMethod(request, response, callback);
//
//	 channel->RunCallback(); 
//
class Channel : public google::protobuf::RpcChannel {
	private:
		friend class ChannelController;
	public:
		Channel(const ChannelOption& option);
		virtual ~Channel();
	public:
		bool StartRoute(const std::string& ip, Port port);
		bool StartMQ();

		void Stop();

		// Is you want receive network event. You need register a callback.
		// Event does not include method of rpc.
		inline void RegisterNetEvent(std::function<void(const frrpc::eRpcEvent& event)> net_event_cb){ net_event_cb_ = net_event_cb; }

		// Call the given method of the remote service.  The signature of this
		// procedure looks the same as Service::CallMethod(), but the requirements
		// are less strict in one important way:  the request and response objects
		// need not be of any specific class as long as their descriptors are
		// method_descriptor->input_type() and method_descriptor->output_type().
		// you need free(delete) all pointer if arg is pointer.
		virtual void CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* cntl, const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done);

		// async module.
		// 原生异步方案可用，但不建议使用。异步模式的controller, response会内部自行删除。原生方式需要调用者new 而框架内删除。整理思路有冲突.
		template <typename RspClass>
		google::protobuf::RpcController* AsyncRequest(google::protobuf::Service* stub, const std::string& method_name, const google::protobuf::Message* request, 
				void (*function)(frrpc::ChannelController*, RspClass*))
		{
			const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
			frrpc::ChannelController* cntl = new frrpc::ChannelController(this, false);
			RspClass* response = google::protobuf::down_cast<RspClass*>(stub->GetResponsePrototype(method_descriptor).New());
			google::protobuf::Closure* closure = frrpc::NewCallback(function, cntl, response);
			CallMethod(method_descriptor, cntl, request, response, closure);
			return cntl;
		}

		template <typename Pointer, typename Class, typename RspClass>
		google::protobuf::RpcController* AsyncRequest(google::protobuf::Service* stub, const std::string& method_name, const google::protobuf::Message* request, 
				const Pointer& object, void (Class::*method)(google::protobuf::RpcController*, RspClass*))
		{
			const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
			frrpc::ChannelController* cntl = new frrpc::ChannelController(this, false);
			RspClass* response = google::protobuf::down_cast<RspClass*>(stub->GetResponsePrototype(method_descriptor).New());
			google::protobuf::Closure* closure = frrpc::NewCallback(object, method, cntl, response);
			CallMethod(method_descriptor, cntl, request, response, closure);
			return cntl;
		}

		// Register permanet cb function.If Call this twice,last will been used.
		// You need delete argv by youself except response.
		//
		// Attention : response does not deleted in callback.
		template <typename RspClass>
		void RegisterCallback(google::protobuf::Service* stub, const std::string& method_name, void (*function)(frrpc::ChannelController*, RspClass*)){
			const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
			std::string callback_key = ConvertMethodKey(method_descriptor->service()->name(), method_descriptor->index());
			RspClass* response = google::protobuf::down_cast<RspClass*>(stub->GetResponsePrototype(method_descriptor).New());
			RegisterCallback(callback_key, new RegisterCallbackInfo_Func<RspClass>(this, method_descriptor, response, function));

			RPC_DEBUG_I("Register function. callback_key [%s] listen method [%s.%d] response name [%s] addr [%p]", 
					callback_key.c_str(), method_descriptor->full_name().c_str(), method_descriptor->index(), response->GetDescriptor()->full_name().c_str(), response);
		}

		template <typename Pointer, typename Class, typename RspClass>
		void RegisterCallback(google::protobuf::Service* stub, const std::string& method_name, const Pointer& object, void (Class::*function)(frrpc::ChannelController*, RspClass*)){
			const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
			std::string callback_key = ConvertMethodKey(method_descriptor->service()->name(), method_descriptor->index());
			RspClass* response = google::protobuf::down_cast<RspClass*>(stub->GetResponsePrototype(method_descriptor).New());
			RegisterCallback(callback_key, new RegisterCallbackInfo_Class<Pointer, Class, RspClass>(this, method_descriptor, response, object, function));

			RPC_DEBUG_I("Register function. callback_key [%s] listen method [%s.%d] response name [%s] addr [%p]", 
					callback_key.c_str(), method_descriptor->full_name().c_str(), method_descriptor->index(), response->GetDescriptor()->full_name().c_str(), response);
		}

		// bind service addr.
		// default is empty string. route choose any service to send.
		bool RegisterService(::google::protobuf::Service* service, const std::string& service_addr);
	private:
		GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(Channel);

		// clear old callback if it exist.
		bool DelCallback(const std::string& callback_key);

		inline bool IsRequestMode(const frrpc::RpcPacketPtr& package){ return package->rpc_meta.rpc_request_meta().request_id() != RPC_REQUEST_ID_NULL; }

		void RegisterCallback(std::string callback_key, CallbackInfo* callback_info);

		void InitThreads();

		void DoRequestPacket(const frrpc::RpcPacketPtr& package);
		void DoRegisterPacket(const frrpc::RpcPacketPtr& package);

		void ClearTimeoutRequest();
		bool CancelRequest(RpcRequestId request_id);
	private:
		bool init_success_;
		bool run_thread_;
		ChannelOption option_;
		frrpc::network::RpcNetChannel* rpc_net_;

		std::list<std::thread> parse_threads_;
		std::list<std::thread> work_threads_;

		std::atomic<RpcRequestId> request_id_;
		std::mutex mutex_request_map_;
		std::mutex mutex_register_map_;
		std::map<RpcRequestId, ChannelCallback*> request_callback_;
		std::map<std::string, CallbackInfo*> register_callback_;

		frtemplate::LockQueue<ChannelCallback*> already_callback_queue_;
		std::list<RequestTimeoutEvent*> timeout_events_;

		std::function<void(const frrpc::eRpcEvent& event)> net_event_cb_;
};

} //namespace frrpc

#endif

