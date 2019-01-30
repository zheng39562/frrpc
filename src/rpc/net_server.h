/**********************************************************
 *  \file net_server.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _net_server_H
#define _net_server_H

#include <math.h>
#include "frnet/frnet_interface.h"
#include "pb/route.pb.h"
#include "public/rpc_heart.h"
#include "rpc/rpc_base_net.h"

namespace frrpc{
namespace network{

class RpcServer_Route;
class RpcServer_Route_Client : public frnet::NetListen{
	public:
		RpcServer_Route_Client(RpcServer_Route* rpc_server_route, RouteID route_id, const std::string& ip, Port);
		RpcServer_Route_Client(const RpcServer_Route_Client& ref)=delete;
		RpcServer_Route_Client& operator=(const RpcServer_Route_Client& ref)=delete;
		virtual ~RpcServer_Route_Client();
	public:
		bool Start();
		bool Stop();

		inline const std::string& ip()const{ return ip_; }
		inline Port port()const{ return port_; }
		inline RouteID route_id()const{ return route_id_; }

		bool Send(LinkID link_id, const RpcMeta& meta, const google::protobuf::Message& body);
		bool Send(std::vector<LinkID> link_ids, const RpcMeta& meta, const google::protobuf::Message& body);

		bool Disconnect(LinkID link_id);

		bool RegisterService(const std::string& service_name, const std::string& service_addr);
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
		bool ReturnError(const std::string& err_info); 

		bool ReceiveRoutePacket(frrpc::network::NetInfo& net_info, RpcPacketPtr& packet);

		bool doRouteCmd(frrpc::network::NetInfo& net_info);
		bool doRouteNotify(frrpc::network::NetInfo& net_info);
		bool ReceiveEventNotice(frrpc::route::RouteCmdResponse route_response);

		bool Send(frrpc::route::RouteNetInfo* route_net_info, const RpcMeta& meta, const google::protobuf::Message& body);
		bool SendRouteCmd(frrpc::route::eRouteCmd cmd, const google::protobuf::Message& cmd_info);
	private:
		frnet::NetClient* net_client_;
		RouteID route_id_;
		std::string ip_;
		Port port_;
		RpcServer_Route* rpc_server_route_;
		Byte receive_buffer_[NET_PACKET_MAX_SIZE];
		RpcHeart rpc_heart_;
};

// TODO:
//	* reconnection
//	* 
class RpcServer_Route : public RpcNetServer{
	public:
		friend class RpcServer_Route_Client;
	public:
		RpcServer_Route(const std::vector<std::tuple<std::string, Port> >& route_list);
		RpcServer_Route(const RpcServer_Route& ref)=delete;
		RpcServer_Route& operator=(const RpcServer_Route& ref)=delete;
		virtual ~RpcServer_Route();
	public:
		virtual bool Start();
		virtual bool Stop();
		
		virtual bool Disconnect(LinkID link_id);

		virtual bool Send(LinkID link_id, const RpcMeta& meta, const google::protobuf::Message& body);
		virtual bool Send(std::vector<LinkID> link_ids, const RpcMeta& meta, const google::protobuf::Message& body);

		virtual bool RegisterService(const std::string& service_name, const std::string& service_addr);

		inline LinkID BuildLinkID(RouteID route_id, Socket socket)const{ return route_id * pow(10, route_length_) + socket; }
		inline Socket GetSocket(LinkID link_id)const{ return (Socket)(link_id % (uint32_t)pow(10, route_length_)); }
		inline RouteID GetRouteID(LinkID link_id)const{ return (RouteID)(link_id / (uint32_t)pow(10, route_length_)); }
	private:
		RpcServer_Route_Client* getRouteClinet(LinkID link_id);
	private:
		std::vector<RpcServer_Route_Client*> route_client_list_;
		uint32_t route_length_;
};




} // namespace network
} // namespace frrpc

#endif 

