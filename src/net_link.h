/**********************************************************
 *  \file net_link.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _net_link_H
#define _net_link_H

#include <queue>

#include "fr_template/lock_queue.hpp"
#include "fr_public/pub_memory.h"

#include "hpsocket/SocketInterface.h"
#include "hpsocket/HPSocket.h"

namespace frrpc{
namespace network{

class NetLink{
	public:
		NetLink();
		virtual ~NetLink();
	public:
		virtual bool Start(const std::string& ip, Port port) = 0;
		virtual void Stop() = 0;
		virtual bool Disconnect(Socket socket);

		virtual bool Send(Socket socket, const BinaryMemory& binary)=0;
		virtual bool SendGroup(const vector<Socket>& socket, const BinaryMemory& binary)=0;
		virtual bool GetRemoteAddress(Socket socket, std::string& ip, Port& port)=0;
	public:
		inline int64_t max_binary_size()const{ return max_binary_size_; }
		inline int64_t set_max_binary_size(int64_t max_binary_size){ max_binary_size_ = max_binary_size; }
	public:
		void FetchMessageQueue(std::queue<BinaryMemoryPtr>& message_queue, int32_t max_queue_size = 2000);
	protected:
		void PushMessageToQueue(const BinaryMemoryPtr& binary);

		inline head_length()const{ return sizeof(uint32_t); }
	private:
		fr_template::LockQueue<BinaryMemoryPtr> message_queue_;
};

}// namespace network{
}// namespace frrpc{

#endif 

