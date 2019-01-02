/**********************************************************
 *  \file frrpc_log.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _frrpc_log_H
#define _frrpc_log_H

#include "frpublic/pub_log.h"
#include "public/rpc_pb2js.h"

namespace frrpc{
	bool IsOpenLog();
	void OpenLog(const std::string& log_path, frpublic::eLogLevel log_level, uint32_t file_size = 10 * 1024 * 1024);
	void CloseLog();

	// msg log default: open
	bool IsOpenMsgLog();
	void OpenMsgLog();
	void CloseMsgLog();
} // namespace frrpc

#define LOG_KEY	"frrpc"
#define RPC_DEBUG_P(msg, arg...) if(frrpc::IsOpenLog()){ K_DEBUG_P(LOG_KEY, msg, ##arg) }
#define RPC_DEBUG_D(msg, arg...) if(frrpc::IsOpenLog()){ K_DEBUG_D(LOG_KEY, msg, ##arg) }
#define RPC_DEBUG_I(msg, arg...) if(frrpc::IsOpenLog()){ K_DEBUG_I(LOG_KEY, msg, ##arg) }
#define RPC_DEBUG_W(msg, arg...) if(frrpc::IsOpenLog()){ K_DEBUG_W(LOG_KEY, msg, ##arg) }
#define RPC_DEBUG_E(msg, arg...) if(frrpc::IsOpenLog()){ K_DEBUG_E(LOG_KEY, msg, ##arg) }
#define RPC_DEBUG_C(msg, arg...) if(frrpc::IsOpenLog()){ K_DEBUG_C(LOG_KEY, msg, ##arg) }

#define MSG_LOG_KEY	"rpc_msg"

#define MSG_DEBUG_FORMAT(format, arg...) if(frrpc::IsOpenMsgLog()){ K_DEBUG_I(MSG_LOG_KEY, format, ##arg); }

#define MSG_DEBUG_SERVER_REQ(service_name, method_name, link_id, request_id, msg) MSG_DEBUG_FORMAT\
	("opt|server,service_name|%s,method_name|%s,link_id|%d,request_id|%d,req|%s", \
	 service_name.c_str(), method_name.c_str(), link_id, request_id, (msg != NULL ? frrpc::pbToJson(*msg).c_str() : "null"))
#define MSG_DEBUG_SERVER_RSP(link_id, request_id, msg) MSG_DEBUG_FORMAT\
	("opt|server,link_id|%d,request_id|%d,rsp|%s", link_id, request_id, (msg != NULL ? frrpc::pbToJson(*msg).c_str() : "null"))
#define MSG_DEBUG_SERVER_NOTIFY(service_name, method_name, link_id, msg) MSG_DEBUG_FORMAT\
	("opt|server,service_name|%s,method_name|%s,link_id|%d,notify|%s", \
	 service_name.c_str(), method_name.c_str(), link_id, (msg != NULL ? frrpc::pbToJson(*msg).c_str() : "null"))
#define MSG_DEBUG_SERVER_EVENT(event_type, link_id) MSG_DEBUG_FORMAT\
	("opt|server,event_type|%s,link_id|%d", event_type.c_str(), link_id)

#define MSG_DEBUG_CHANNEL_REQ(channel_name, service_name, method_name, request_id, msg) MSG_DEBUG_FORMAT\
	("opt|%s,service_name|%s,method_name|%s,request_id|%d,req|%s", \
	 channel_name.c_str(), service_name.c_str(), method_name.c_str(), request_id, (msg != NULL ? frrpc::pbToJson(*msg).c_str() : "null"))
#define MSG_DEBUG_CHANNEL_RSP(channel_name, request_id, msg) MSG_DEBUG_FORMAT\
	("opt|%s,request_id|%d,rsp|%s", channel_name.c_str(), request_id, (msg != NULL ? frrpc::pbToJson(*msg).c_str() : "null"))
#define MSG_DEBUG_CHANNEL_NOTIFY(channel_name, service_name, method_name, msg) MSG_DEBUG_FORMAT\
	("opt|%s,service_name|%s,method_name|%s,notify|%s", channel_name.c_str(), service_name.c_str(), method_name.c_str(), (msg != NULL ? frrpc::pbToJson(*msg).c_str() : "null"))
#define MSG_DEBUG_CHANNEL_EVENT(channel_name, event_type) MSG_DEBUG_FORMAT\
	("opt|%s,event_type|%s", channel_name.c_str(), event_type.c_str())

#endif 

