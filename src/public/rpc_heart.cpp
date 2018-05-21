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

namespace frrpc{// {{{1

BinaryMemoryPtr BuildHeartPacket(){//{{{2
	NetInfo net_info;
	net_info.set_net_type(eNetType_Heart);
	return BuildBinaryFromMessage(net_info);
}//}}}2

frpublic::BinaryMemoryPtr RpcHeart::static_heart_packet_ = BuildHeartPacket();

RpcHeart::RpcHeart()// {{{2
	:running_(false),
	 is_client_(true),
	 thread_heart_time_(),
	 mutex_socket_(),
	 wait_heart_array_(),
	 socket_array_()
{ 
}//}}}2

RpcHeart::~RpcHeart(){ // {{{2
	if(thread_heart_time_.joinable()){
		thread_heart_time_.join();
	}

}//}}}2

void RpcHeart::RunClient(frnet::NetClient* client, time_t timeout){//{{{2
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
						DEBUG_E("Channel fail to send heart. ret [" << ret << "]");
					}
				}

				FrSleep(sleep_time_ms);
			}
		}
	}, client, timeout);
}//}}}2

void RpcHeart::RunServer(frnet::NetServer* server, time_t timeout){//{{{2
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
						DEBUG_I("socket[" << socket << "] heart timeout.");
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
}//}}}2

void RpcHeart::StopHeartCheck(){// {{{2
	running_ = false;
	if(thread_heart_time_.joinable()){
		thread_heart_time_.join();
	}

	lock_guard<mutex> lock(mutex_socket_);
	socket_array_.clear();
	wait_heart_array_.clear();
}// }}}2

void RpcHeart::DelSocket(Socket socket){//{{{2
	if(running_ && !is_client_){
		lock_guard<mutex> lock(mutex_socket_);
		wait_heart_array_.erase(socket);
		socket_array_.erase(socket);
	}
}//}}}2

void RpcHeart::AddSocket(Socket socket){//{{{2
	if(running_ && !is_client_){
		lock_guard<mutex> lock(mutex_socket_);
		socket_array_.insert(socket);
	}
}//}}}2

void RpcHeart::UpdateSocket(Socket socket){//{{{2
	if(running_ && !is_client_ && wait_heart_array_.find(socket) != wait_heart_array_.end()){
		lock_guard<mutex> lock(mutex_socket_);
		DEBUG_P("receive msg update socket heart [" << socket << "]");
		wait_heart_array_.erase(socket);
	}
}//}}}2


}//}}}1
