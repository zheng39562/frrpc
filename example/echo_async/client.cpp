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

/*
	* 完善所有的测试用例, 简易的测试用例都当做demo使用.(每个用力都需要server 版本和 route版本)
	* 需要测试
		* rpc请求服务
		* 连接，断开，以及相关的通知功能.
			* 两种模式的通知机制应该存在不同的情况.
		* 考虑大量传输的可能性.
		* 心跳部分:
			* 心跳尝试独立成一个类：发送函数可以作为一个回调传入.
			* 分为主发和检测：超时断线也作为函数传入.
		* channel 
			* 断线等network响应对应都未完善.
*/

string g_ip_("0.0.0.0");
Port g_port_(12345);
bool g_wait_rsp_echo(false);
bool g_wait_rsp_register_echo(false);
bool g_wait_rsp_register_class_echo(false);

void OnEcho(example::response* rsp, frrpc::Controller* ctnl){
	DEBUG_D("Receive response [" << rsp->msg() << "]");;

	// this point of object is you created.So you must delete it.
	DELETE_POINT_IF_NOT_NULL(rsp);

	g_wait_rsp_echo = false;
}

void OnRegisterEcho(google::protobuf::Message* response, frrpc::Controller* ctnl){
	example::response* rsp = dynamic_cast<example::response*>(response);
	DEBUG_D("Receive response [" << rsp->msg() << "]");;

	g_wait_rsp_register_echo = false;
}

class EchoClass{
	public:
		void OnRegisterClassEcho(google::protobuf::Message* response, frrpc::Controller* ctnl){
			example::response* rsp = dynamic_cast<example::response*>(response);
			DEBUG_D("Receive response [" << rsp->msg() << "]");
			g_wait_rsp_register_class_echo = false;
		}
};

void WaitAllRsp(frrpc::Channel& channel){
	g_wait_rsp_echo = true;
	g_wait_rsp_register_echo = true;
	g_wait_rsp_register_class_echo = true;
	while((g_wait_rsp_echo || g_wait_rsp_register_echo || g_wait_rsp_register_class_echo) && !frrpc::IsAskedToQuit()){
		frpublic::FrSleep(10);
		channel.RunCallback();
	}
}

// todo 
//	* register net event.
int main(int argc, char* argv[]){
	frrpc::OpenLog("./log", frpublic::eLogLevel_Program);
	frpublic::SingleLogServer::GetInstance()->set_default_log_key("client");
	if(argc != 4){
		DEBUG_D("argc is wrong. [server_option] [ip] [port]");
		return -1;
	}
	string connect_option = string(argv[1]);
	g_ip_ = string(argv[2]);
	g_port_ = stoi(string(argv[3]));

	DEBUG_D("Client option : [" << connect_option << "][" << g_ip_ << "][" << g_port_ << "]");

	frrpc::ChannelOption channel_option;
	frrpc::Channel channel(channel_option);
	if("Route" == connect_option || connect_option == "Route"){
		// connect route.
		// you must launch route.
		if(!channel.StartRoute(g_ip_, g_port_)) {
			DEBUG_E("Fail to start route.");
			return -1;
		}
	}
	else{
		// connect server.
		if(!channel.StartServer(g_ip_, g_port_)) {
			DEBUG_E("Fail to start server.");
			return -1;
		}
	}

	frrpc::Controller* cntl = new frrpc::Controller();
	example::EchoService_Stub stub(&channel);

	// register request method.
	DEBUG_D("Register Echo.");
	frrpc::RegisterRpcMethod(channel, &stub, "RegisterEcho", &OnRegisterEcho, cntl);

	// register class method.
	EchoClass echo_object;
	DEBUG_D("Register Register Class Echo.");
	frrpc::RegisterRpcMethod(channel, &stub, "RegisterClassEcho", &echo_object, &EchoClass::OnRegisterClassEcho, cntl);

	// normal request method
	DEBUG_D("Normal Echo.");
	example::request req;
	req.set_msg("hello server.");
	example::response* rsp = new example::response();
	stub.Echo(cntl, &req, rsp, frrpc::NewCallback(&OnEcho, rsp, cntl));

	WaitAllRsp(channel);

	channel.Stop();
	return 0;
}

