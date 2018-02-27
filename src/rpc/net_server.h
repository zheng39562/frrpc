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

#include <thread>
#include <mutex>
#include <math.h>

#include "rpc_base_net.h"
#include "frrpc_define.h"
#include "frnet/frnet_interface.h"
#include "common/rpc_heart.h"
#include "pb/route.pb.h"

namespace frrpc{
namespace network{

// class RpcServer_Server {{{1

class RpcServer_Server : public RpcNetServer, public frnet::NetListen{
	public:
		RpcServer_Server(const std::string &ip, Port port);
		RpcServer_Server(const RpcServer_Server &ref)=delete;
		RpcServer_Server& operator=(const RpcServer_Server &ref)=delete;
		virtual ~RpcServer_Server();

	public:
		virtual bool Start();
		virtual bool Stop();

		// disconnect is link_id
		virtual bool Disconnect(LinkID link_id);

		virtual bool Send(Controller* cntl, const RpcMeta& meta, const google::protobuf::Message& body);

		virtual bool RegisterService(const std::string& service_name, const std::string& service_addr);

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
		// * Is socket always positive? 
		inline LinkID BuildLinkID(Socket socket)const { return (LinkID)socket; }
		inline Socket GetSocket(LinkID link_id)const { return (Socket)link_id; }

		bool ReturnError(Socket socket, const std::string& error_info);
	private:
		frnet::NetServer* server_;
		std::string ip_;
		Port port_;
		NetInfo net_info_;
		RpcHeart rpc_heart_;
};
// }}}1

// class RpcServer_Gate {{{1

// class RpcServer_Gate_Client {{{2
class RpcServer_Gate;
class RpcServer_Gate_Client : public frnet::NetListen{
	public:
		RpcServer_Gate_Client(RpcServer_Gate* rpc_server_gate, GateID gate_id, const std::string& ip, Port);
		RpcServer_Gate_Client(const RpcServer_Gate_Client& ref)=delete;
		RpcServer_Gate_Client& operator=(const RpcServer_Gate_Client& ref)=delete;
		virtual ~RpcServer_Gate_Client();
	public:
		bool Start();
		bool Stop();

		inline const std::string& ip()const{ return ip_; }
		inline Port port()const{ return port_; }
		inline GateID gate_id()const{ return gate_id_; }

		bool Send(Controller* cntl, const RpcMeta& meta, const google::protobuf::Message& body);

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

		bool PerformRouteCmd(frrpc::network::NetInfo& net_info);
		bool ReceiveEventNotice(frrpc::route::RouteResponse route_response);
	private:
		frnet::NetClient* net_client_;
		GateID gate_id_;
		std::string ip_;
		Port port_;
		RpcServer_Gate* rpc_server_gate_;
		Byte receive_buffer_[NET_PACKET_MAX_SIZE];
		RpcHeart rpc_heart_;
};// }}}2

// class RpcServer_Gate {{{2
// TODO:
//	* reconnection
//	* 
class RpcServer_Gate : public RpcNetServer{
	public:
		friend class RpcServer_Gate_Client;
	public:
		RpcServer_Gate(const std::vector<std::tuple<const std::string&, Port> >& gate_list);
		RpcServer_Gate(const RpcServer_Gate& ref)=delete;
		RpcServer_Gate& operator=(const RpcServer_Gate& ref)=delete;
		virtual ~RpcServer_Gate();
	public:
		virtual bool Start();
		virtual bool Stop();
		
		virtual bool Disconnect(LinkID link_id);

		virtual bool Send(Controller* cntl, const RpcMeta& meta, const google::protobuf::Message& body);

		virtual bool RegisterService(const std::string& service_name, const std::string& service_addr);

		inline LinkID BuildLinkID(GateID gate_id, Socket socket)const{ return socket * pow(10, gate_length_) + gate_id; }
		inline Socket GetSocket(LinkID link_id)const{ return link_id % (uint32_t)pow(10, gate_length_); }
		inline GateID GetGateID(LinkID link_id)const{ return link_id / (uint32_t)pow(10, gate_length_); }
	private:
		std::vector<RpcServer_Gate_Client*> gate_client_list_;
		uint32_t gate_length_;
};
// }}}2

// }}}1

} // namespace network
} // namespace frrpc

#endif 

