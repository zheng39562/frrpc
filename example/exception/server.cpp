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
#include "frpublic/pub_tool.h"

using namespace std;

class EchoServiceImpl : public example::EchoService{
	public:
		EchoServiceImpl(frrpc::Server* server):server_(server),recv_echo_count_(0){ ; }
	public:
		virtual void Echo(::google::protobuf::RpcController* controller, const ::example::request* request, ::example::response* response, ::google::protobuf::Closure* done){
			frrpc::ServerController* echo_cntl = dynamic_cast<frrpc::ServerController*>(controller);
			if(echo_cntl == nullptr){
				DEBUG_E("echo controller is null(Fail to Convert RpcController to frrpc::Controller).");
			}

			DEBUG_D("Receive msg [%s]", request->msg().c_str());

			frpublic::FrSleep(2000);

			response->set_msg("hello client.");
			done->Run();

			if(request->msg() == "disconnect"){
				DEBUG_D("disconnect [%d]", echo_cntl->link_id());
				server_->Disconnect(echo_cntl->link_id());
			}
		}
	private:
		frrpc::Server* server_;
		int recv_echo_count_;
};

int main(int argc, char* argv[]){
	frrpc::OpenLog("./log", frpublic::eLogLevel_Program);
	frpublic::SingleLogServer::GetInstance()->set_default_log_key("server");

	if(argc < 3 || argc > 5){
		DEBUG_E("parameter size is wrong.");
		return -1;
	}

	string ip = string(argv[1]);
	Port port = stoi(string(argv[2]));
	string service_addr = "";
	if(argc == 4){
		service_addr = string(argv[3]);
	}

	DEBUG_D("Network option : [%s:%d]", ip.c_str(), port);

	frrpc::ServerOption option;
	option.work_thread_num = 2;
	option.service_addr = service_addr;
	frrpc::Server server(option);
	if(!server.StartRoute({ tuple<string, Port>(ip, port) })){
		DEBUG_E("Fail to start server.");
		return -1;
	}

	EchoServiceImpl* service = new EchoServiceImpl(&server);
	server.AddService(service);

	DEBUG_D("Wait quit.");
	server.RunUntilQuit();
	DEBUG_D("Already quit.");

	DELETE_POINT_IF_NOT_NULL(service);

	return 0;
}

