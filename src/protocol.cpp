/**********************************************************
 *  \file protocol.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "protocol.h"

namespace frrpc{

RpcMessage::RpcMessage()
	:request_id(0),
	 rpc_meta(NULL),
	 method_descriptor(NULL),
	 request(NULL),
	 response(NULL)
{ ; }

RpcMessage::~RpcMessage(){ ; }

bool RpcMessage::IsCompleted(){
	return rpc_meta != NULL && method_descriptor != NULL && request != NULL && response != NULL;
}

void RpcMessage::Clear(){
	request_id = 0;
	DELETE_POINT_IF_NOT_NULL(rpc_meta);
	method_descriptor = NULL;
	DELETE_POINT_IF_NOT_NULL(request);
	DELETE_POINT_IF_NOT_NULL(response);
}

} // namespace frrpc


