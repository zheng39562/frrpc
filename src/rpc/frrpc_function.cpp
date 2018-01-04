/**********************************************************
 *  \file frrpc_function.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "frrpc_function.h"

#include <google/protobuf/dynamic_message.h>

using namespace std;
using namespace fr_public;
using namespace google::protobuf;

namespace frrpc{

bool RunUntilStop(){
	return true;
}

bool IsQuit(){
	return true;
}

void OpenRpcLog(){
}

void CloseRpcLog(){
}

static google::protobuf::DynamicMessageFactory frrpc_func_s_message_factory_;
google::protobuf::Message* CreateProtoMessage(const google::protobuf::Descriptor* type){
	return frrpc_func_s_message_factory_.GetPrototype(type)->New();
}


}// namespace frrpc{

