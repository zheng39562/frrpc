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

#include <google/protobuf/service.h>

#include "frrpc_define.h"

namespace frrpc{

// Abstract interface for an RPC channel.  An RpcChannel represents a
// communication line to a Service which can be used to call that Service's
// methods.  The Service may be running on another machine.  Normally, you
// should not call an RpcChannel directly, but instead construct a stub Service
// wrapping it.  Example:
//   RpcChannel* channel = new MyRpcChannel("remotehost.example.com:1234");
//   MyService* service = new MyService::Stub(channel);
//   service->MyMethod(request, &response, callback);
class Channel : public google::protobuf::RpcChannel {
	public:
		Channel()
		virtual ~Channel();

		// Call the given method of the remote service.  The signature of this
		// procedure looks the same as Service::CallMethod(), but the requirements
		// are less strict in one important way:  the request and response objects
		// need not be of any specific class as long as their descriptors are
		// method->input_type() and method->output_type().
		virtual void CallMethod(const MethodDescriptor* method, RpcController* controller, const Message* request, Message* response, Closure* done);
	private:
		GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(Channel);
};

} //namespace frrpc

#endif 

