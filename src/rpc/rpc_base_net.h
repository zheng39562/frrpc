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

#include "frrpc_define.h"
#include "net_link.h"
#include "net.pb.h"
#include "frrpc.pb.h"

namespace frrpc{
namespace network{

// class RpcServerPacket{{{1
class RpcServerPacket{
	public:
		RpcServerPacket(): link_id(0), rpc_meta(), binary(){ ; }
		~RpcServerPacket()=default;
	public:
		inline void Clear(){
			link_id = 0;
			rpc_meta.Clear();
			binary->reset();
		}
	public:
		LinkId link_id;
		RpcMeta rpc_meta;
		BinaryMemoryPtr binary;
};
typedef std::shared_ptr<RpcServerPacket> RpcServerPacketPtr;
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

	// Public function {{{2
	public:
		virtual bool Start()=0;
		virtual void Stop()=0;

		// hpsocket version has a bug : Send big data by multiple thread to the same socket.It does not ensure data order . 
		// Big that means : GetSocketBufferSize()
		// TODO:
		//	Will Changes library of network.
		virtual bool Send(LinkID link_ids, const fr_public::BinaryMemory& binary)=0;
		virtual bool Send(const vector<LinkID>& link_ids, const fr_public::BinaryMemory& binary)=0;
		bool Send(LinkID link_id, const RpcMeta& meta, const Message& body);
		bool Send(const vector<LinkID>& link_ids, const RpcMeta& meta, const Message& body);

		inline void FetchMessageQueue(std::queue<RpcServerPacketPtr>& packet_queue, int32_t max_queue_size = 2000){
			if(rpc_base_net != NULL){ rpc_base_net->FetchMessageQueue(packet_queue, max_queue_size); }
		}

		void FetchMessageQueue(std::queue<RpcServerPacketPtr>& packet_queue, int32_t max_queue_size);
	// }}}2

	// Protected function {{{2
	protected:
		void PushMessageToQueue(const RpcServerPacketPtr& packet);
	// }}}2
	
	// Private function {{{2
	private:
		// Build binary.
		// body is a abstract name that means request or response.
		BinaryMemoryPtr BuildBinaryFromMessage(const RpcMeta& meta, const Message& body);
	// }}}2

	private:
		fr_template::LockQueue<RpcServerPacketPtr> packet_queue_;
		atomic<LinkID> link_id_;
};
// }}}1

} // namespace network
} // namespace frrpc

#endif 

