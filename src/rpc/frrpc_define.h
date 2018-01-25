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

#include "frrpc_log.h"

// typedef {{{2
typedef uint64_t LinkID;
typedef uint64_t RpcRequestId;

typedef uint32_t PacketSize;
typedef uint16_t NetInfoSize;
typedef uint16_t RpcMetaSize;
typedef uint32_t RpcBodySize;

typedef uint32_t GateID;

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
#define RPC_LINK_ID_NULL 0

#define NET_HEART_TIMEOUT				60
#define NET_PACKET_MAX_SIZE				4 * 1024
// }}}2

// common class {{{2
// }}}2

#endif 

