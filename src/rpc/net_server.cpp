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
using namespace frrpc::route;
using namespace google::protobuf;


#define DEBUG_PB_PARSE_FAILURE(name) RPC_DEBUG_E("Fail to parse " << name)

namespace frrpc{
namespace network{

// Class RpcServer_Server {{{1
//

RpcServer_Server::RpcServer_Server(const std::string &ip, Port port)// {{{2
	:server_(CreateNetServer(this)),
	 ip_(ip),
	 port_(port),
	 net_info_(),
	 rpc_heart_()
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
	
	rpc_heart_.RunServer(server_);
	return true;
}// }}}2

bool RpcServer_Server::Stop(){// {{{2
	if(!server_->Stop()){
		RPC_DEBUG_E("Fail to stop server.");
		return false;
	}
	rpc_heart_.StopHeartCheck();

	RPC_DEBUG_I("stop server.");
	return true;
}// }}}2

bool RpcServer_Server::Disconnect(LinkID link_id){// {{{2
	return server_->Disconnect(BuildLinkID(link_id));
}// }}}2

bool RpcServer_Server::Send(Controller* cntl, const RpcMeta& meta, const google::protobuf::Message& body){//{{{2
	bool ret(false);

	BinaryMemoryPtr binary = BuildBinaryFromMessage(net_info_, meta, body);
	if(binary != NULL){
		ret = true;
		for(int index = 0; index < cntl->link_size(); ++index){
			LinkID link_id = cntl->link_id(index);
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
}//}}}2

bool RpcServer_Server::RegisterService(const std::string& service_name, const std::string& service_addr){//{{{2
	return true;
}//}}}2

bool RpcServer_Server::OnReceive(Socket socket, const frpublic::BinaryMemory& binary, size_t& read_size){// {{{2
	rpc_heart_.UpdateSocket(socket);

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
	rpc_heart_.AddSocket(socket);
}// }}}2

void RpcServer_Server::OnDisconnect(Socket socket){// {{{2
	PushMessageToQueue(RpcPacketPtr(new RpcPacket(BuildLinkID(socket), eNetEvent_Disconnection)));
	rpc_heart_.DelSocket(socket);
}// }}}2

void RpcServer_Server::OnClose(){// {{{2
	NET_DEBUG_D("close net server.");
}//}}}2

void RpcServer_Server::OnError(const NetError& net_error){// {{{2
	NET_DEBUG_D("Receive error [" << net_error.err_no << "].");
}//}}}2

bool RpcServer_Server::ReturnError(Socket socket, const std::string& error_info){// {{{2
	RPC_DEBUG_E(error_info << " Disconnect socket [" << socket << "]");
	Disconnect(BuildLinkID(socket));
	return false;
};// }}}2

// }}}1


// class RpcServer_Gate {{{1

RpcServer_Gate_Client::RpcServer_Gate_Client(RpcServer_Gate* rpc_server_gate, GateID gate_id, const std::string& ip, Port port)// {{{2
	:net_client_(CreateNetClient(this)),
	 gate_id_(gate_id),
	 ip_(ip),
	 port_(port),
	 rpc_server_gate_(rpc_server_gate),
	 rpc_heart_()
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
	rpc_heart_.RunClient(net_client_);
	return true;
}// }}}2

bool RpcServer_Gate_Client::Stop(){// {{{2
	rpc_heart_.StopHeartCheck();
	return net_client_->Stop();
}// }}}2
	
bool RpcServer_Gate_Client::Send(Controller* cntl, const RpcMeta& meta, const google::protobuf::Message& body){// {{{2
	RouteNetInfo route_net_info;
	route_net_info.set_is_channel_packet(false);
	route_net_info.set_service_name(cntl->service_name());
	route_net_info.set_service_addr(cntl->service_addr());

	route_net_info.clear_target_sockets();
	for(int index = 0; index < cntl->link_size(); ++index){
		LinkID link_id(cntl->link_id(index));
		if(gate_id() == rpc_server_gate_->GetGateID(link_id)){
			route_net_info.add_target_sockets(rpc_server_gate_->GetSocket(link_id));
		}
	}

	bool ret(false);
	if(route_net_info.target_sockets_size() > 0){
		NetInfo net_info;
		net_info.set_net_type(eNetType_Route);
		if(route_net_info.SerializeToString(net_info.mutable_net_binary())){
			BinaryMemoryPtr binary = rpc_server_gate_->BuildBinaryFromMessage(net_info, meta, body);
			if(binary != NULL){
				ret = net_client_->Send(binary);
				if(!ret){
					RPC_DEBUG_E("Fail to send binary.");
				}
			}
		}
	}
	else{
		RPC_DEBUG_E("None socket is belong to this gate.");
	}

	return ret;
}//}}}2

bool RpcServer_Gate_Client::RegisterService(const std::string& service_name, const std::string& service_addr){//{{{2
	frrpc::route::RouteServiceInfo route_service_info;
	route_service_info.set_name(service_name);
	route_service_info.set_addr(service_addr);

	frrpc::route::RouteRequest route_request;
	route_request.set_cmd(eRouteCmd_ServiceRegister);
	if(!route_service_info.SerializeToString(route_request.mutable_request_binary())){
		RPC_DEBUG_E("Fail to serialize route service info.");
		return false;
	}
	
	frrpc::network::NetInfo net_info;
	net_info.set_net_type(eNetType_RouteCmd);
	if(!route_request.SerializeToString(net_info.mutable_net_binary())){
		RPC_DEBUG_E("Fail to serialize route request.");
		return false;
	}

	PacketSize size(net_info.ByteSize());

	BinaryMemoryPtr register_packet(new BinaryMemory());
	register_packet->add((void*)&size, sizeof(size));
	if(!net_info.SerializeToArray(register_packet->CopyMemoryFromOut(size), size)){
		RPC_DEBUG_E("Fail to serialize net info.");
		return false;
	}

	return net_client_->Send(register_packet);
}//}}}2

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

			switch(net_info.net_type()){
				case eNetType_Route: if(!ReceiveRoutePacket(net_info, packet)){ return false; } break;
				case eNetType_RouteCmd: if(!PerformRouteCmd(net_info)){ return false; } break;
				case eNetType_Heart: break;
				default: RPC_DEBUG_E("unknow net type [" << net_info.net_type() << "]"); return false;
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

bool RpcServer_Gate_Client::ReceiveRoutePacket(NetInfo& net_info, RpcPacketPtr& packet){//{{{2
	frrpc::route::RouteNetInfo route_net_info;
	if(route_net_info.ParseFromString(net_info.net_binary())){
		for(int index = 0; index < route_net_info.target_sockets_size(); ++index){
			RpcPacketPtr packet_tmp(new RpcPacket(*packet));
			packet_tmp->link_id = rpc_server_gate_->BuildLinkID(gate_id(), route_net_info.target_sockets(index));

			rpc_server_gate_->PushMessageToQueue(packet_tmp);
		}
	}
	return true;
}//}}}2

bool RpcServer_Gate_Client::PerformRouteCmd(NetInfo& net_info){//{{{2
	frrpc::route::RouteResponse route_response;
	if(route_response.ParseFromString(net_info.net_binary())){
		switch(route_response.cmd()){
			case eRouteCmd_EventRegister: RPC_DEBUG_E("This cmd(EventRegister) has not response."); return false;
			case eRouteCmd_EventCancel: RPC_DEBUG_E("This cmd(EventCancel) has not response."); return false;
			case eRouteCmd_EventNotice: return ReceiveEventNotice(route_response);
			case eRouteCmd_ServiceRegister: RPC_DEBUG_E("This cmd(ServerRegister) has not response."); return false;
			case eRouteCmd_ServiceCancel: RPC_DEBUG_E("This cmd(ServerRegister) has not response."); return false;
			default: RPC_DEBUG_E("unknow route command [" << route_response.cmd() << "]."); return false;
		}
	}
	return false;
}//}}}2

bool RpcServer_Gate_Client::ReceiveEventNotice(frrpc::route::RouteResponse route_response){//{{{2
	frrpc::route::EventNotice event_notice;
	if(!event_notice.ParseFromString(route_response.response_binary())){
		DEBUG_PB_PARSE_FAILURE("event notice."); return false;
	}

	switch(event_notice.type()){
		case eRouteEventType_Disconnect:{ 
			frrpc::route::EventNotice_Disconnect event_notice_disconnect;
			if(!event_notice_disconnect.ParseFromString(event_notice.event_binary())){
				DEBUG_PB_PARSE_FAILURE("event notice of disconnect."); return false;
			}
			
			rpc_server_gate_->PushMessageToQueue(RpcPacketPtr(new RpcPacket(rpc_server_gate_->BuildLinkID(gate_id(), event_notice_disconnect.socket()), eNetEvent_Disconnection)));
			break;
		}
	}
	return true;
}//}}}2

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

bool RpcServer_Gate::Send(Controller* cntl, const RpcMeta& meta, const google::protobuf::Message& body){// {{{2
	if(cntl == NULL){ RPC_DEBUG_E("controller is null."); return false; }

	set<GateID> gate_ids;
	for(int index = 0; index < cntl->link_size(); ++index){
		gate_ids.insert(GetGateID(cntl->link_id(index)));
	}

	bool ret(true);
	for(auto& gate_id : gate_ids){
		if(gate_id < gate_client_list_.size()){
			ret &= gate_client_list_[gate_id]->Send(cntl, meta, body);
		}
	}
	return ret;
}// }}}2

bool RpcServer_Gate::RegisterService(const std::string& service_name, const std::string& service_addr){//{{{2
	bool ret(true);
	for(auto& gate_client : gate_client_list_){
		ret &= gate_client->RegisterService(service_name, service_addr);
	}
	return ret;
}//}}}2

// }}}1

} // namespace network
} // namespace frrpc


