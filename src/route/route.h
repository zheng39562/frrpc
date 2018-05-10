/**********************************************************
 *  \file route.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _route_H
#define _route_H

#include <mutex>
#include <atomic>
#include <memory>

#include "pb/net.pb.h"
#include "pb/route.pb.h"
#include "frnet/frnet_interface.h"
#include "common/rpc_heart.h"

#include "route_define.h"

namespace frrpc{ // {{{1
namespace route{ 

	std::string GetCommandName(eRouteCmd cmd);

// class RouteServiceInfos {{{2
class RouteServiceInfos{
	public:
		RouteServiceInfos(const std::string& service_name);
		~RouteServiceInfos();
	public:
		Socket GetServiceSocket(const std::string& service_addr);

		bool AddService(Socket service_socket, const std::string& service_addr);
		bool DeleteService(Socket service_socket);

		inline size_t ServiceSize()const{ return sockets_.size(); }
	private:
		std::string service_name_;
		std::vector<Socket> sockets_;
		std::map<std::string, Socket> addr_2socket_;

		// if service is not special. We use this variable to choose service.
		// one serivce receive a message.(Balanced Load)
		int32_t cur_send_index_;
};
typedef std::shared_ptr<RouteServiceInfos> RouteServiceInfosPtr;
//}}}2

// class RpcRoute{{{2
// 
// * 接受channel的数据，并根据servicename转发给service.
// * channel未绑定service时(无状态服): 针对此的均衡负载方案无，暂时以顺序去发送(即每个service平均分担一个消息来解决.)
// * channel绑定service时(有状态服): 发送给对应绑定service.
// * route 需要提供绑定的功能.
//
// TODO:
//	* 当前使用锁的方案，性能可能会是瓶颈，回头使用atomic和mutex混用的方案解决.(也需要demo 去证实此方案确实会加快效率)
//
//	* err_no 实现：定义和对应返回到channel端
class RpcRoute : public frnet::NetListen{
	private:
		typedef std::string ServiceName;

	public:
		RpcRoute();
		virtual ~RpcRoute();
	public:
		inline bool Start(const std::string& ip, Port port){ 
			if(net_server_->Start(ip, port)){ rpc_heart_.RunServer(net_server_); return true; }
			else{ return false; } 
		}
		inline bool Stop(){ 
			rpc_heart_.StopHeartCheck();
			return net_server_->Stop(); 
		}
	protected:
		// param[out] read_size : 
		//	delete date size when function finish. Set 0 If you do not want delete any data.
		//
		// retval : close client if return is false.
		virtual bool OnReceive(Socket sockfd, const frpublic::BinaryMemory& binary, size_t& read_size);

		virtual void OnConnect(Socket sockfd);
		virtual void OnDisconnect(Socket sockfd);

		virtual void OnClose();

		// include all error : read, write, disconnect and so on.
		virtual void OnError(const frnet::NetError& net_error);
	private:
		bool GetNetInfo(const frpublic::BinaryMemory& binary, int32_t offset, frrpc::network::NetInfo& net_info);

		bool RouteProcess(Socket socket, const frpublic::BinaryMemory& binary, int32_t offset, frrpc::network::NetInfo& net_info);
		bool CommandProcess(Socket socket, const frpublic::BinaryMemory& binary, int32_t offset, frrpc::network::NetInfo& net_info);
		bool HeartProcess(Socket socket, const frpublic::BinaryMemory& binary, int32_t offset, frrpc::network::NetInfo& net_info);

		bool EventRegister(Socket socket, const std::string& binary);
		bool EventCancel(Socket socket, const std::string& binary);
		bool EventNoticeDisconnect(Socket socket, const std::string& binary);

		bool ServiceRegister(Socket socket, const std::string& binary);
		bool ServiceCancel(Socket socket);
		bool AddService(Socket service_socket, const std::string& service_name, const std::string& service_addr);
		bool DeleteService(Socket service_socket);
		inline bool IsServiceSocket(Socket socket)const{ return service_socket_2name_.find(socket) != service_socket_2name_.end(); };

		bool SendEventNotice_Disconnect(Socket disconnect_socket);

		frpublic::BinaryMemoryPtr BuildSendPacket(const frpublic::BinaryMemory& binary, int32_t offset, const frrpc::network::NetInfo& net_info);
	private:
		frnet::NetServer* net_server_;
		RpcHeart rpc_heart_;

		std::mutex mutex_service_2info_;
		std::map<ServiceName, RouteServiceInfosPtr> service_2info_;
		std::map<Socket, ServiceName> service_socket_2name_;

		std::mutex mutex_event_disconnect_;
		std::map<Socket, std::set<Socket> > event_dis_notice_2listen_; // socket listen socket list
		std::map<Socket, std::set<Socket> > event_dis_listen_2notice_; // notice list when socket disconnect
};
//}}}2

} // namespace route
} // namespace frrpc
//}}}1

#endif 

