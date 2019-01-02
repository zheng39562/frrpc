/**********************************************************
 *  \file frrpc_log.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "frrpc_log.h"

using namespace frpublic;

namespace frrpc{ 

static bool frrpc_func_s_is_open_log_ = false;
bool IsOpenLog(){ return frrpc_func_s_is_open_log_; }

void OpenLog(const std::string& log_path, eLogLevel log_level, uint32_t file_size){
	frrpc_func_s_is_open_log_ = true;

	SingleLogServer::GetInstance()->InitLog(log_path, file_size);
	SingleLogServer::GetInstance()->set_log_level(LOG_KEY, log_level);

	OpenMsgLog();
}

void CloseLog(){
	frrpc_func_s_is_open_log_ = false;
}

static bool frrpc_func_s_is_open_msg_log_ = false;
bool IsOpenMsgLog(){ return frrpc_func_s_is_open_msg_log_; }

void OpenMsgLog(){
	frrpc_func_s_is_open_msg_log_ = true;

	SingleLogServer::GetInstance()->set_log_level(MSG_LOG_KEY, frpublic::eLogLevel_Info);
}

void CloseMsgLog(){
	frrpc_func_s_is_open_msg_log_ = false;
}

} // namespace frrpc{ 

