/**********************************************************
 *  \file main.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "route.h"

#include "frpublic/pub_ini.h"
#include "frpublic/pub_tool.h"
#include <sys/types.h> 
#include <signal.h> 

using namespace std;
using namespace frpublic;

// Exit function list. {{{1

void AskToQuit();

void SignalHandle(int sign){
	switch(sign){
		case SIGINT: {
			AskToQuit();
			break;
		}
	}
}
void RegisterQuitSignal(){
	sighandler_t ret = signal(SIGINT, SignalHandle);
	if(ret != SIG_DFL || ret != SIG_IGN){
		if(ret == SIG_ERR){
			abort();
		}
	}
	else{
		abort();
	}
}
static bool g_is_quit = false;
static pthread_once_t frrpc_func_s_signal_handle_once = PTHREAD_ONCE_INIT;
void AskToQuit(){
	g_is_quit = true;
}
bool IsAskedToQuit(){
	pthread_once(&frrpc_func_s_signal_handle_once, RegisterQuitSignal);
	return g_is_quit;
}
//}}}1

// param[1] route ini file.
int main(int argc, char* argv[]){
	SingleLogServer::GetInstance()->InitLog("./log", 10 * 1024 * 1024);
	SingleLogServer::GetInstance()->set_default_log_key("route");
	SingleLogServer::GetInstance()->set_log_level("route", frpublic::eLogLevel_Program);

	if(!SingleIniCfg::GetInstance()->InitFile((argc > 1 ? string(argv[1]) : "./route.ini"))){
		DEBUG_E("Fail to read ini. file path [" << (argc > 1 ? string(argv[1]) : "./route.ini") << "]");
		return -1;
	}

	string ip = SingleIniCfg::GetInstance()->GetString("network", "ip");
	Port port = SingleIniCfg::GetInstance()->GetInt("network", "port");
	SingleLogServer::GetInstance()->set_log_level("route", PARSE_LOG_STRING(SingleIniCfg::GetInstance()->GetString("log", "level")));

	frrpc::route::RpcRoute rpc_route;
	if(!rpc_route.Start(ip, port)){
		DEBUG_E("Fail to start route.");
		return -1;
	}

	while(!IsAskedToQuit()){
		frpublic::FrSleep(10);
	}

	rpc_route.Stop();
	return 0;
}

