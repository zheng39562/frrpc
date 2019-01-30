/**********************************************************
 *  \file client.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "rpc/frrpc_log.h"
#include "rpc/channel.h"
#include "echo.pb.h"
#include "rpc/frrpc_function.h"
#include "frpublic/pub_tool.h"

using namespace std;

bool g_recv_rsp_register(false);

class EchoClass{
	public:
		void OnRegisterEcho(frrpc::ChannelController* ctnl, example::response* response){
			DEBUG_D("Receive response [%s]", response->msg().c_str());
			g_recv_rsp_register = true;
		}
};

void WaitAllRsp(){
	while(!g_recv_rsp_register && !frrpc::IsAskedToQuit()){
		frpublic::FrSleep(10);
	}
}

int main(int argc, char* argv[]){
	frrpc::OpenLog("./log", frpublic::eLogLevel_Program);
	frpublic::SingleLogServer::GetInstance()->set_default_log_key("client");
	if(argc < 3 || argc > 5){
		DEBUG_D("argc is wrong. [server_option] [ip] [port]");
		return -1;
	}

	string ip = string(argv[1]);
	Port port = stoi(string(argv[2]));
	string service_addr = "";
	if(argc == 4){
		service_addr = string(argv[3]);
	}

	DEBUG_D("Client option : [%s:%d]", ip.c_str(), port);

	frrpc::ChannelOption channel_option("echo_sync");
	frrpc::Channel channel(channel_option);
	if(!channel.StartRoute(ip, port)) {
		DEBUG_E("Fail to start route.");
		return -1;
	}

	example::EchoService_Stub stub(&channel);
	channel.RegisterService(&stub, service_addr);

	//
	// register class method.
	// 
	EchoClass echo_object;
	channel.RegisterCallback(&stub, "RegisterEcho", &echo_object, &EchoClass::OnRegisterEcho);

	//
	// normal request method
	// 
	// 同步模式下，框架不会删除cntl和rsp.
	example::request req;
	req.set_msg("Hello server.");
	frrpc::ChannelController cntl(&channel, true);
	example::response response;
	stub.Echo(&cntl, &req, &response, NULL);
	stub.Echo(&cntl, &req, &response, NULL);
	if(cntl.Failed()){
		DEBUG_D("receive error(expected).");
	}

	WaitAllRsp();

	channel.Stop();
	return 0;
}

