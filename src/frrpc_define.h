/**********************************************************
 *  \file frrpc_define.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _frrpc_define_H
#define _frrpc_define_H

#include "fr_public/pub_log.h"

typedef uint32_t Port;
typedef uint32_t Socket;
typedef unsigned short Port;
typedef unsigned char Byte;
typedef uint16_t RpcMetaSize;
typedef uint32_t RpcRequestSize;

#define BYTE_CONVERT_TO_TYPE_OFFSET(byte_ptr, type, offset) *(const type*)((const Byte*)byte_ptr + offset)

#endif 

