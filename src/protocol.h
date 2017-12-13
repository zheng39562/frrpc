/**********************************************************
 *  \file protocol.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _protocol_H
#define _protocol_H

#include <memory>
#include <google/protobuf/message.h>
#include "frrpc_define.h"
#include "frrpc.pb.h"

namespace frrpc{

//! \brief	rpc的基本结构.
class RpcMessage{
	public:
		RpcMessage();
		~RpcMessage();
	public:
		bool IsCompleted();
		void Clear();
	public:
		Scoket socket;
		const ::google::protobuf::MethodDescriptor* method_descriptor;
		const ::google::protobuf::Message* request;
		::google::protobuf::Message* response;
};

std::shared_ptr<RpcMessage> RpcMessagePtr;

} // namespace frrpc

#endif 

