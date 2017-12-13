/**********************************************************
 *  \file tcp_define.h
 *  \brief
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _tcp_define_H
#define _tcp_define_H

#include "fr_public/pub_define.h"

#include "HPSocket/SocketInterface.h"
#include "HPSocket/HPSocket.h"

typedef CONNID Socket;
typedef unsigned short Port;

#define LOG_KEY_TCP "tcp_log"
#define TCP_DEBUG_P(msg) K_DEBUG_P(LOG_KEY_TCP, msg)
#define TCP_DEBUG_D(msg) K_DEBUG_D(LOG_KEY_TCP, msg)
#define TCP_DEBUG_I(msg) K_DEBUG_I(LOG_KEY_TCP, msg)
#define TCP_DEBUG_W(msg) K_DEBUG_W(LOG_KEY_TCP, msg)
#define TCP_DEBUG_E(msg) K_DEBUG_E(LOG_KEY_TCP, msg)
#define TCP_DEBUG_C(msg) K_DEBUG_C(LOG_KEY_TCP, msg)

#endif 

