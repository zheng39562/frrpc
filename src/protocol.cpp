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
	:socket_id(0),
	 method_descriptor(NULL),
	 request(NULL),
	 response(NULL)
{ ; }
RpcMessage::~RpcMessage(){ ; }

bool RpcMessage::IsCompleted(){
	return socket_id != 0 && method_descriptor != NULL && request != NULL && response != NULL;
}

void RpcMessage::Clear(){
	socket_id = 0;
	method_descriptor = NULL;
	request = NULL;
	response = NULL;
}

} // namespace frrpc


