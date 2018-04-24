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

class EchoServiceImpl : public example::EchoService{
	public:
		EchoServiceImpl(frrpc::Server* server):server_(server){ ; }
	public:
		virtual void Echo(::google::protobuf::RpcController* controller, const ::example::request* request, ::example::response* response, ::google::protobuf::Closure* done){
			frrpc::Controller* echo_cntl = dynamic_cast<frrpc::Controller*>(controller);
			if(echo_cntl == nullptr){
				DEBUG_E("echo controller is null(Fail to Convert RpcController to frrpc::Controller).");
			}

			DEBUG_D("Receive msg [" << request->msg() << "]");

			response->set_msg("hello client.");

			frrpc::Controller cntl;
			cntl.set_link(echo_cntl->link_id());
			bool ret(true);
			ret &= server_->SendRpcMessage(&cntl, "EchoService", "RegisterEcho", *response);
			ret &= server_->SendRpcMessage(&cntl, "EchoService", "RegisterClassEcho", *response);
			if(!ret){
				DEBUG_E("SendRpcMessage has error.");
			}

			done->Run();
		}
	private:
		frrpc::Server* server_;
};

int main(int argc, char* argv[]){
	frrpc::OpenLog("./log", frpublic::eLogLevel_Program);
	frpublic::SingleLogServer::GetInstance()->set_default_log_key("server");

	if(argc != 4){
		DEBUG_E("parameter size is wrong.");
		return -1;
	}

	string connect_option = string(argv[1]);
	g_ip_ = string(argv[2]);
	g_port_ = stoi(string(argv[3]));

	DEBUG_D("Network option : [" << connect_option << "][" << g_ip_ << "][" << g_port_ << "]");

	frrpc::ServerOption option;
	frrpc::Server server(option);
	if("Gate" == connect_option || connect_option == "gate"){
		if(!server.StartGate({ tuple<string, Port>(g_ip_, g_port_) })){
			DEBUG_E("Fail to start server.");
			return -1;
		}
	}
	else{
		if(!server.StartServer(g_ip_, g_port_)){
			DEBUG_E("Fail to start server.");
			return -1;
		}
	}

	EchoServiceImpl* service = new EchoServiceImpl(&server);
	server.AddService(service);

	DEBUG_D("Wait quit.");
	server.RunUntilQuit();
	DEBUG_D("Already quit.");

	DELETE_POINT_IF_NOT_NULL(service);

	return 0;
}

