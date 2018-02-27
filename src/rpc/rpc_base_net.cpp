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

bool RpcBaseNet::GetMessageFromBinary(const BinaryMemory& binary, int offset, NetInfo& net_info, RpcPacketPtr& packet){// {{{2
	uint32_t cur_offset(sizeof(PacketSize) + offset);

	NetInfoSize net_size = *(NetInfoSize*)binary.buffer(cur_offset);
	cur_offset += sizeof(NetInfoSize);
	if(net_size > 0){
		if(!net_info.ParseFromArray(binary.buffer(cur_offset), net_size)){
			RPC_DEBUG_E("Fail to parse net_info.");
			return false;
		}
		cur_offset += net_size;
	}

	if(net_info.net_type() <= eNetType_Special){
		if(packet != NULL){
			RpcMetaSize meta_size = *(RpcMetaSize*)binary.buffer(cur_offset);
			cur_offset += sizeof(RpcMetaSize);
			if(meta_size > 0){
				if(!packet->rpc_meta.ParseFromArray(binary.buffer(cur_offset), meta_size)){
					RPC_DEBUG_E("Fail to parse rpc meta.");
					return false;
				}
				cur_offset += meta_size;
			}
			else{
				RPC_DEBUG_E("rpc meta is zero.");
				return false;
			}

			RpcBodySize body_size = binary.size() - cur_offset;
			if(body_size > 0){
				packet->binary = BinaryMemoryPtr(new BinaryMemory());
				packet->binary->add(binary.buffer(cur_offset), body_size);
				if(packet->binary == NULL){
					RPC_DEBUG_E("new failed.");
					return false;
				}
			}
		}
		else{
			RPC_DEBUG_E("Message is not special.And packet is null.");
			return false;
		}
	}

	return true;
}// }}}2

BinaryMemoryPtr RpcBaseNet::BuildBinaryFromMessage(const NetInfo& net_info){// {{{2
	BinaryMemoryPtr binary(new BinaryMemory());
	if(binary != NULL){
		NetInfoSize net_info_size(net_info.ByteSize());
		PacketSize packet_size(sizeof(NetInfoSize) + net_info_size);

		binary->reserve(sizeof(PacketSize) + packet_size);
		if(binary->buffer() != NULL){
			PushNumberToBinaryMemory(packet_size, binary);
			
			PushNumberToBinaryMemory(net_info_size, binary);
			if(!net_info.SerializeToArray(binary->CopyMemoryFromOut(net_info.ByteSize()), net_info.ByteSize())){
				RPC_DEBUG_E("Fail to serialize net info.");
				return NULL;
			}
		}
		else{
			RPC_DEBUG_E("Fail to new buffer ");
		}
	}
	return binary;
}// }}}2

BinaryMemoryPtr RpcBaseNet::BuildBinaryFromMessage(const NetInfo& net_info, const RpcMeta& meta, const Message& body){// {{{2
	BinaryMemoryPtr binary(new BinaryMemory());
	if(binary != NULL){
		NetInfoSize net_info_size(net_info.ByteSize());
		RpcMetaSize rpc_meta_size(meta.ByteSize());
		PacketSize packet_size(sizeof(NetInfoSize) + net_info_size + sizeof(RpcMetaSize) + rpc_meta_size + body.ByteSize());

		binary->reserve(sizeof(PacketSize) + packet_size);
		if(binary->buffer() != NULL){
			PushNumberToBinaryMemory(packet_size, binary);
			
			PushNumberToBinaryMemory(net_info_size, binary);
			if(!net_info.SerializeToArray(binary->CopyMemoryFromOut(net_info.ByteSize()), net_info.ByteSize())){
				RPC_DEBUG_E("Fail to serialize net info.");
				return NULL;
			}

			PushNumberToBinaryMemory(rpc_meta_size, binary);
			if(!meta.SerializeToArray(binary->CopyMemoryFromOut(meta.ByteSize()), meta.ByteSize())){
				RPC_DEBUG_E("Fail to serialize meta.");
				return NULL;
			}

			if(!body.SerializeToArray(binary->CopyMemoryFromOut(body.ByteSize()), body.ByteSize())){
				RPC_DEBUG_E("Fail to serialize body.");
				return NULL;
			}
		}
		else{
			RPC_DEBUG_E("Fail to new buffer ");
		}
	}
	return binary;
}// }}}2

void RpcBaseNet::FetchMessageQueue(std::queue<RpcPacketPtr>& packet_queue, int32_t max_queue_size){// {{{2
	packet_queue_.pop(packet_queue, max_queue_size);
}// }}}2

void RpcBaseNet::PushMessageToQueue(const RpcPacketPtr& packet){// {{{2
	if(packet != NULL){
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

