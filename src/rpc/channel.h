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

#include <google/protobuf/service.h>

#include "frrpc_define.h"
#include "frrpc_function.h"
#include "rpc_base_net.h"

namespace frrpc{

// class ChannelOption {{{1
class ChannelOption{
	public:
		ChannelOption(): link_type(eLinkType_Gate), compress_type(eCompressType_Not) { ; }
		~ChannelOption()=default;
	public:
		eLinkType link_type;
		eCompressType compress_type;
};
// }}}1

// class RegisterCallBack {{{1
class RegisterCallBack{
	public:
		RegisterCallBack(Closure* _callback, google::protobuf::Message* _request): callback(_callback), response(_request) {;}
		~RegisterCallBack()=default;
	public:
		Closure* callback;
		google::protobuf::Message* response;
};
// }}}1

// class RequestCallBack {{{1
class RequestCallBack{
	public:
		RequestCallBack(Closure* _callback, google::protobuf::Message* _request): callback(_callback), response(_request) {;}
		~RequestCallBack()=default;
	public:
		Closure* callback;
		google::protobuf::Message* response;
};

// }}}1

// Class Channel {{{1
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
//   ReigiterCallBack(channel, service->FindMethodByName(method_name), callback, arg1);
//
//	 channel->RunCallback();
//
//	TODO:
//		* synchronization mode.
//		* request timeout;
//		* call in multiple thread 
class Channel : public google::protobuf::RpcChannel {
	public:
		Channel(const std::string& ip, Port port);
		Channel(const std::string& ip, Port port, const ChannelOption& option);
		virtual ~Channel();
	public:
		// Function : Start {{{2
		// start channle by different link.
		bool StartServer(const std::string& ip, Port port);
		bool StartGate(const std::string& ip, Port port);
		bool StartMQ();

		void Stop();
		// }}}2

		// Call the given method of the remote service.  The signature of this
		// procedure looks the same as Service::CallMethod(), but the requirements
		// are less strict in one important way:  the request and response objects
		// need not be of any specific class as long as their descriptors are
		// method->input_type() and method->output_type().
		// you need free(delete) all pointer if arg is pointer.
		virtual void CallMethod(const MethodDescriptor* method, RpcController* controller, const Message* request, Message* response, Closure* done);

		// Register permanet cb function.If Call this twice,last will been used.
		// You need delete argv by youself except response.
		//
		// Attention : response does not deleted in callback.
		void RegisterCallback(const MethodDescriptor* method, google::protobuf::Message* response, Closure* permanet_callback);

		// All callback function will been called.
		// param[in] run_cb_times : The number of calls to call callbacks.
		void RunCallback(uint32_t run_cb_times = 20);

		// Clear All Setting
		//	* Stop net_link.
		//	* Clear Callback(Request and Register).
		//	* Reset flag of initial.
		// Attention : This function only clear variables that is in itself.You need delete pointer for youself.
		// TODO:
		//	* Clear part pointer.
		void ClearAllSetting();
	private:
		GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(Channel);

		// clear old callback if it exist.
		void ClearCallback(const std::string& callback_key);

		inline IsRequestMode(const RpcServerPacketPtr& package){ return package->rpc_meta.rpc_request_meta().request_id() != RPC_REQUEST_ID_NULL; }
	private:
		bool init_success_;
		RpcBaseNet* rpc_net_;
		std::map<RpcRequestId, RequestCallBack> request_callback_;
		map<std::string, RegisterCallBack> default_callback_;
		ChannelOption option_;
		std::atomic<RpcRequestId> request_id_;
};

// template function list : ReigiterCallBack {{{2
// Zero
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, void(*function(google::protobuf::Message*))){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(callback, response));
}

// Zero
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, const Pointer& object, void (Class::*method)(google::protobuf::Message*)){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(object, method, response));
}

// Arg1
template <typename Arg1>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, void(*function(google::protobuf::Message*, Arg1)), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(callback, response, arg1));
}

// Arg1
template <typename Class, typename Pointer, typename Arg1>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(object, method, response, arg1));
}

// Arg2
template <typename Arg1, typename Arg2>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, void(*function(google::protobuf::Message*, Arg1)), Arg1 arg1, Arg2 arg2){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(callback, response, arg1, arg2));
}

// Arg2
template <typename Class, typename Pointer, typename Arg1, typename Arg2>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(object, method, response, arg1, arg2));
}

// Arg3
template <typename Arg1, typename Arg2, typename Arg3>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, void(*function(google::protobuf::Message*, Arg1)), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(callback, response, arg1, arg2, arg3));
}

// Arg3
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(object, method, response, arg1, arg2, arg3));
}

// Arg4
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, void(*function(google::protobuf::Message*, Arg1)), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(callback, response, arg1, arg2, arg3, arg4));
}

// Arg4
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(object, method, response, arg1, arg2, arg3, arg4));
}

// Arg5
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, void(*function(google::protobuf::Message*, Arg1)), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(callback, response, arg1, arg2, arg3, arg4, arg5));
}

// Arg5
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(object, method, response, arg1, arg2, arg3, arg4, arg5));
}

// Arg6 
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, void(*function(google::protobuf::Message*, Arg1)), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(callback, response, arg1, arg2, arg3, arg4, arg5, arg6));
}

// Arg6 
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(object, method, response, arg1, arg2, arg3, arg4, arg5, arg6));
}

// Arg7 
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, void(*function(google::protobuf::Message*, Arg1)), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(callback, response, arg1, arg2, arg3, arg4, arg5, arg6, arg7));
}

// Arg7 
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(object, method, response, arg1, arg2, arg3, arg4, arg5, arg6, arg7));
}

// Arg8 
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, void(*function(google::protobuf::Message*, Arg1)), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(callback, response, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
}

// Arg8 
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(object, method, response, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
}

// Arg9
template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, void(*function(google::protobuf::Message*, Arg1)), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(callback, response, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
}

// Arg9
template <typename Class, typename Pointer, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
inline void ReigiterCallBack(Channel& channel, const google::protobuf::MethodDescriptor* method, const Pointer& object, void (Class::*method)(google::protobuf::Message*, Arg1), Arg1 arg1){
	google::protobuf::Message* response = new CreateProtoMessage(method->output_type());
	channel.RegisterCallback(method, response, NewPermanentCallback(object, method, response, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
}

// }}}2

// }}}1

} //namespace frrpc

#endif

