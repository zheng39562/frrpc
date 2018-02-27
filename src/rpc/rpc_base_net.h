/**********************************************************
 *  \file net_base.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _net_base_H
#define _net_base_H

#include "pb/net.pb.h"
#include "pb/frrpc.pb.h"

#include "frtemplate/lock_queue.hpp"
#include "frpublic/pub_memory.h"
#include "frrpc_define.h"
#include "controller.h"

namespace frrpc{
namespace network{

// class RpcPacket{{{1

class RpcPacket{
	public:
		RpcPacket(LinkID _link_id, eNetEvent _net_event): link_id(_link_id), net_event(_net_event), rpc_meta(), binary(){ ; }
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
	public:
		LinkID link_id;
		eNetEvent net_event;
		RpcMeta rpc_meta;
		frpublic::BinaryMemoryPtr binary;
};
typedef std::shared_ptr<RpcPacket> RpcPacketPtr;
// }}}1


// class RpcBaseNet{{{1
//
// Example:
//	RpcBaseNet* rpc_net = new RpcXXX();
//	rpc_net->Start();
//
class RpcBaseNet{
	public:
		RpcBaseNet();
		virtual ~RpcBaseNet();

	public:
		virtual bool Start()=0;
		virtual bool Stop()=0;

		// disconnect is link_id
		virtual bool Disconnect(LinkID link_id)=0;

		// A packet include three part.net information, rpc meta, response.
		//
		// Controller has network information. Base Controller satisfy most of the scene.
		// If You need more.You has two choice : Add variable in base controller, or Inherit base controller.
		//
		virtual bool Send(Controller* cntl, const RpcMeta& meta, const google::protobuf::Message& body)=0;

		void FetchMessageQueue(std::queue<RpcPacketPtr>& packet_queue, int32_t max_queue_size);
	protected:
		// parse binary, set net_info and packet.
		// notice : packet has many variables. It only set net_event, rpc_meta and binary.
		// 
		// binary struct : net_size(2) + net_info + meta_size(2) + meta_size + binary
		bool GetMessageFromBinary(const frpublic::BinaryMemory& binary, int32_t offset, NetInfo& net_info, RpcPacketPtr& packet);
		// Build binary.
		// body is a abstract name that means request or response.
		frpublic::BinaryMemoryPtr BuildBinaryFromMessage(const NetInfo& net_info);
		frpublic::BinaryMemoryPtr BuildBinaryFromMessage(const NetInfo& net_info, const RpcMeta& meta, const google::protobuf::Message& body);

		//
		void PushMessageToQueue(const RpcPacketPtr& packet);

		bool GetAndCheckPacketSize(const void* buffer, PacketSize& size);
	private:
		frtemplate::LockQueue<RpcPacketPtr> packet_queue_;
};
// }}}1

class RpcNetServer : public RpcBaseNet{
	public:
		RpcNetServer()=default;
		virtual ~RpcNetServer()=default;
	public:
		virtual bool RegisterService(const std::string& service_name, const std::string& service_addr)=0;
};

} // namespace network
} // namespace frrpc

#endif 

