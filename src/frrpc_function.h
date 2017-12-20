/**********************************************************
 *  \file frrpc_function.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _frrpc_function_H
#define _frrpc_function_H

#include "fr_pub/pub_memory.h"
#include "protocol.h"

namespace frrpc{

void RunUntilStop();
bool IsQuit();

void OpenRpcLog();
void CloseRpcLog();

google::protobuf::Message* CreateProtoMessage(const Descriptor* type);

}// namespace frrpc{


#endif 

