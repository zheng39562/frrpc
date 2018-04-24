/**********************************************************
 *  \file net_base.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "rpc_base_net.h"

#include "frpublic/pub_tool.h"
#include "frrpc_function.h"

using namespace std;
using namespace frpublic;
using namespace google::protobuf;

namespace frrpc{
namespace network{

// Class RpcBaseNet {{{1

RpcBaseNet::RpcBaseNet()// {{{2
	:packet_queue_()
{
	;
}// }}}2

RpcBaseNet::~RpcBaseNet(){ // {{{2
}// }}}2

void RpcBaseNet::FetchMessageQueue(std::queue<RpcPacketPtr>& packet_queue, int32_t max_queue_size){// {{{2
	packet_queue_.pop(packet_queue, max_queue_size);
}// }}}2

void RpcBaseNet::PushMessageToQueue(const RpcPacketPtr& packet){// {{{2
	if(packet != NULL){
		RPC_DEBUG_P("receive a complete packet. " 
				<< "link_id [" << packet->link_id << "]" << "net_event [" << packet->net_event << "]" 
				<< "name [" << packet->rpc_meta.service_name() << "." << packet->rpc_meta.method_index() << "]" << "binary [" << (packet->binary != nullptr ? packet->binary->to_hex() : "empty binary") << "]");
		packet_queue_.push(packet);
	}
	else{
		RPC_DEBUG_E("packet is null. Please check.");
	}
}// }}}2

bool RpcBaseNet::GetAndCheckPacketSize(const void* buffer, PacketSize& size){// {{{2
	if(buffer == NULL){
		RPC_DEBUG_E("point is null."); return false;
	}

	size = *(const PacketSize*)buffer; 
	if(size >= NET_PACKET_MAX_SIZE){ 
		RPC_DEBUG_E("net_type_size is bigger than buffer. Please reset buffer size(recompile)."); return false; 
	}
	if(size == 0){ 
		RPC_DEBUG_E("size is zero."); return false; 
	}
	return true;
}//}}}2

// Class RpcBaseNet }}}1

} // namespace network
} // namespace frrpc

