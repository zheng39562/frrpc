/**********************************************************
 *  \file net_base.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "net_base.h"

namespace frrpc{
namespace network{

// Class RpcBaseNet {{{1

RpcBaseNet::RpcBaseNet()
	:packet_queue_(),
	 link_id_(0)
{
	;
}

RpcBaseNet::~RpcBaseNet(){ ; }


bool RpcBaseNet::Send(LinkID link_id, const RpcMeta& meta, const Message& body){// {{{2
	BinaryMemoryPtr binary(BuildBinaryFromMessage(mata, body));
	if(binary == NULL){
		return Send(link_id, binary);
	}
	DEBUG_E("Fail to build binary.");
	return false;
}// }}}2

bool RpcBaseNet::Send(const vector<LinkID>& link_ids, const RpcMeta& meta, const Message& body){// {{{2
	BinaryMemoryPtr binary(BuildBinaryFromMessage(mata, body));
	if(binary == NULL){
		return Send(link_ids, binary);
	}
	DEBUG_E("Fail to build binary.");
	return false;
}// }}}2

BinaryMemoryPtr RpcBaseNet::BuildBinaryFromMessage(const RpcMeta& meta, const Message& body){// {{{2
	BinaryMemoryPtr binary(new BinaryMemory());
	if(binary != NULL){
		binary->reserve(meta.ByteSize() + body.ByteSize());
		if(binary->buffer() != NULL){
			if(!meta.SerializeToArray(binary->buffer(), meta.ByteSize())){
				DEBUG_E("Fail to serialize meta.");
				return NULL;
			}
			if(!body.SerializeToArray(ADDR_MOVE_BY_BYTE(binary->buffer() + meta.ByteSize()), body.ByteSize())){
				DEBUG_E("Fail to serialize body.");
				return NULL;
			}
		}
		else{
			DEBUG_E("Fail to new buffer ");
		}
	}
	return binary;
}// }}}2

void RpcBaseNet::FetchMessageQueue(std::queue<RpcServerPacketPtr>& packet_queue, int32_t max_queue_size){// {{{2
	packet_queue_.pop(packet_queue, max_queue_size);
}// }}}2

void RpcBaseNet::PushMessageToQueue(const RpcServerPacketPtr& packet){// {{{2
	packet_queue_.push(packet);
}// }}}2

// Class RpcBaseNet }}}1

} // namespace network
} // namespace frrpc
