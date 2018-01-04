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

#include <thread>
#include <mutex>
#include <math.h>

#include "rpc_base_net.h"
#include "frrpc_define.h"

namespace frrpc{
namespace network{

// class RpcChannel_Server {{{1

class RpcChannel_Server : public RpcBaseNet, public CTcpPullClientListener{
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

		// hpsocket version has a bug : Send big data by multiple thread to the same socket.It does not ensure data order . 
		// Big that means : GetSocketBufferSize()
		// TODO:
		//	Will Changes library of network.
		virtual bool Send(const RpcMeta& meta, const google::protobuf::Message& body);
		virtual bool Send(LinkID link_id, const RpcMeta& meta, const google::protobuf::Message& body);
		virtual bool Send(const std::vector<LinkID>& link_ids, const RpcMeta& meta, const google::protobuf::Message& body);

		virtual bool GetRemoteAddress(LinkID link_id, std::string& ip, Port& port);
	private:
		virtual EnHandleResult OnConnect(ITcpClient* pSender, Socket socket);
		virtual EnHandleResult OnSend(ITcpClient* pSender, Socket socket, const BYTE* pData, int iLength);
		virtual EnHandleResult OnReceive(ITcpClient* pSender, Socket socket, int iLength);
		virtual EnHandleResult OnClose(ITcpClient* pSender, Socket socket, EnSocketOperation enOperation, int iErrorCode);

		EnHandleResult ReturnError(const std::string& error_info);
	private:
		virtual bool IsChannel()const;
		virtual bool SendHeart(LinkID link_id);
	private:
		CTcpPullClientPtr net_client_;
		std::string ip_;
		Port port_;
		NetInfo net_info_;
};
// }}}1

// class RpcChannel_Gate {{{1

typedef RpcChannel_Server RpcChannel_Gate;

// }}}1

} // namespace network
} // namespace frrpc

#endif 

