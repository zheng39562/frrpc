/**********************************************************
 *  \file frrpc_log.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "frrpc_log.h"

using namespace fr_public;

namespace frrpc{ 

static bool frrpc_func_s_is_open_log = false;
bool IsOpenLog(){ return frrpc_func_s_is_open_log; }

void OpenLog(const std::string& log_path, eLogLevel log_level, uint32_t file_size){
	frrpc_func_s_is_open_log = true;

	SingleLogServer::GetInstance()->InitLog(log_path, file_size);
	SingleLogServer::GetInstance()->set_log_level(LOG_KEY, log_level);
}

void CloseLog(){
	frrpc_func_s_is_open_log = false;
}

} // namespace frrpc{ 

