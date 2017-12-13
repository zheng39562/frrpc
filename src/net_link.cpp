/**********************************************************
 *  \file net_link.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "net_link.h"

using namespace fr_template;

namespace frrpc{
namespace network{

NetLink::NetLink()
	:max_binary_size_(0xFFFFFFFF),
	 message_queue_()
{ ; }
NetLink::~NetLink(){
	message_queue_.clear();
}


void NetLink::FetchMessageQueue(std::queue<BinaryMemoryPtr>& message_queue, int32_t max_queue_size){
	message_queue.pop(message_queue, max_queue_size);
}

void NetLink::PushMessageToQueue(const BinaryMemoryPtr& binary){
	message_queue.push(binary);
}

}// namespace network{
}// namespace frrpc{

