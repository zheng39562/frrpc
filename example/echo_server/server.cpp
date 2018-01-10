/**********************************************************
 *  \file server.cpp
 *  \brief
 *  \note	
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "rpc/frrpc_log.h"
#include "rpc/server.h"
#include "echo.pb.h"
#include "rpc/frrpc_function.h"

using namespace std;

// Golbal Variables
string g_ip_("0.0.0.0");
Port g_port_(12345);

static google::protobuf::DynamicMessageFactory frrpc_func_s_message_factory_;
google::protobuf::Message* CreateProtoMessage(const google::protobuf::Descriptor* type){
	return frrpc_func_s_message_factory_.GetPrototype(type)->New();
}

class EchoServiceImpl : public example::EchoService{
	public:
		virtual void Echo(::google::protobuf::RpcController* controller, const ::example::request* request, ::example::response* response, ::google::protobuf::Closure* done){
			DEBUG_D("Receive msg [" << request->msg() << "]");
			response->set_msg("hello frrpc client.");
			done->Run();
		}
};

int main(int argc, char* argv[]){
	frrpc::OpenLog("./log", fr_public::eLogLevel_Program);
	fr_public::SingleLogServer::GetInstance()->set_default_log_key("server");

	frrpc::Server server;

	EchoServiceImpl* service = new EchoServiceImpl();

	server.AddService(service);

	if(!server.StartServer(g_ip_, g_port_)) {
		DEBUG_E("Fail to start server.");
		return -1;
	}

	DEBUG_D("Wait quit.");
	server.RunUntilQuit();
	DEBUG_D("Already quit.");

	DELETE_POINT_IF_NOT_NULL(service);

	return 0;
}

