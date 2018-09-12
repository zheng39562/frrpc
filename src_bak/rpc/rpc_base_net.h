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

#include "frtemplate/lock_queue.hpp"
#include "frpublic/pub_memory.h"

#include "frrpc_define.h"

#include "pb/net.pb.h"
#include "pb/frrpc.pb.h"
#include "public/rpc_serializable.h"
#include "rpc/controller.h"

namespace frrpc{
namespace network{

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

		void FetchMessageQueue(std::queue<RpcPacketPtr>& packet_queue, int32_t max_queue_size);

		void PushMessageToQueue(const RpcPacketPtr& packet);
	protected:
		bool GetAndCheckPacketSize(const void* buffer, PacketSize& size);
	private:
		frtemplate::LockQueue<RpcPacketPtr> packet_queue_;
};

class RpcNetServer : public RpcBaseNet{
	public:
		RpcNetServer()=default;
		virtual ~RpcNetServer()=default;
	public:
		virtual bool Send(LinkId link_id, const RpcMeta& meta, const google::protobuf::Message& body)=0;

		virtual bool RegisterService(const std::string& service_name, const std::string& service_addr)=0;
};

class RpcNetChannel : public RpcBaseNet{
	public:
		RpcNetChannel()=default;
		virtual ~RpcNetChannel()=default;
	public:
		virtual bool Send(const std::string service_name, const RpcMeta& meta, const google::protobuf::Message& body)=0;
};

} // namespace network
} // namespace frrpc

#endif 

