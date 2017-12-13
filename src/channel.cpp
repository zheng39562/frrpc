/**********************************************************
 *  \file channel.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "channel.h"

namespace frrpc{

Channel::Channel(){
	;
}

Channel::~Channel(){
	;
}

void Channel::CallMethod(const MethodDescriptor* method, RpcController* controller, const Message* request, Message* response, Closure* done){
	;
}


} // namespace frrpc{

