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

		// A packet include three part.net information, rpc meta, response.
		//
		// Controller has network information. Base Controller satisfy most of the scene.
		// If You need more.You has two choice : Add variable in base controller, or Inherit base controller.
		//
		virtual bool Send(Controller* cntl, const RpcMeta& meta, const google::protobuf::Message& body)=0;

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
		virtual bool RegisterService(const std::string& service_name, const std::string& service_addr)=0;
};

} // namespace network
} // namespace frrpc

#endif 

