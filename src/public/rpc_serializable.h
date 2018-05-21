/**********************************************************
 *  \file rpc_serializable.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _rpc_serializable_H
#define _rpc_serializable_H

#include "frpublic/pub_memory.h"
#include "pb/net.pb.h"
#include "pb/frrpc.pb.h"
#include "public_define.h"

namespace frrpc{//{{{1

// class RpcPacket{{{2

class RpcPacket{
	public:
		RpcPacket(LinkID _link_id, network::eNetEvent _net_event): link_id(_link_id), net_event(_net_event), rpc_meta(), binary(){ ; }
		RpcPacket(const RpcPacket& ref){
			link_id = ref.link_id;
			net_event = ref.net_event;
			rpc_meta = ref.rpc_meta;

			if(binary == NULL){
				binary = frpublic::BinaryMemoryPtr(new frpublic::BinaryMemory());
			}
			*binary = *ref.binary;
		}
		RpcPacket& operator=(const RpcPacket& ref){
			link_id = ref.link_id;
			net_event = ref.net_event;
			rpc_meta = ref.rpc_meta;

			if(binary == NULL){
				binary = frpublic::BinaryMemoryPtr(new frpublic::BinaryMemory());
			}
			*binary = *ref.binary;
		}
		~RpcPacket()=default;
	public:
		inline void Clear(){
			link_id = 0;
			rpc_meta.Clear();
			binary->clear();
		}
		inline std::string info()const{
			std::string info;
			info += "link_id : " + std::to_string(link_id);
			info += " net_event : " + std::to_string(net_event);
			info += " service name : " + rpc_meta.service_name() + " method index : " + std::to_string(rpc_meta.method_index());
			info += " binary size " + (binary == NULL ? "nullptr" : std::to_string(binary->size()));
			return info;
		}
	public:
		LinkID link_id;
		network::eNetEvent net_event;
		RpcMeta rpc_meta;
		frpublic::BinaryMemoryPtr binary;
};
typedef std::shared_ptr<RpcPacket> RpcPacketPtr;
// }}}2

// packet struct : net_size(2) + net_info + meta_size(2) + meta_size + binary
//
frpublic::BinaryMemoryPtr BuildBinaryFromMessage(const network::NetInfo& net_info);
frpublic::BinaryMemoryPtr BuildBinaryFromMessage(const network::NetInfo& net_info, const RpcMeta& meta, const google::protobuf::Message& body);

// parse binary, set net_info and packet.
// notice : packet has many variables. It only set net_event, rpc_meta and binary.
bool GetMessageFromBinary(const frpublic::BinaryMemory& binary, int32_t offset, network::NetInfo& net_info, RpcPacketPtr& packet);

}//}}}1

#endif 

