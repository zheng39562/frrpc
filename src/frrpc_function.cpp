/**********************************************************
 *  \file frrpc_function.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "frrpc_function.h"

namespace frrpc{

bool RunUntilStop(){
}

bool IsQuit(){
}

void OpenRpcLog(){
}

void CloseRpcLog(){
}

static DynamicMessageFactory frrpc_func_s_message_factory_;
google::protobuf::Message* CreateProtoMessage(const Descriptor* type){
	return frrpc_func_s_message_factory_.GetPrototype(method_descriptor->input_type());
}

bool ParseBinary(const fr_public::BinaryMemory& binary, RpcMessage& rpc_message){
	;
}

}// namespace frrpc{

