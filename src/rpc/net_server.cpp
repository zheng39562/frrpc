/**********************************************************
 *  \file net_server_route.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "net_server.h"

#include "pb/route.pb.h"
#include "frpublic/pub_memory.h"
#include "frpublic/pub_tool.h"
#include "public/rpc_serializable.h"
#include "rpc/controller.h"
#include "rpc/frrpc_function.h"

using namespace std;
using namespace frpublic;
using namespace frnet;
using namespace frrpc::route;
using namespace google::protobuf;

namespace frrpc{
namespace network{

RpcServer_Route_Client::RpcServer_Route_Client(RpcServer_Route* rpc_server_route, RouteID route_id, const std::string& ip, Port port)
	:net_client_(CreateNetClient(this)),
	 route_id_(route_id),
	 ip_(ip),
	 port_(port),
	 rpc_server_route_(rpc_server_route),
	 rpc_heart_()
{
	 memset(receive_buffer_, 0, sizeof(receive_buffer_));
}

RpcServer_Route_Client::~RpcServer_Route_Client(){
	if(net_client_ != NULL){
		net_client_->Stop();
		delete net_client_;
		net_client_ = NULL;
	}
}

bool RpcServer_Route_Client::Start(){
	RPC_DEBUG_P("Connect Route [%s:%d]", ip_.c_str(), port_);
	if(!net_client_->Start(ip_.c_str(), port_)){
		RPC_DEBUG_P("Fail to start [%s:%d]", ip_.c_str(), port_);
		return false;
	}
	rpc_heart_.RunClient(net_client_);
	return true;
}

bool RpcServer_Route_Client::Stop(){
	rpc_heart_.StopHeartCheck();
	return net_client_->Stop();
}
	
bool RpcServer_Route_Client::Send(LinkID link_id, const RpcMeta& meta, const google::protobuf::Message& body){
	RPC_DEBUG_P("Route [%d] send message. [%s.%d]", route_id(), meta.service_name().c_str(), meta.method_index());

	RouteNetInfo route_net_info;
	route_net_info.set_is_channel_packet(false);
	if(route_id() == rpc_server_route_->GetRouteID(link_id)){
		route_net_info.add_target_sockets(rpc_server_route_->GetSocket(link_id));
	}

	return Send(&route_net_info, meta, body);
}

bool RpcServer_Route_Client::Send(std::vector<LinkID> link_ids, const RpcMeta& meta, const google::protobuf::Message& body){
	RPC_DEBUG_P("Route [%d] send message. [%s.%d]", route_id(), meta.service_name().c_str(), meta.method_index());

	RouteNetInfo route_net_info;
	route_net_info.set_is_channel_packet(false);

	for(LinkID link_id : link_ids){
		if(route_id() == rpc_server_route_->GetRouteID(link_id)){
			route_net_info.add_target_sockets(rpc_server_route_->GetSocket(link_id));
		}
	}

	return Send(&route_net_info, meta, body);
}

bool RpcServer_Route_Client::RegisterService(const std::string& service_name, const std::string& service_addr){
	frrpc::route::RouteServiceInfo route_service_info;
	route_service_info.set_name(service_name);
	route_service_info.set_addr(service_addr);

	frrpc::route::RouteRequest route_request;
	route_request.set_cmd(eRouteCmd_Server_ServiceRegister);
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

	BinaryMemoryPtr register_packet = BuildBinaryFromMessage(net_info);
	return register_packet != NULL && net_client_->Send(register_packet) != eNetSendResult_Ok;
}

bool RpcServer_Route_Client::OnReceive(Socket socket, const frpublic::BinaryMemory& binary, size_t& read_size){
	int32_t offset(0);
	while((binary.size() - offset) > sizeof(PacketSize)){
		PacketSize size(*(const PacketSize*)binary.buffer()); 
		if(size + sizeof(size) > (binary.size() - read_size)){ 
			DEBUG_P("packet is not complete.");
			return true; 
		}

		if(size >= NET_PACKET_MAX_SIZE){ return ReturnError("net_type_size is bigger than buffer. Please reset buffer size(recompile)."); }
		if(size == 0){ return ReturnError("size is zero."); }

		NetInfo net_info;
		RpcPacketPtr packet(new RpcPacket(0, eNetEvent_Method));
		if(packet != NULL){
			if(!GetMessageFromBinary(binary, offset, net_info, packet)){
				return ReturnError("Error : GetMessageFromBinary.");
			}

			switch(net_info.net_type()){
				case eNetType_Route: if(!ReceiveRoutePacket(net_info, packet)){ return false; } break;
				case eNetType_RouteCmd: if(!PerformRouteCmd(net_info)){ return false; } break;
				case eNetType_Heart: break;
				default: RPC_DEBUG_E("unknow net type [%d]", net_info.net_type()); return false;
			}
		}
		else{ RPC_DEBUG_E("Fail to new packet."); return false; }

		read_size += size + sizeof(size);
		offset += size + sizeof(size);
	}
	return true;
}

void RpcServer_Route_Client::OnConnect(Socket socket){
}

void RpcServer_Route_Client::OnDisconnect(Socket sockfd){
	;
}

void RpcServer_Route_Client::OnClose(){
}

void RpcServer_Route_Client::OnError(const NetError& net_error){
}

bool RpcServer_Route_Client::ReturnError(const std::string& err_info){
	RPC_DEBUG_E("%s", err_info.c_str());
	return false; 
}

bool RpcServer_Route_Client::ReceiveRoutePacket(NetInfo& net_info, RpcPacketPtr& packet){

	frrpc::route::RouteNetInfo route_net_info;
	if(route_net_info.ParseFromString(net_info.net_binary())){
		RpcPacketPtr packet_tmp(new RpcPacket(*packet));
		packet_tmp->link_id = rpc_server_route_->BuildLinkID(route_id(), route_net_info.source_socket());
		rpc_server_route_->PushMessageToQueue(packet_tmp);
	}
	else{ DEBUG_E("Fail to parse route net info. net_info size %d", net_info.net_binary().size()); return false; }

	return true;
}

bool RpcServer_Route_Client::PerformRouteCmd(NetInfo& net_info){
	frrpc::route::RouteResponse route_response;
	if(route_response.ParseFromString(net_info.net_binary())){
		switch(route_response.cmd()){
			case eRouteCmd_EventRegister: RPC_DEBUG_E("This cmd(EventRegister) has not response."); return false;
			case eRouteCmd_EventCancel: RPC_DEBUG_E("This cmd(EventCancel) has not response."); return false;
			case eRouteCmd_EventNotice: return ReceiveEventNotice(route_response);
			case eRouteCmd_Server_ServiceRegister: RPC_DEBUG_E("This cmd(ServerRegister) has not response."); return false;
			case eRouteCmd_Server_ServiceCancel: RPC_DEBUG_E("This cmd(ServerRegister) has not response."); return false;
			default: RPC_DEBUG_E("unknow route command [%d]", route_response.cmd()); return false;
		}
	}
	return false;
}

bool RpcServer_Route_Client::ReceiveEventNotice(frrpc::route::RouteResponse route_response){
	frrpc::route::EventNotice event_notice;
	if(!event_notice.ParseFromString(route_response.response_binary())){
		DEBUG_E("Fail to parse event notice."); return false;
	}

	switch(event_notice.type()){
		case eRouteEventType_Disconnect:{ 
			frrpc::route::EventNotice_Disconnect event_notice_disconnect;
			if(!event_notice_disconnect.ParseFromString(event_notice.event_binary())){
				DEBUG_E("Fail to parse event notice of disconnect."); return false;
			}
			
			rpc_server_route_->PushMessageToQueue(RpcPacketPtr(new RpcPacket(rpc_server_route_->BuildLinkID(route_id(), event_notice_disconnect.socket()), eNetEvent_Disconnection)));
			break;
		}
	}
	return true;
}

bool RpcServer_Route_Client::Send(RouteNetInfo* route_net_info, const RpcMeta& meta, const google::protobuf::Message& body){
	if(route_net_info->target_sockets_size() > 0){
		NetInfo net_info;
		net_info.set_net_type(eNetType_Route);
		if(route_net_info->SerializeToString(net_info.mutable_net_binary())){
			BinaryMemoryPtr binary = BuildBinaryFromMessage(net_info, meta, body);
			if(binary != NULL){
				if(net_client_->Send(binary) != eNetSendResult_Ok){
					RPC_DEBUG_E("Fail to send binary."); return false;
				}
			}
		}
	}
	else{ RPC_DEBUG_E("None socket is belong to this route."); return false; }

	return true;
}

RpcServer_Route::RpcServer_Route(const std::vector<std::tuple<std::string, Port> >& route_list)
	:route_client_list_(),
	 route_length_(GetNumberLength(route_list.size()))
{
	for(auto& route_info : route_list){
		route_client_list_.push_back(new RpcServer_Route_Client(this, route_client_list_.size(), get<0>(route_info), get<1>(route_info)));
	}
}

RpcServer_Route::~RpcServer_Route(){
	Stop();

	for(auto& route_client : route_client_list_){
		if(route_client == NULL){
			DELETE_POINT_IF_NOT_NULL(route_client);
		}
	}
}

bool RpcServer_Route::Start(){
	bool ret(true);
	for(auto& route_client : route_client_list_){
		ret &= route_client->Start();
	}
	return ret;
}

bool RpcServer_Route::Stop(){
	bool ret(true);
	for(auto& route_client : route_client_list_){
		if(!route_client->Stop()){
			RPC_DEBUG_D("Fail to stop route[%d]", route_client->route_id());
			ret = false;
		}
	}
	return ret;
}

bool RpcServer_Route::Disconnect(LinkID link_id){
	// todo: send disconnect message to route.
	return true;
}

bool RpcServer_Route::Send(LinkID link_id, const RpcMeta& meta, const google::protobuf::Message& body){
	RouteID route_id = GetRouteID(link_id);
	if(0 <= route_id && route_id < route_client_list_.size()){
		return route_client_list_[route_id]->Send(link_id, meta, body);
	}

	RPC_DEBUG_P("route_id is wrong. route id [%d]", route_id); 
	return false;
}

bool RpcServer_Route::Send(std::vector<LinkID> link_ids, const RpcMeta& meta, const google::protobuf::Message& body){
	set<RouteID> route_ids;
	for(LinkID link_id : link_ids){
		RPC_DEBUG_P("send to route [%d] route length [%d] link id [%d]", GetRouteID(link_id), route_length_, link_id);
		route_ids.insert(GetRouteID(link_id));
	}

	bool ret(!route_ids.empty());
	for(auto route_id : route_ids){
		if(route_id < route_client_list_.size()){
			ret &= (route_client_list_[route_id]->Send(link_ids, meta, body) == eNetSendResult_Ok);
		}
	}
	return ret;
}

bool RpcServer_Route::RegisterService(const std::string& service_name, const std::string& service_addr){
	bool ret(true);
	for(auto& route_client : route_client_list_){
		ret &= route_client->RegisterService(service_name, service_addr);
	}
	return ret;
}

} // namespace network
} // namespace frrpc


