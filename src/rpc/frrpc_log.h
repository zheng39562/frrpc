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

#include "fr_public/pub_log.h"

namespace frrpc{
	bool IsOpenLog();
	void OpenLog(const std::string& log_path, fr_public::eLogLevel log_level, uint32_t file_size = 10 * 1024 * 1024);
	void CloseLog();
} // namespace frrpc

#define LOG_KEY	"frrpc"
#define RPC_DEBUG_P(msg) if(frrpc::IsOpenLog()){ K_DEBUG_P(LOG_KEY, msg) }
#define RPC_DEBUG_D(msg) if(frrpc::IsOpenLog()){ K_DEBUG_D(LOG_KEY, msg) }
#define RPC_DEBUG_I(msg) if(frrpc::IsOpenLog()){ K_DEBUG_I(LOG_KEY, msg) }
#define RPC_DEBUG_W(msg) if(frrpc::IsOpenLog()){ K_DEBUG_W(LOG_KEY, msg) }
#define RPC_DEBUG_E(msg) if(frrpc::IsOpenLog()){ K_DEBUG_E(LOG_KEY, msg) }
#define RPC_DEBUG_C(msg) if(frrpc::IsOpenLog()){ K_DEBUG_C(LOG_KEY, msg) }

#endif 

