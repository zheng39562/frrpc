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

// typedef {{{2
typedef uint32_t Port;
typedef uint32_t Socket;
typedef unsigned short Port;
typedef unsigned char Byte;
typedef uint16_t RpcMetaSize;
typedef uint32_t RpcRequestSize;
typedef uint64_t RpcRequestId;
// }}}2

// enum {{{2

// }}}2

// marco function. // {{{2
#define BYTE_CONVERT_TO_TYPE_OFFSET(byte_ptr, type, offset) *(const type*)((const Byte*)byte_ptr + offset)
#define DELETE_POINT_IF_NOT_NULL(ptr) if(ptr != NULL){ delete ptr; ptr = NULL; }
#define ADDR_MOVE_BY_BYTE(ptr, offset) (void*)((char*)ptr + offset)
#define CONST_ADDR_MOVE_BY_BYTE(ptr, offset) (const void*)((const char*)ptr + offset)
// }}}2

// marco number {{{2
#define RPC_REQUEST_ID_NULL 0
// }}}2

#endif 

