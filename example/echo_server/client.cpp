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
#include "fr_public/pub_tool.h"

using namespace std;

string g_ip_("0.0.0.0");
Port g_port_(12345);

void EchoCallback(example::response* response){
	DEBUG_D("Receive response [" << response->msg() << "]");;

	DELETE_POINT_IF_NOT_NULL(response);
}

int main(int argc, char* argv[]){
	frrpc::OpenLog("./log", fr_public::eLogLevel_Program);
	fr_public::SingleLogServer::GetInstance()->set_default_log_key("client");

	frrpc::ChannelOption channel_option;
	frrpc::Channel channel(channel_option);
	if(!channel.StartServer(g_ip_, g_port_)) {
		DEBUG_E("Fail to start server.");
		return -1;
	}

	frrpc::Controller ctrl;
	example::request req;
	req.set_msg("hello frrpc server.");

	example::EchoService_Stub stub(&channel);
	while(!frrpc::IsAskedToQuit()){
		example::response* res = new example::response();
		google::protobuf::Closure* callback = NewCallback(EchoCallback, res);

		stub.Echo(&ctrl, &req, res, callback);
		DEBUG_D("Call Echo. Send msg [" << req.msg() << "]");

		fr_public::FrSleep(1000);

		channel.RunCallback();
	}

	return 0;
}

