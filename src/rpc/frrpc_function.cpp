/**********************************************************
 *  \file frrpc_function.cpp
 *  \brief
 *  \note	
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "frrpc_function.h"

#include <sys/types.h> 
#include <signal.h> 

using namespace std;
using namespace fr_public;
using namespace google::protobuf;

namespace frrpc{

// inside function.
void SignalHandle(int sign);
void CheckRegister(sighandler_t* ret);
void RegisterQuitSignal();

static bool frrpc_func_s_quit = false;

void AskToQuit(){
	RPC_DEBUG_D("Ask to quit.");
	frrpc_func_s_quit = true;
}

static pthread_once_t frrpc_func_s_signal_handle_once = PTHREAD_ONCE_INIT;
bool IsAskedToQuit(){
	pthread_once(&frrpc_func_s_signal_handle_once, RegisterQuitSignal);
	return frrpc_func_s_quit;
}

void SignalHandle(int sign){
	switch(sign){
		case SIGINT: {
			AskToQuit();
			break;
		}
	}
}

void CheckRegister(sighandler_t* ret){
	if(*ret != SIG_DFL || *ret != SIG_IGN){
		if(*ret == SIG_ERR){
			abort();
		}
	}
	else{
		abort();
	}
}

void RegisterQuitSignal(){
	sighandler_t ret_ini = signal(SIGINT, SignalHandle);
	CheckRegister(&ret_ini);
}

static google::protobuf::DynamicMessageFactory frrpc_func_s_message_factory_;
google::protobuf::Message* CreateProtoMessage(const google::protobuf::Descriptor* type){
	return frrpc_func_s_message_factory_.GetPrototype(type)->New();
}

}// namespace frrpc{

