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

namespace frrpc{
namespace network{

// class RpcServer_Server {{{1

class RpcServer_Server : public RpcBaseNet, public CTcpPullServerListener{
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

		// hpsocket version has a bug : Send big data by multiple thread to the same socket.It does not ensure data order . 
		// Big that means : GetSocketBufferSize()
		// TODO:
		//	Will Changes library of network.
		virtual bool Send(const RpcMeta& meta, const google::protobuf::Message& body);
		virtual bool Send(LinkID link_id, const RpcMeta& meta, const google::protobuf::Message& body);
		virtual bool Send(const std::vector<LinkID>& link_ids, const RpcMeta& meta, const google::protobuf::Message& body);

		virtual bool GetRemoteAddress(LinkID link_id, std::string& ip, Port& port);
	private:
		virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen);
		virtual EnHandleResult OnAccept(ITcpServer* pSender, Socket socket, SOCKET soClient);
		virtual EnHandleResult OnSend(ITcpServer* pSender, Socket socket, const BYTE* pData, int iLength);
		virtual EnHandleResult OnReceive(ITcpServer* pSender, Socket socket, int iLength);
		virtual EnHandleResult OnClose(ITcpServer* pSender, Socket socket, EnSocketOperation enOperation, int iErrorCode);
		virtual EnHandleResult OnShutdown(ITcpServer* pSender);

		virtual bool IsChannel()const;

		// * Is socket always positive? 
		inline LinkID BuildLinkID(Socket socket)const { return (LinkID)socket; }
		inline Socket GetSocket(LinkID link_id)const { return (Socket)link_id; }

		EnHandleResult ReturnError(Socket socket, const std::string& error_info);

		virtual bool SendHeart(LinkID link_id);
	private:
		CTcpPullServerPtr server_;
		std::string ip_;
		Port port_;
		NetInfo net_info_;
};
// }}}1

// class RpcServer_Gate {{{1

class RpcServer_Gate;
// class RpcServer_Gate_Client {{{2
class RpcServer_Gate_Client : public CTcpPullClientListener{
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

		bool Send(LinkID link_id, const RpcMeta& meta, const google::protobuf::Message& body);
		bool Send(const std::vector<LinkID>& link_ids, const RpcMeta& meta, const google::protobuf::Message& body);

		bool SendHeart(LinkID link_id);
	private:
		virtual EnHandleResult OnConnect(ITcpClient* pSender, Socket socket);
		virtual EnHandleResult OnSend(ITcpClient* pSender, Socket socket, const BYTE* pData, int iLength);
		virtual EnHandleResult OnReceive(ITcpClient* pSender, Socket socket, int iLength);
		virtual EnHandleResult OnClose(ITcpClient* pSender, Socket socket, EnSocketOperation enOperation, int iErrorCode);

		// 
		EnHandleResult ReturnError(const std::string& err_info); 
	private:
		CTcpPullClientPtr net_client_;
		GateID gate_id_;
		std::string ip_;
		Port port_;
		RpcServer_Gate* rpc_server_gate_;
		Byte receive_buffer_[NET_PACKET_MAX_SIZE];
};// }}}2

// class RpcServer_Gate {{{2
// TODO:
//	* reconnection
//	* 
class RpcServer_Gate : public RpcBaseNet{
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

		virtual bool Send(const RpcMeta& meta, const google::protobuf::Message& body);
		virtual bool Send(LinkID link_id, const RpcMeta& meta, const google::protobuf::Message& body);
		virtual bool Send(const std::vector<LinkID>& link_ids, const RpcMeta& meta, const google::protobuf::Message& body);

		inline LinkID BuildLinkID(GateID gate_id, Socket socket)const{ return socket * pow(10, gate_length_) + gate_id; }
		inline Socket GetSocket(LinkID link_id)const{ return link_id % (uint32_t)pow(10, gate_length_); }
		inline GateID GetGateID(LinkID link_id)const{ return link_id / (uint32_t)pow(10, gate_length_); }
	private:
		virtual bool IsChannel()const;
		virtual bool SendHeart(LinkID link_id);
	private:
		std::vector<RpcServer_Gate_Client*> gate_client_list_;
		uint32_t gate_length_;
};
// }}}2

// }}}1

} // namespace network
} // namespace frrpc

#endif 

