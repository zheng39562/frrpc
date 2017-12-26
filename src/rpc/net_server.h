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

#include "frrpc_define.h"
#include "rpc_base_net.h"

namespace frrpc{
namespace network{

class RpcServer_Server : public RpcBaseNet, public NetLink, public CTcpPullServerListener{
	public:
		RpcServer_Server(const std::string &ip, Port port);
		RpcServer_Server(const RpcServer_Server &ref)=delete;
		RpcServer_Server& operator=(const RpcServer_Server &ref)=delete;
		virtual ~RpcServer_Server();

	// Public function {{{2
	public:
		virtual bool Start();
		virtual void Stop();

		// hpsocket version has a bug : Send big data by multiple thread to the same socket.It does not ensure data order . 
		// Big that means : GetSocketBufferSize()
		// TODO:
		//	Will Changes library of network.
		virtual bool Send(LinkID link_ids, const fr_public::BinaryMemory& binary);
		virtual bool Send(const vector<LinkID>& link_ids, const fr_public::BinaryMemory& binary);

		// disconnect is link_id
		virtual bool Disconnect(LinkID link_id);

		virtual bool GetRemoteAddress(LinkID link_id, std::string& ip, Port& port);
	// }}}2
	
	// Private function {{{2
	private:
		virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen);
		virtual EnHandleResult OnAccept(ITcpServer* pSender, Socket socket, SOCKET soClient);
		virtual EnHandleResult OnSend(ITcpServer* pSender, Socket socket, const BYTE* pData, int iLength);
		virtual EnHandleResult OnReceive(ITcpServer* pSender, Socket socket, int iLength);
		virtual EnHandleResult OnClose(ITcpServer* pSender, Socket socket, EnSocketOperation enOperation, int iErrorCode);
		virtual EnHandleResult OnShutdown(ITcpServer* pSender);
	// }}}2
	
	private:
		CTcpPullServerPtr server_;
		std::map<LinkID, std::tuple<eNetType, NetInfo> > link_id_2info_;
};

class RpcServer_Gate : public FrTcpLinker, public CTcpPullClientListener{
	public:
		RpcServer_Gate(int32_t gate_id);
		RpcServer_Gate(const RpcServer_Gate& ref)=delete;
		RpcServer_Gate& operator=(const RpcServer_Gate& ref)=delete;
		virtual ~RpcServer_Gate();
	public:
		virtual bool Start(const std::string& ip, Port port);
		virtual void Stop();
		//
		virtual bool Disconnect(Socket socket);
		// 
		virtual bool Send(Socket socket, const BinaryMemory& binary)=0;
		virtual bool SendGroup(const vector<Socket>& socket, const BinaryMemory& binary)=0;
	private:
		virtual EnHandleResult OnConnect(ITcpClient* pSender, Socket socket);
		virtual EnHandleResult OnSend(ITcpClient* pSender, Socket socket, const BYTE* pData, int iLength);
		virtual EnHandleResult OnReceive(ITcpClient* pSender, Socket socket, int iLength);
		virtual EnHandleResult OnClose(ITcpClient* pSender, Socket socket, EnSocketOperation enOperation, int iErrorCode);
	private:
		CTcpPullClientPtr m_pClient;
		std::string gate_ip;
		Port gate_port;
};

} // namespace network
} // namespace frrpc

#endif 

