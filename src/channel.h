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
#include "net_link.h"

namespace frrpc{

// channel option.
class ChannelOption{
	public:
		ChannelOption(): link_type(eLinkType_Gate), compress_type(eCompressType_Not) { ; }
		~ChannelOption()=default;
	public:
		eLinkType link_type;
		eCompressType compress_type;
};

class RegisterCallBack{
	public:
		RegisterCallBack(Closure* _callback, google::protobuf::Message* _request): callback(_callback), response(_request) {;}
		~RegisterCallBack()=default;
	public:
		Closure* callback;
		google::protobuf::Message* response;
}

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
//   channel->RegisterCallback(service->FindMethodByName(method_name), response, NewPermanentCallback(callback, response, arg1));
//
//	 channel->RunCallback();
//
//	TODO:
//		* request timeout;
//		* call in multiple thread 
class Channel : public google::protobuf::RpcChannel {
	public:
		Channel(const std::string& ip, Port port);
		Channel(const std::string& ip, Port port, const ChannelOption& option);
		virtual ~Channel();

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
	private:
		GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(Channel);

		// Initialization
		bool Init(const std::string& ip, Port port);

		// 
		NetLink* CreateLink(eLinkType link_type);

		//
		BinaryMemoryPtr BuildBinaryFromRequest(const Message* meta, const Message* request);

		// 
		void ClearCallback(const std::string& callback_key);

		// 
		bool ParseBinaryAndCall(const BinaryMemoryPtr& binary);
	private:
		bool init_success_;
		NetLink* net_link;
		std::map<RpcRequestId, Closure*> request_callback_;
		map<std::string, RegisterCallBack> default_callback_;
		ChannelOption option_;
		std::atomic<RpcRequestId> request_id_;
};

} //namespace frrpc

#endif 

