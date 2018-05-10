/**********************************************************
 *  \file rpc_serializable.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "rpc_serializable.h"

using namespace frrpc::network;
using namespace frpublic;
using namespace google::protobuf;

namespace frrpc{//{{{1

BinaryMemoryPtr BuildBinaryFromMessage(const NetInfo& net_info){// {{{2
	BinaryMemoryPtr binary(new BinaryMemory());
	if(binary != NULL){
		NetInfoSize net_info_size(net_info.ByteSize());
		PacketSize packet_size(sizeof(NetInfoSize) + net_info_size);

		binary->reserve(sizeof(PacketSize) + packet_size);
		if(binary->buffer() != NULL){
			PushNumberToBinaryMemory(packet_size, binary);
			
			PushNumberToBinaryMemory(net_info_size, binary);
			if(!net_info.SerializeToArray(binary->CopyMemoryFromOut(net_info.ByteSize()), net_info.ByteSize())){
				DEBUG_E("Fail to serialize net info.");
				return NULL;
			}
		}
		else{
			DEBUG_E("Fail to new buffer ");
		}

	}
	return binary;
}// }}}2

BinaryMemoryPtr BuildBinaryFromMessage(const NetInfo& net_info, const RpcMeta& meta, const Message& body){// {{{2
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
				DEBUG_E("Fail to serialize net info.");
				return NULL;
			}

			PushNumberToBinaryMemory(rpc_meta_size, binary);
			if(!meta.SerializeToArray(binary->CopyMemoryFromOut(meta.ByteSize()), meta.ByteSize())){
				DEBUG_E("Fail to serialize meta.");
				return NULL;
			}

			if(!body.SerializeToArray(binary->CopyMemoryFromOut(body.ByteSize()), body.ByteSize())){
				DEBUG_E("Fail to serialize body.");
				return NULL;
			}

			DEBUG_P("size " << binary->size()
				<< " packet binary [" << binary->to_hex(0, sizeof(PacketSize)) << "][" << packet_size << "]"
				<< " net binary [" << binary->to_hex(sizeof(PacketSize) + sizeof(NetInfoSize), net_info_size) << "][" << net_info_size << "]"
				<< " meta binary [" << binary->to_hex(sizeof(PacketSize) + sizeof(NetInfoSize) + net_info_size + sizeof(RpcMetaSize), rpc_meta_size) << "][" << rpc_meta_size << "]"
				<< " body binary [" << binary->to_hex(sizeof(PacketSize) + sizeof(NetInfoSize) + net_info_size + sizeof(RpcMetaSize) + rpc_meta_size, body.ByteSize()) << "][" << body.ByteSize() << "]");
		}
		else{ DEBUG_E("Fail to new buffer "); }
	}

	return binary;
}// }}}2

bool GetMessageFromBinary(const BinaryMemory& binary, int offset, NetInfo& net_info, RpcPacketPtr& packet){// {{{2
	PacketSize packet_size = *(PacketSize*)binary.buffer(offset);
	uint32_t cur_offset(sizeof(PacketSize) + offset);

	NetInfoSize net_size = *(NetInfoSize*)binary.buffer(cur_offset);
	cur_offset += sizeof(NetInfoSize);
	if(net_size > 0){
		if(!net_info.ParseFromArray(binary.buffer(cur_offset), net_size)){
			DEBUG_E("Fail to parse net_info.");
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
					DEBUG_E("Fail to parse rpc meta.");
					return false;
				}
				cur_offset += meta_size;
			}
			else{
				DEBUG_E("rpc meta is zero.");
				return false;
			}

			RpcBodySize body_size = packet_size - sizeof(NetInfoSize) - net_size - sizeof(RpcMetaSize) - meta_size;
			if(body_size > 0){
				DEBUG_P("size " << binary.size()
					<< " packet binary [" << binary.to_hex(offset, sizeof(PacketSize)) << "][" << packet_size << "]"
					<< " net binary [" << binary.to_hex(offset + sizeof(PacketSize) + sizeof(NetInfoSize), net_size) << "][" << net_size << "]"
					<< " meta binary [" << binary.to_hex(offset + sizeof(PacketSize) + sizeof(NetInfoSize) + net_size + sizeof(RpcMetaSize), meta_size) << "][" << meta_size << "]"
					<< " body binary [" << binary.to_hex(offset + sizeof(PacketSize) + sizeof(NetInfoSize) + net_size + sizeof(RpcMetaSize) + meta_size, body_size) << "][" << body_size << "]");
				packet->binary = BinaryMemoryPtr(new BinaryMemory());
				packet->binary->add(binary.buffer(cur_offset), body_size);
				if(packet->binary == NULL){
					DEBUG_E("new failed.");
					return false;
				}
			}
		}
		else{
			DEBUG_E("Message is not special.And packet is null.");
			return false;
		}
	}

	return true;
}// }}}2

}//}}}1

