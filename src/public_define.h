/**********************************************************
 *  \file public_define.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _public_define_H
#define _public_define_H

typedef uint64_t LinkID;
typedef uint64_t RpcRequestId;

typedef uint32_t PacketSize;
typedef uint16_t NetInfoSize;
typedef uint16_t RpcMetaSize;
typedef uint32_t RpcBodySize;

typedef uint32_t RouteID;

#define NET_HEART_TIMEOUT				60
#define NET_PACKET_MAX_SIZE				4 * 1024

#endif 

