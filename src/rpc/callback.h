/**********************************************************
 *  \file callback_info.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _callback_H
#define _callback_H

#include <condition_variable>

#include <google/protobuf/service.h>
#include "rpc/controller.h"
#include "rpc/frrpc_define.h"

#include "rpc/net_channel.h"
#include "rpc/controller.h"
#include "rpc/callback.h"
#include "rpc/frrpc_define.h"
#include "rpc/frrpc_function.h"
#include "rpc/controller.h"

namespace frrpc{

class ChannelCallback{
	public:
		ChannelCallback(google::protobuf::Closure* _callback, google::protobuf::RpcController* _cntl, google::protobuf::Message* _response);
		ChannelCallback(const ChannelCallback& ref)=delete;
		ChannelCallback& operator=(const ChannelCallback& ref)=delete;
		~ChannelCallback();
	public:
		void Call();
	public:
		frrpc::ChannelController* cntl;
		google::protobuf::Message* response;
		google::protobuf::Closure* callback;
		std::condition_variable* sync_condition;
};

class Channel;

class CallbackInfo{
	public:
		CallbackInfo(Channel* _channel, const google::protobuf::MethodDescriptor* _method, google::protobuf::Message* _response);
	public:
		inline const std::string& service_name()const{ return service_name_; }
		inline const std::string& method_name()const{ return method_name_; }
		inline const google::protobuf::Message* response()const{ return response_; }

		virtual ChannelCallback* BuildCallback(const frrpc::RpcPacketPtr& package) = 0;

		virtual void Release();
	protected:
		google::protobuf::RpcController* createNewCntl(const frrpc::RpcPacketPtr& package);
		google::protobuf::Message* createNewResponse(const frrpc::RpcPacketPtr& package);
	private:
		Channel* channel_;
		std::string service_name_;
		std::string method_name_;
		google::protobuf::Message* response_;
};

template <typename RspClass>
class RegisterCallbackInfo_Func : public CallbackInfo{
	public:
		typedef void (*RegisterFunction)(frrpc::ChannelController*, RspClass*);
	public:
		RegisterCallbackInfo_Func(Channel* _channel, const google::protobuf::MethodDescriptor* _method, RspClass* _response, RegisterFunction _function)
			:CallbackInfo(_channel, _method, _response), function_(_function){}
	public:
		virtual ChannelCallback* BuildCallback(const frrpc::RpcPacketPtr& package){
			frrpc::ChannelController* cntl = dynamic_cast<frrpc::ChannelController*>(createNewCntl(package));
			RspClass* response = dynamic_cast<RspClass*>(createNewResponse(package));
			return new ChannelCallback(frrpc::NewCallback(function_, cntl, response), cntl, response);
		}
	private:
		RegisterFunction function_;
};

template <typename Pointer, typename Class, typename RspClass>
class RegisterCallbackInfo_Class : public CallbackInfo{
	public:
		typedef void (Class::*RegisterClassFunction)(frrpc::ChannelController*, RspClass*);
	public:
		RegisterCallbackInfo_Class(Channel* _channel, const google::protobuf::MethodDescriptor* _method, RspClass* _response, Pointer _object, RegisterClassFunction _function)
			:CallbackInfo(_channel, _method, _response), object_(_object), function_(_function){}
	public:
		virtual ChannelCallback* BuildCallback(const frrpc::RpcPacketPtr& package){
			frrpc::ChannelController* cntl = dynamic_cast<frrpc::ChannelController*>(createNewCntl(package));
			RspClass* response = dynamic_cast<RspClass*>(createNewResponse(package));
			return new ChannelCallback(frrpc::NewCallback(object_, function_, cntl, response), cntl, response);
		}
	private:
		Pointer object_;
		RegisterClassFunction function_;
};

} // namespace frrpc

#endif 

