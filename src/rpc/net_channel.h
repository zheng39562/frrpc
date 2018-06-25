/**********************************************************
 *  \file net_client.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#ifndef _net_client_H
#define _net_client_H

#include <math.h>
#include "frnet/frnet_interface.h"
#include "pb/route.pb.h"
#include "public/rpc_heart.h"
#include "rpc/rpc_base_net.h"

namespace frrpc{
namespace network{

class RpcChannel_Server : public RpcBaseNet, public frnet::NetListen{
	public:
		RpcChannel_Server(const std::string &ip, Port port);
		RpcChannel_Server(const RpcChannel_Server &ref)=delete;
		RpcChannel_Server& operator=(const RpcChannel_Server &ref)=delete;
		virtual ~RpcChannel_Server();
	public:
		virtual bool Start();
		virtual bool Stop();

		// client does not need it.
		// If you want disconnect you can call Stop;
		virtual bool Disconnect(LinkID link_id);

		virtual bool Send(Controller* cntl, const RpcMeta& meta, const google::protobuf::Message& body);

		virtual bool GetRemoteAddress(LinkID link_id, std::string& ip, Port& port);
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
		bool ReturnError(const std::string& error_info);
	private:
		frnet::NetClient* net_client_;
		std::string ip_;
		Port port_;
		RpcHeart rpc_heart_;
};


class RpcChannel_Route : public RpcBaseNet, public frnet::NetListen{
	public:
		RpcChannel_Route(const std::string &ip, Port port);
		RpcChannel_Route(const RpcChannel_Route &ref)=delete;
		RpcChannel_Route& operator=(const RpcChannel_Route &ref)=delete;
		virtual ~RpcChannel_Route();
	public:
		virtual bool Start();
		virtual bool Stop();

		// client does not need it.
		// If you want disconnect you can call Stop;
		virtual bool Disconnect(LinkID link_id);

		virtual bool Send(Controller* cntl, const RpcMeta& meta, const google::protobuf::Message& body);

		inline const std::string& ip()const{ return ip_; }
		inline Port port()const{ return port_; }
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
		virtual bool IsChannel()const;
		virtual bool SendHeart(LinkID link_id);

	private:
		frnet::NetClient* net_client_;
		std::string ip_;
		Port port_;
		RpcHeart rpc_heart_;
};



} // namespace network
} // namespace frrpc

#endif 

