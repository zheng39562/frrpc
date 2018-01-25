/**********************************************************
 *  \file net_server.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "net_server.h"

#include "frpublic/pub_memory.h"
#include "frpublic/pub_tool.h"
#include "controller.h"
#include "frrpc_function.h"

using namespace std;
using namespace frpublic;
using namespace frnet;
using namespace google::protobuf;

namespace frrpc{
namespace network{

// Class RpcServer_Server {{{1
//

RpcServer_Server::RpcServer_Server(const std::string &ip, Port port)// {{{2
	:server_(CreateNetServer(this)),
	 ip_(ip),
	 port_(port),
	 net_info_()
{ 
	net_info_.set_net_type(eNetType_Server); 
}// }}}2

RpcServer_Server::~RpcServer_Server(){ // {{{2
	if(server_ != NULL){
		server_->Stop();
		delete server_;
		server_ = NULL;
	}
}// }}}2

bool RpcServer_Server::Start(){// {{{2
	if(!server_->Start(ip_.c_str(), port_)){
		RPC_DEBUG_E("Fail to start server");
		return false;
	}
	RPC_DEBUG_I("Listen [" << ip_ << ":" << port_ << "]");

	RunHeartCheck(NET_HEART_TIMEOUT);
	return true;
}// }}}2

bool RpcServer_Server::Stop(){// {{{2
	if(!server_->Stop()){
		RPC_DEBUG_E("Fail to stop server.");
		return false;
	}
	RPC_DEBUG_I("stop server.");

	StopHeartCheck();
	return true;
}// }}}2

bool RpcServer_Server::Disconnect(LinkID link_id){// {{{2
	return server_->Disconnect(BuildLinkID(link_id));
}// }}}2

bool RpcServer_Server::Send(const RpcMeta& meta, const Message& body){// {{{2
	return false;
}// }}}2

bool RpcServer_Server::Send(LinkID link_id, const RpcMeta& meta, const Message& body){// {{{2
	vector<LinkID> link_ids({link_id});
	return Send(link_ids, meta, body);
}// }}}2

bool RpcServer_Server::Send(const vector<LinkID>& link_ids, const RpcMeta& meta, const Message& body){// {{{2
	bool ret(false);

	BinaryMemoryPtr binary = BuildBinaryFromMessage(net_info_, meta, body);
	if(binary != NULL){
		ret = true;
		for(auto& link_id : link_ids){
			if(!server_->Send(GetSocket(link_id), binary)){
				RPC_DEBUG_E("Fail to send body. Detail : link_id [" << link_id << "] body size [" << binary->size() << "]");
				ret = false;
				continue;
			}
		}
	}
	else{
		RPC_DEBUG_E("Fail to build binary(bianry is null).");
	}
	return ret;
}// }}}2

bool RpcServer_Server::OnReceive(Socket socket, const frpublic::BinaryMemory& binary, size_t& read_size){// {{{2
	UpdateHeartTime(socket);

	int32_t offset(0);
	while((binary.size() - read_size) > sizeof(PacketSize)){
		PacketSize size(*(const PacketSize*)binary.buffer()); 
		if(size + sizeof(size) > (binary.size() - read_size)){ 
			return true; 
		}

		if(size >= NET_PACKET_MAX_SIZE){ return ReturnError(socket, "net_type_size is bigger than buffer. Please reset buffer size(recompile)."); }
		if(size == 0){ return ReturnError(socket, "size is zero."); }

		NetInfo net_info;
		RpcPacketPtr packet(new RpcPacket(BuildLinkID(socket), eNetEvent_Method));
		if(packet != NULL){
			if(!GetMessageFromBinary(binary, offset, net_info, packet)){
				return ReturnError(socket, "Error : GetMessageFromBinary.");
			}

			if(net_info.net_type() == net_info_.net_type()){
				PushMessageToQueue(packet);
			}
			else{
				return SendHeart(packet->link_id);
			}
		}
		else{
			RPC_DEBUG_E("Fail to new packet.");
			return false;
		}

		read_size += size + sizeof(size);
		offset += size + sizeof(size);
	}
	return true;
}// }}}2

void RpcServer_Server::OnConnect(Socket socket){// {{{2
	PushMessageToQueue(RpcPacketPtr(new RpcPacket(BuildLinkID(socket), eNetEvent_Connection)));
	AddLinkID(BuildLinkID(socket));
}// }}}2

void RpcServer_Server::OnDisconnect(Socket socket){// {{{2
	PushMessageToQueue(RpcPacketPtr(new RpcPacket(BuildLinkID(socket), eNetEvent_Disconnection)));
	DelLinkID(BuildLinkID(socket));
}// }}}2

void RpcServer_Server::OnClose(){// {{{2
	NET_DEBUG_D("close net server.");
}//}}}2

void RpcServer_Server::OnError(const NetError& net_error){// {{{2
	NET_DEBUG_D("Receive error [" << net_error.err_no << "].");
}//}}}2

bool RpcServer_Server::IsChannel()const{// {{{2
	return false;
}// }}}2

bool RpcServer_Server::ReturnError(Socket socket, const std::string& error_info){// {{{2
	RPC_DEBUG_E(error_info << " Disconnect socket [" << socket << "]");
	Disconnect(BuildLinkID(socket));
	return false;
};// }}}2

bool RpcServer_Server::SendHeart(LinkID link_id){// {{{2
	NetInfo net_info;
	net_info.set_net_type(eNetType_Heart);
	BinaryMemoryPtr binary = BuildBinaryFromMessage(net_info);
	if(binary != NULL){
		return server_->Send(BuildLinkID(link_id), binary);
	}
	return false;
}// }}}2

// }}}1


// class RpcServer_Gate {{{1

RpcServer_Gate_Client::RpcServer_Gate_Client(RpcServer_Gate* rpc_server_gate, GateID gate_id, const std::string& ip, Port port)// {{{2
	:net_client_(CreateNetClient(this)),
	 gate_id_(gate_id),
	 ip_(ip),
	 port_(port),
	 rpc_server_gate_(rpc_server_gate)
{
	 memset(receive_buffer_, 0, sizeof(receive_buffer_));
}// }}}2

RpcServer_Gate_Client::~RpcServer_Gate_Client(){// {{{2
	if(net_client_ != NULL){
		net_client_->Stop();
		delete net_client_;
		net_client_ = NULL;
	}
}// }}}2

bool RpcServer_Gate_Client::Start(){// {{{2
	if(!net_client_->Start(ip_.c_str(), port_)){
		RPC_DEBUG_E("Fail to start[" << ip_ << ":" << port_ << "]");
		return false;
	}
	return true;
}// }}}2

bool RpcServer_Gate_Client::Stop(){// {{{2
	return net_client_->Stop();
}// }}}2
	
bool RpcServer_Gate_Client::Send(LinkID link_id, const RpcMeta& meta, const Message& body){// {{{2
	return Send({link_id}, meta, body);
}// }}}2

bool RpcServer_Gate_Client::Send(const vector<LinkID>& link_ids, const RpcMeta& meta, const Message& body){// {{{2
	bool ret(false);

	NetInfo net_info;
	for(auto& link_id : link_ids){
		if(gate_id() == rpc_server_gate_->GetGateID(link_id)){
			net_info.add_sockets(rpc_server_gate_->GetSocket(link_id));
		}
	}
	if(net_info.sockets_size() > 0){
		BinaryMemoryPtr binary = rpc_server_gate_->BuildBinaryFromMessage(net_info, meta, body);
		if(binary != NULL){
			ret = net_client_->Send(binary);
			if(!ret){
				RPC_DEBUG_E("Fail to send binary.");
			}
		}
	}
	else{
		RPC_DEBUG_E("None socket is belong to this gate.");
	}

	return ret;
}// }}}2

bool RpcServer_Gate_Client::OnReceive(Socket socket, const frpublic::BinaryMemory& binary, size_t& read_size){// {{{2
	int32_t offset(0);
	while((binary.size() - read_size) > sizeof(PacketSize)){
		PacketSize size(*(const PacketSize*)binary.buffer()); 
		if(size + sizeof(size) > (binary.size() - read_size)){ 
			return true; 
		}

		if(size >= NET_PACKET_MAX_SIZE){ return ReturnError("net_type_size is bigger than buffer. Please reset buffer size(recompile)."); }
		if(size == 0){ return ReturnError("size is zero."); }

		NetInfo net_info;
		RpcPacketPtr packet(new RpcPacket(0, eNetEvent_Method));
		if(packet != NULL){
			if(!rpc_server_gate_->GetMessageFromBinary(binary, offset, net_info, packet)){
				return ReturnError("Error : GetMessageFromBinary.");
			}

			for(int index = 0; index < net_info.sockets_size(); ++index){
				LinkID link_id = rpc_server_gate_->BuildLinkID(gate_id(), net_info.sockets(index));
				rpc_server_gate_->UpdateHeartTime(link_id);

				if(net_info.net_type() == eNetType_Gate){
					RpcPacketPtr packet_tmp(new RpcPacket(*packet));
					packet_tmp->link_id = link_id;
					rpc_server_gate_->PushMessageToQueue(packet_tmp);
				}
				else if(net_info.net_type() == eNetType_Heart){
					return SendHeart(link_id);
				}
			}
		}
		else{
			RPC_DEBUG_E("Fail to new packet.");
			return false;
		}

		read_size += size + sizeof(size);
		offset += size + sizeof(size);
	}
	return true;
}// }}}2

void RpcServer_Gate_Client::OnConnect(Socket socket){// {{{2
}// }}}2

void RpcServer_Gate_Client::OnDisconnect(Socket sockfd){// {{{2
	;
}//}}}2

void RpcServer_Gate_Client::OnClose(){// {{{2
}// }}}2

void RpcServer_Gate_Client::OnError(const NetError& net_error){// {{{2
}// }}}2

bool RpcServer_Gate_Client::ReturnError(const std::string& err_info){// {{{2
	RPC_DEBUG_E(err_info);
	return false; 
}// }}}2

bool RpcServer_Gate_Client::SendHeart(LinkID link_id){// {{{2
	NetInfo net_info;
	net_info.set_net_type(eNetType_Heart);
	net_info.add_sockets(rpc_server_gate_->GetSocket(link_id));
	BinaryMemoryPtr binary = rpc_server_gate_->BuildBinaryFromMessage(net_info);
	if(binary != NULL){
		return net_client_->Send(binary);
	}
	return false;
}// }}}2

RpcServer_Gate::RpcServer_Gate(const std::vector<std::tuple<const std::string&, Port> >& gate_list)// {{{2
	:gate_client_list_(),
	 gate_length_(GetNumberLength(gate_list.size()))
{
	for(auto& gate_info : gate_list){
		gate_client_list_.push_back(new RpcServer_Gate_Client(this, gate_client_list_.size(), get<0>(gate_info), get<1>(gate_info)));
	}
}// }}}2

RpcServer_Gate::~RpcServer_Gate(){// {{{2
	Stop();

	for(auto& gate_client : gate_client_list_){
		if(gate_client == NULL){
			DELETE_POINT_IF_NOT_NULL(gate_client);
		}
	}
}// }}}2

bool RpcServer_Gate::Start(){// {{{2
	for(auto& gate_client : gate_client_list_){
		gate_client->Start();
	}
}// }}}2

bool RpcServer_Gate::Stop(){// {{{2
	bool ret(true);
	for(auto& gate_client : gate_client_list_){
		if(!gate_client->Stop()){
			RPC_DEBUG_D("Fail to stop gate[" << gate_client->gate_id() << "].");
			ret = false;
		}
	}
	return ret;
}// }}}2

bool RpcServer_Gate::Disconnect(LinkID link_id){// {{{2
	// todo: send disconnect message to gate.
	return true;
}// }}}2

bool RpcServer_Gate::Send(const RpcMeta& meta, const Message& body){// {{{2
	return false;
}// }}}2

bool RpcServer_Gate::Send(LinkID link_id, const RpcMeta& meta, const Message& body){// {{{2
	GateID gate_id(GetGateID(link_id));
	if(gate_id < gate_client_list_.size()){
		return gate_client_list_[gate_id]->Send(link_id, meta, body);
	}
	return false;
}// }}}2

bool RpcServer_Gate::Send(const vector<LinkID>& link_ids, const RpcMeta& meta, const Message& body){// {{{2
	set<GateID> gate_ids;
	for(auto& link_id : link_ids){
		gate_ids.insert(GetGateID(link_id));
	}

	bool ret(false);
	if(!gate_ids.empty()){
		ret = true;
		for(auto& gate_id : gate_ids){
			if(gate_id < gate_client_list_.size()){
				ret &= gate_client_list_[gate_id]->Send(link_ids, meta, body);
			}
		}
	}
	return ret;
}// }}}2

bool RpcServer_Gate::IsChannel()const{// {{{2
	return false;
}// }}}2

bool RpcServer_Gate::SendHeart(LinkID link_id){// {{{2
	GateID gate_id(GetGateID(link_id));
	if(gate_id < gate_client_list_.size()){
		return gate_client_list_[gate_id]->SendHeart(link_id);
	}
	return false;
}// }}}2

// }}}1

} // namespace network
} // namespace frrpc


