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

#include <google/protobuf/service.h>
#include "pb/net.pb.h"
#include "rpc/net_channel.h"
#include "rpc/controller.h"
#include "rpc/frrpc_define.h"
#include "rpc/frrpc_function.h"

namespace frrpc{

class ChannelOption{
	public:
		ChannelOption(): compress_type(eCompressType_Not) { ; }
		~ChannelOption()=default;
	public:
		eCompressType compress_type;
};


class RegisterCallBack{
	public:
		RegisterCallBack(google::protobuf::Closure* _callback, google::protobuf::RpcController* _cntl, google::protobuf::Message* _response)
			:callback(_callback), cntl(_cntl), response(_response) { }
		RegisterCallBack(const RegisterCallBack& ref)=delete;
		RegisterCallBack& operator=(const RegisterCallBack& ref)=delete;
		~RegisterCallBack()=default;
	public:
		void Clear(){
			cntl->Reset();
			response->Clear();
		}
		void Release(){
			DELETE_POINT_IF_NOT_NULL(cntl); 
			DELETE_POINT_IF_NOT_NULL(callback); 
			DELETE_POINT_IF_NOT_NULL(response); 
		}
	public:
		google::protobuf::RpcController* cntl;
		google::protobuf::Closure* callback;
		google::protobuf::Message* response;
};


class RequestCallBack{
	public:
		RequestCallBack(google::protobuf::Closure* _callback, google::protobuf::RpcController* _cntl, google::protobuf::Message* _response)
			:callback(_callback), cntl(_cntl), response(_response){}
		RequestCallBack(const RequestCallBack& ref)=delete;
		RequestCallBack& operator=(const RequestCallBack& ref)=delete;
		~RequestCallBack(){
			DELETE_POINT_IF_NOT_NULL(cntl); 
			DELETE_POINT_IF_NOT_NULL(response); 
		}
	public:
		google::protobuf::RpcController* cntl;
		google::protobuf::Message* response;
		google::protobuf::Closure* callback;
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
// Example (Registe):
//	 void callback(response, arg1);
//
//   MyService* service = new MyService::Stub(channel);
//   RpcChannel* channel = new MyRpcChannel("remotehost.example.com:1234");
//   RegisterRpcMethod(channel, service->FindMethodByName(method_name), callback, arg1);
//
//	 channel->RunCallback();
//
//	TODO:
//		* synchronization mode.
//		* request timeout;
//		* call in multiple thread 
class Channel : public google::protobuf::RpcChannel {
	public:
		Channel(const ChannelOption& option);
		virtual ~Channel();
	public:
		bool StartRoute(const std::string& ip, Port port);
		bool StartMQ();

		void Stop();

		// Is you want receive network event. You need register a callback.
		// Event does not include method of rpc.
		inline void RegisterNetEvent(std::function<void(const network::eNetEvent& event)> net_event_cb){ net_event_cb_ = net_event_cb; }

		// Call the given method of the remote service.  The signature of this
		// procedure looks the same as Service::CallMethod(), but the requirements
		// are less strict in one important way:  the request and response objects
		// need not be of any specific class as long as their descriptors are
		// method->input_type() and method->output_type().
		// you need free(delete) all pointer if arg is pointer.
		virtual void CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* cntl, const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done);

		// async module.
		// @return 返回不允许在外面删除，用于来取消这次request。如果已发出，则可以用来取消回调(未实现).
		google::protobuf::RpcController* SendRequest(google::protobuf::Service* stub, const std::string& method_name, const google::protobuf::Message* request, 
				void (*function)(google::protobuf::RpcController*, google::protobuf::Message*));

		template <typename Pointer, typename Class>
		google::protobuf::RpcController* SendRequest(google::protobuf::Service* stub, const std::string& method_name, const google::protobuf::Message* request, 
				const Pointer& object, void (Class::*method)(google::protobuf::RpcController*, google::protobuf::Message*))
		{
			const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
			google::protobuf::RpcController* cntl = new frrpc::Controller();
			google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
			google::protobuf::Closure* closure = frrpc::NewCallback(object, method, cntl, response);
			CallMethod(method_descriptor, cntl, request, response, closure);
			return cntl;
		}

		// Register permanet cb function.If Call this twice,last will been used.
		// You need delete argv by youself except response.
		//
		// Attention : response does not deleted in callback.
		void RegisterCallback(google::protobuf::Service* stub, const std::string& method_name, void (*function)(google::protobuf::RpcController* ctnl, google::protobuf::Message*));
		template <typename Pointer, typename Class>
		void RegisterCallback(google::protobuf::Service* stub, const std::string& method_name, const Pointer& object, void (Class::*method)(google::protobuf::RpcController* ctnl, google::protobuf::Message*)){
			const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
			google::protobuf::RpcController* cntl = new frrpc::Controller();
			google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
			google::protobuf::Closure* permanet_callback = frrpc::NewPermanentCallback(object, method, cntl, response);
			RegisterCallback(method_descriptor, cntl, response, permanet_callback);
		}

		// bind service addr.
		// default is empty string. route choose any service to send.
		bool RegisterService(::google::protobuf::Service* service, const std::string& service_addr);

		// All callback function will been called.
		// param[in] run_cb_times : The number of calls to call callbacks.
		void RunCallback(uint32_t run_cb_times = 20);

		// Clear All Setting
		//	* Disconnect.
		//	* Clear Callback(Request and Register).
		//	* Reset flag of initial.
		// Attention : This function only clear variables that is in itself.You need delete pointer for youself.
		// TODO:
		//	* Clear part pointer.
		void ClearAllSetting();
	private:
		GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(Channel);

		// clear old callback if it exist.
		bool DelCallback(const std::string& callback_key);

		inline bool IsRequestMode(const frrpc::RpcPacketPtr& package){ return package->rpc_meta.rpc_request_meta().request_id() != RPC_REQUEST_ID_NULL; }

		void RegisterCallback(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* cntl, google::protobuf::Message* response, google::protobuf::Closure* permanet_callback);
	private:
		bool init_success_;
		frrpc::network::RpcNetChannel* rpc_net_;
		std::map<RpcRequestId, RequestCallBack*> request_callback_;
		std::map<std::string, RegisterCallBack*> register_callback_;
		ChannelOption option_;
		std::atomic<RpcRequestId> request_id_;
		std::function<void(const network::eNetEvent& event)> net_event_cb_;
};

/*
// template function list : RegisterRpcMethod
// Zero
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, void (*function)(google::protobuf::Message*)){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(function, response));
}

// Zero
template <typename Class, typename Pointer>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, const Pointer& object, void (Class::*method)(google::protobuf::Message*)){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(object, method, response));
}

// Arg1
template <typename Arg1>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, void (*function)(google::protobuf::Message*, Arg1), Arg1 arg1){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(function, response, arg1));
}

// Arg1
template <typename Class, typename Pointer, typename Arg1>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1), Arg1 arg1){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(object, method, response, arg1));
}

// Arg2
template <typename Arg1, typename Arg2>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, void (*function)(google::protobuf::Message*, Arg1, Arg2), Arg1 arg1, Arg2 arg2){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(function, response, arg1, arg2));
}

// Arg2
template <typename Class, typename Pointer, typename Arg1, typename Arg2>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1, Arg2), Arg1 arg1, Arg2 arg2){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(object, method, response, arg1, arg2));
}

// Arg3
template <typename Arg1, typename Arg2, typename Arg3>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, void(*function(google::protobuf::Message*, Arg1, Arg2, Arg3)), Arg1 arg1, Arg2 arg2, Arg3 arg3){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(function, response, arg1, arg2, arg3));
}

// Arg3
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1, Arg2, Arg3), Arg1 arg1, Arg2 arg2, Arg3 arg3){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(object, method, response, arg1, arg2, arg3));
}

// Arg4
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, void(*function(google::protobuf::Message*, Arg1, Arg2, Arg3, Arg4)), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(function, response, arg1, arg2, arg3, arg4));
}

// Arg4
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1, Arg2, Arg3, Arg4), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(object, method, response, arg1, arg2, arg3, arg4));
}

// Arg5
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, void(*function(google::protobuf::Message*, Arg1, Arg2, Arg3, Arg4, Arg5)), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(function, response, arg1, arg2, arg3, arg4, arg5));
}

// Arg5
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1, Arg2, Arg3, Arg4, Arg5), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(object, method, response, arg1, arg2, arg3, arg4, arg5));
}

// Arg6 
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, void(*function(google::protobuf::Message*, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(function, response, arg1, arg2, arg3, arg4, arg5, arg6));
}

// Arg6 
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(object, method, response, arg1, arg2, arg3, arg4, arg5, arg6));
}

// Arg7 
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, void(*function(google::protobuf::Message*, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(function, response, arg1, arg2, arg3, arg4, arg5, arg6, arg7));
}

// Arg7 
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(object, method, response, arg1, arg2, arg3, arg4, arg5, arg6, arg7));
}

// Arg8 
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, void(*function(google::protobuf::Message*, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(function, response, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
}

// Arg8 
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(object, method, response, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
}

// Arg9
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, void(*function(google::protobuf::Message*, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(function, response, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
}

// Arg9
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
inline void RegisterRpcMethod(Channel& channel, google::protobuf::Service* stub, const std::string& method_name, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9), Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9){
	const google::protobuf::MethodDescriptor* method_descriptor = stub->GetDescriptor()->FindMethodByName(method_name);
	google::protobuf::Message* response = stub->GetResponsePrototype(method_descriptor).New();
	channel.RegisterCallback(method_descriptor, response, frrpc::NewPermanentCallback(object, method, response, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
}

*/

} //namespace frrpc

#endif

