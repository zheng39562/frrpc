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

bool g_recv_rsp(false);
bool g_recv_rsp_error(false);
bool g_recv_rsp_register(false);
bool g_recv_rsp_multiple_param(false);


void OnEcho(frrpc::ChannelController* ctnl, example::response* response){
	if(ctnl->Failed()){
		g_recv_rsp_error = true;
		if(g_recv_rsp){
			DEBUG_D("Receive error(expected) [%s]", ctnl->ErrorText().c_str());
		}
		else{
			DEBUG_E("error return [%s]", ctnl->ErrorText().c_str());
		}
	}
	else{
		DEBUG_D("Receive response [%s]", response->msg().c_str());
		g_recv_rsp = true;
	}
}

class EchoClass{
	public:
		void OnRegisterEcho(frrpc::ChannelController* ctnl, example::response* response){
			DEBUG_D("Receive response [%s]", response->msg().c_str());
			g_recv_rsp_register = true;
		}

		void OnEchoClassMultipleParameter(frrpc::ChannelController* ctnl, example::response* response, int* extra_param){
			DEBUG_D("Receive response [%s] extra param %d", response->msg().c_str(), *extra_param);
			g_recv_rsp_multiple_param = true;
		}
};

void WaitAllRsp(){
	while((!g_recv_rsp || !g_recv_rsp_error || !g_recv_rsp_register || !g_recv_rsp_multiple_param) && !frrpc::IsAskedToQuit()){
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

	frrpc::ChannelOption channel_option("echo_async");
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
	example::request req;
	req.set_msg("Hello server.");
	// 第一次请求: 正常返回，且推送RegisterEcho消息.
	google::protobuf::RpcController* req_cntl = channel.AsyncRequest(&stub, "Echo", &req, &OnEcho);
	// 第二次请求: 测试错误返回. 并使用pb本身推荐的方式(个人不推荐).
	// 创建的controller和response不需要自行删除,rpc会自动删除.
	frrpc::ChannelController* cntl = new frrpc::ChannelController(&channel, false);
	example::response* response = new example::response();
	stub.Echo(cntl, &req, response, frrpc::NewCallback(&OnEcho, cntl, response));

	//
	// multiple parameter demo. 
	//
	// 建议在多参数时使用google原始调用方式。
	// 此方式下，controller和response rpc会自动删除。但额外参数，不会被删除，需要手动处理.
	// 如回调仅有controller和response建议,则使用AsyncRequest,其帮助创建controller和rsp减少外部处理调用工作.
	frrpc::ChannelController* cntl_mul = new frrpc::ChannelController(&channel, false);
	example::response* response_mul = new example::response();
	int* extra_param = new int(12345);
	stub.EchoMulParam(cntl_mul, &req, response_mul, frrpc::NewCallback(&echo_object, &EchoClass::OnEchoClassMultipleParameter, cntl_mul, response_mul, extra_param));


	WaitAllRsp();

	channel.Stop();
	return 0;
}

