/**********************************************************
 *  \file rpc_heart.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _rpc_heart_H
#define _rpc_heart_H

#include <set>
#include <thread>
#include <mutex>
#include <string>
#include "public_define.h"
#include "frpublic/pub_memory.h"
#include "frnet/frnet_interface.h"

namespace frrpc{// {{{1

frpublic::BinaryMemoryPtr BuildHeartPacket();

// class RpcHeart {{{2
// TODO:
//	Client disconnect: client only send heart now.It does not check heart response.
//		* bool SendResponse(Socket socket);
class RpcHeart{
	public:
		RpcHeart();
		~RpcHeart();
	public:
		// heart check is a choice.If you need, call RunHeartCheck()
		// Attention : If you use it. you need add/delete/update by youself(In drived class)
		// these function has mutex. So you can call them in multiple thread. 
		//
		// Recall Run or Stop is not useless.
		//
		// unit of timeout is second. Use Sleep function, so it is not very accurate.
		void RunClient(frnet::NetClient* client, time_t timeout = 60);
		void RunServer(frnet::NetServer* server, time_t timeout = 60);
		void StopHeartCheck();

		void DelSocket(Socket socket);
		void AddSocket(Socket socket);
		void UpdateSocket(Socket socket);
	private:
		static frpublic::BinaryMemoryPtr static_heart_packet_;

		bool running_;
		bool is_client_;
		std::thread thread_heart_time_;
		std::mutex mutex_socket_;
		std::set<Socket> wait_heart_array_;
		std::set<Socket> socket_array_;
};
//}}}2
//

} //}}}1

#endif 

