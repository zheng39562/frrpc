/**********************************************************
 *  \file rpc_heart.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "rpc_heart.h"

#include "pb/net.pb.h"
#include "frpublic/pub_tool.h"
#include "public/rpc_serializable.h"

using namespace std;
using namespace frnet;
using namespace frpublic;
using namespace frrpc::network;

namespace frrpc{

BinaryMemoryPtr BuildHeartPacket(){
	NetInfo net_info;
	net_info.set_net_type(eNetType_Heart);
	return BuildBinaryFromMessage(net_info);
}

frpublic::BinaryMemoryPtr RpcHeart::static_heart_packet_ = BuildHeartPacket();

RpcHeart::RpcHeart()
	:running_(false),
	 is_client_(true),
	 thread_heart_time_(),
	 mutex_socket_(),
	 wait_heart_array_(),
	 socket_array_()
{ 
}

RpcHeart::~RpcHeart(){ 
	if(thread_heart_time_.joinable()){
		thread_heart_time_.join();
	}

}

void RpcHeart::RunClient(frnet::NetClient* client, time_t timeout){
	is_client_ = true;
	running_ = true;
	thread_heart_time_ = thread([&](frnet::NetClient* client, time_t timeout){
		time_t sleep_time_ms(250);
		time_t cur_times(0);
		if(client != NULL){
			time_t sleep_time = timeout * 1000 / sleep_time_ms / 4; // 1000 / 4
			while(running_){
				if(++cur_times > sleep_time){
					cur_times = 0;

					eNetSendResult ret = client->Send(static_heart_packet_);
					if(ret != eNetSendResult_Ok){
						DEBUG_E("Channel fail to send heart. ret [%d]", ret);
					}
				}

				FrSleep(sleep_time_ms);
			}
		}
	}, client, timeout);
}

void RpcHeart::RunServer(frnet::NetServer* server, time_t timeout){
	is_client_ = false;
	running_ = true;
	thread_heart_time_ = thread([&](frnet::NetServer* server, time_t timeout){
		time_t cur_times(0);
		if(server != NULL){
			while(running_){
				if(++cur_times > timeout){
					cur_times = 0;

					std::lock_guard<mutex> lock(mutex_socket_);

					for(auto& socket : wait_heart_array_){
						DEBUG_I("socket[%d] heart timeout.", socket);
						server->Disconnect(socket);
						socket_array_.erase(socket);
					}
					wait_heart_array_ = socket_array_;

					DEBUG_P("Reset heart check.");
				}

				FrSleep(1000);
			}
		}
		else{ DEBUG_E("server is null."); }
	}, server, timeout);
}

void RpcHeart::StopHeartCheck(){
	running_ = false;
	if(thread_heart_time_.joinable()){
		thread_heart_time_.join();
	}

	lock_guard<mutex> lock(mutex_socket_);
	socket_array_.clear();
	wait_heart_array_.clear();
}

void RpcHeart::DelSocket(Socket socket){
	if(running_ && !is_client_){
		lock_guard<mutex> lock(mutex_socket_);
		wait_heart_array_.erase(socket);
		socket_array_.erase(socket);
	}
}

void RpcHeart::AddSocket(Socket socket){
	if(running_ && !is_client_){
		lock_guard<mutex> lock(mutex_socket_);
		socket_array_.insert(socket);
	}
}

void RpcHeart::UpdateSocket(Socket socket){
	if(running_ && !is_client_ && wait_heart_array_.find(socket) != wait_heart_array_.end()){
		lock_guard<mutex> lock(mutex_socket_);
		DEBUG_P("receive msg update socket heart [%d]", socket);
		wait_heart_array_.erase(socket);
	}
}


}
