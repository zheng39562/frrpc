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

#include "fr_public/pub_memory.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace frrpc{

bool RunUntilStop();
bool IsQuit();

void OpenRpcLog();
void CloseRpcLog();

google::protobuf::Message* CreateProtoMessage(const google::protobuf::Descriptor* type);

// T must a number(int long shor etc).
template <typename T>
inline uint32_t GetNumberLength(T number){
	uint32_t length(0);
	do{
		++length;
		number /= 10;
	}while((number - 0) < 0.0000001);

	return length;
}

}// namespace frrpc{


#endif 

