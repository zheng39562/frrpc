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
#include "rpc/frrpc_function.h"

using namespace std;
using namespace frpublic;
using namespace google::protobuf;

namespace frrpc{
namespace network{

RpcBaseNet::RpcBaseNet()
	:packet_queue_()
{
	;
}

RpcBaseNet::~RpcBaseNet(){ 
}

void RpcBaseNet::FetchMessageQueue(std::queue<RpcPacketPtr>& packet_queue, int32_t max_queue_size){
	packet_queue_.pop(packet_queue, max_queue_size);
}

void RpcBaseNet::PushMessageToQueue(const RpcPacketPtr& packet){
	if(packet != NULL){
		RPC_DEBUG_P("receive a complete packet. link id [%d] net event [%d] name [%s.%d] binary [%s]", packet->link_id, packet->net_event, packet->rpc_meta.service_name().c_str(), packet->rpc_meta.method_index(), (packet->binary != nullptr ? packet->binary->to_hex().c_str() : "empty binary"));
		packet_queue_.push(packet);
	}
	else{
		RPC_DEBUG_E("packet is null. Please check.");
	}
}

bool RpcBaseNet::GetAndCheckPacketSize(const void* buffer, PacketSize& size){
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
}

} // namespace network
} // namespace frrpc

