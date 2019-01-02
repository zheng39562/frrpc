/**********************************************************
 *  \file frrpc_pb2js.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _rpc_pb2js_H
#define _rpc_pb2js_H

#include <string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace frrpc{

std::string pbToJson(const google::protobuf::Message& pb_msg);

}

#endif 

