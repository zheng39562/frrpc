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

bool g_disconnect = false;

void NetCallback(const frrpc::eRpcEvent& event){
	DEBUG_D("net event : [%s]", frrpc::eRpcEvent_Name(event).c_str());

	if(frrpc::eRpcEvent_Disconnection == event){
		g_disconnect = true;
	}
}

void CancelCheck(){
	DEBUG_D("Request cancel.");
}

void TimeoutCheck(frrpc::ChannelController* cntl){
	if(cntl->Failed()){
		DEBUG_D("receive error(expected) : %s.", cntl->ErrorText().c_str());
	}
	else{
		DEBUG_E("timeout logic is wrong.");
	}
}

void OnEcho(frrpc::ChannelController* cntl, example::response* response){
	TimeoutCheck(cntl);
}

void WaitAllRsp(){
	while(!g_disconnect && !frrpc::IsAskedToQuit()){
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

	frrpc::ChannelOption channel_option("demo_exception");
	channel_option.request_timeout = 1000;
	frrpc::Channel channel(channel_option);
	if(!channel.StartRoute(ip, port)) {
		DEBUG_E("Fail to start route.");
		return -1;
	}
	channel.RegisterNetEvent(&NetCallback);

	example::EchoService_Stub stub(&channel);
	channel.RegisterService(&stub, service_addr);

	// 
	// timeout
	// 
	example::request req;
	req.set_msg("timeout.");
	frrpc::ChannelController cntl(&channel, true);
	example::response response;
	stub.Echo(&cntl, &req, &response, NULL);
	TimeoutCheck(&cntl);

	channel.AsyncRequest(&stub, "Echo", &req, &OnEcho);

	//
	// cancel
	//
	req.set_msg("cancel");
	// 此controller不需要删除。内部会管理.
	google::protobuf::RpcController* cntl_async = channel.AsyncRequest(&stub, "Echo", &req, &OnEcho);
	cntl_async->NotifyOnCancel(frrpc::NewCallback(&CancelCheck));
	cntl_async->StartCancel();
	
	//
	// disconnect
	//
	req.set_msg("disconnect");
	channel.AsyncRequest(&stub, "Echo", &req, &OnEcho);

	WaitAllRsp();

	channel.Stop();

	return 0;
}

