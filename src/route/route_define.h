/**********************************************************
 *  \file route_define.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _route_define_H
#define _route_define_H

#include "public_define.h" 

#define SOCKET_NULL 0
#define ROUTE_INDEX_NULL -1

#define DEBUG_PB_SERIALIZE_FAILURE(pb_struct_name) DEBUG_E("Fail to serialize " << pb_struct_name)
#define DEBUG_PB_PARSE_FAILURE(pb_struct_name) DEBUG_E("Fail to parse " << pb_struct_name)

#endif 

