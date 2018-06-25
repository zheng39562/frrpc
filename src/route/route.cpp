/**********************************************************
 *  \file route.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "route.h"

using namespace std;
using namespace frpublic;
using namespace frnet;
using namespace frrpc;
using namespace frrpc::network;


namespace frrpc{ 
namespace route{ 

std::string GetCommandName(eRouteCmd cmd){
	switch(cmd){
		case eRouteCmd_EventRegister : return "Command-Event-Register";
		case eRouteCmd_EventCancel : return "Command-Event-Cancel";
		case eRouteCmd_EventNotice : return "Command-Event-Notice";
		case eRouteCmd_ServiceRegister : return "Command-Service-Register";
		case eRouteCmd_ServiceCancel : return "Command-Service-Cancel";
		default: return "Unkonw Command";
	}
}

} // namepsace route
} // namepsace frrpc


namespace frrpc{ 
namespace route{ 

RouteServiceInfos::RouteServiceInfos(const std::string& service_name) 
	:service_name_(service_name),
	 sockets_(),
	 addr_2socket_(),
	 cur_send_index_(0)
{
	;
}

RouteServiceInfos::~RouteServiceInfos(){
}

Socket RouteServiceInfos::GetServiceSocket(const std::string& service_addr){
	Socket service_socket(SOCKET_NULL);

	if(!sockets_.empty()){
		if(service_addr.empty()){
			cur_send_index_ = (cur_send_index_ + 1) % sockets_.size();
			service_socket = sockets_.at(cur_send_index_);
		}
		else{
			auto addr_2socket_iter = addr_2socket_.find(service_addr); 
			if(addr_2socket_iter != addr_2socket_.end()){
				service_socket = addr_2socket_iter->second;
			}
			else{
				DEBUG_E("Can not find this addr[%s]", service_addr.c_str());
			}
		}
	}
	else{
		DEBUG_E("Server does not connect this route.");
	}
	return service_socket;
}

bool RouteServiceInfos::AddService(Socket service_socket, const std::string& service_addr){
	if(addr_2socket_.find(service_addr) == addr_2socket_.end()){
		sockets_.push_back(service_socket);
		addr_2socket_.insert(make_pair(service_addr, service_socket));
	}
	else{ DEBUG_E("Service add repeat. socket [%d] addr [%s]", service_socket, service_addr.c_str()); return false; }

	return true;
}

bool RouteServiceInfos::DeleteService(Socket service_socket){
	for(auto sockets_iter = sockets_.begin(); sockets_iter != sockets_.end(); ++sockets_iter){
		if(service_socket == *sockets_iter){
			sockets_.erase(sockets_iter);
			break;
		}
	}

	for(auto addr_2socket_iter = addr_2socket_.begin(); addr_2socket_iter != addr_2socket_.end(); ++addr_2socket_iter){
		if(service_socket == addr_2socket_iter->second){
			addr_2socket_.erase(addr_2socket_iter);
			break;
		}
	}
	return true;
}

} // namepsace route
} // namepsace frrpc


namespace frrpc{ 
namespace route{ 

RpcRoute::RpcRoute()
	:net_server_(CreateNetServer(this)),
	 rpc_heart_(),
	 mutex_service_2info_(),
	 service_2info_(),
	 service_socket_2name_(),
	 mutex_event_disconnect_(),
	 event_dis_notice_2listen_(),
	 event_dis_listen_2notice_()
{
	;
}

RpcRoute::~RpcRoute(){
	if(net_server_ != NULL){
		delete net_server_; net_server_ = NULL;
	}

	rpc_heart_.StopHeartCheck();
}

bool RpcRoute::OnReceive(Socket sockfd, const frpublic::BinaryMemory& binary, size_t& read_size){
	rpc_heart_.UpdateSocket(sockfd);

	int32_t cur_offset(0);
	frrpc::network::NetInfo net_info;
	while((binary.size() - cur_offset) > sizeof(PacketSize)){
		PacketSize size(*(const PacketSize*)binary.buffer(cur_offset)); 
		if(size >= NET_PACKET_MAX_SIZE){ DEBUG_E("net_type_size is bigger than buffer[%d]. Please reset buffer size(recompile). socket [%d]", size, sockfd); return false; }
		if(size == 0){ DEBUG_E("size is zero. socket [%d]", sockfd); return false; }

		PacketSize cur_packet_size = size + sizeof(size);
		if(cur_packet_size > (binary.size() - cur_offset)){ 
			return true; 
		}

		if(!GetNetInfo(binary, cur_offset, net_info)){
			DEBUG_E("Fail to parse binary. net info socket [%d] cur_offset [%d] binary size [%d] binary [%s]", sockfd, cur_offset, binary.size(), binary.to_hex().c_str()); return false;
		}

		DEBUG_I("receive message from sockfd [%d]. net type[%s] binary [%s]", sockfd, frrpc::network::eNetEvent_Name(net_info.net_type()).c_str(), binary.to_hex(cur_offset, size).c_str());

		bool ret(false);
		switch(net_info.net_type()){
			case eNetType_Route: ret = RouteProcess(sockfd, binary, cur_offset, net_info); break;
			case eNetType_RouteCmd: ret = CommandProcess(sockfd, binary, cur_offset, net_info); break;
			case eNetType_Heart: ret = HeartProcess(sockfd, binary, cur_offset, net_info); break;
			default : DEBUG_E("unknow net type [%d]", net_info.net_type()); return false;
		}
		if(!ret){ return false; }

		cur_offset += cur_packet_size;
	}

	read_size = cur_offset;
	return true;
}

void RpcRoute::OnConnect(Socket sockfd){
	rpc_heart_.AddSocket(sockfd);
}

void RpcRoute::OnDisconnect(Socket sockfd){
	rpc_heart_.DelSocket(sockfd);

	if(IsServiceSocket(sockfd)){
		DeleteService(sockfd);
	}

	auto DeleteSlefOperator = [](std::map<Socket, std::set<Socket> > a_2b, std::map<Socket, std::set<Socket> > b_2a, Socket a){
		auto a_2b_iter = a_2b.find(a);
		if(a_2b_iter != a_2b.end()){
			for(auto b : a_2b_iter->second){
				auto b_2a_iter = b_2a.find(b);
				if(b_2a_iter != b_2a.end()){
					b_2a_iter->second.erase(a);
				}
			}
		}
	};

	lock_guard<mutex> lock(mutex_event_disconnect_);

	if(!SendEventNotice_Disconnect(sockfd)){
		DEBUG_E("Fail to send notice(disconnect)");
	}

	DeleteSlefOperator(event_dis_notice_2listen_, event_dis_listen_2notice_, sockfd);
	DeleteSlefOperator(event_dis_listen_2notice_, event_dis_notice_2listen_, sockfd);
}

void RpcRoute::OnClose(){
	mutex_service_2info_.lock();
	service_2info_.clear();
	service_socket_2name_.clear();
	mutex_service_2info_.unlock();

	mutex_event_disconnect_.lock();
	event_dis_notice_2listen_.clear();
	event_dis_listen_2notice_.clear();
	mutex_event_disconnect_.unlock();

}

void RpcRoute::OnError(const frnet::NetError& net_error){
	DEBUG_E("Receive network error. socket [%d] errno [%d]", net_error.sockfd, net_error.err_no);
}

bool RpcRoute::GetNetInfo(const frpublic::BinaryMemory& binary, int32_t offset, frrpc::network::NetInfo& net_info){
	int32_t cur_offset(sizeof(PacketSize) + offset);

	NetInfoSize net_size = *(NetInfoSize*)binary.buffer(cur_offset);
	cur_offset += sizeof(NetInfoSize);
	if(net_size > 0){
		if(!net_info.ParseFromArray(binary.buffer(cur_offset), net_size)){
			DEBUG_E("Fail to serialize net info. cur_offset [%d] net_size[%d] binary size[%d]", cur_offset, net_size, binary.size()); return false;
		}
	}
	else{ DEBUG_E("net_size less that 0. net_size [%d]", net_size); return false; }
	return true;
}

bool RpcRoute::RouteProcess(Socket socket, const frpublic::BinaryMemory& binary, int32_t offset, frrpc::network::NetInfo& net_info){
	frrpc::route::RouteNetInfo route_net_info;
	if(route_net_info.ParseFromString(net_info.net_binary())){
		BinaryMemoryPtr send_packet;
		if(route_net_info.is_channel_packet()){
			route_net_info.set_source_socket(socket);
			if(!route_net_info.SerializeToString(net_info.mutable_net_binary())){
				DEBUG_PB_SERIALIZE_FAILURE("new net info"); return false;
			}

			route_net_info.clear_target_sockets();
			{
				lock_guard<mutex> local_lock(mutex_service_2info_);
				auto service_2info_iter = service_2info_.find(route_net_info.service_name());
				if(service_2info_iter == service_2info_.end()){
					string service_names;
					for(auto& service_info_item : service_2info_){
						service_names += "," + service_info_item.first;
					}
					DEBUG_W("Can not find service name [%s] service name list [%s]", route_net_info.service_name().c_str(), service_names.c_str());
					return true;
				}
				route_net_info.add_target_sockets(service_2info_iter->second->GetServiceSocket(route_net_info.service_addr()));
			}

			send_packet = BuildSendPacket(binary, offset, net_info);
		}
		else{
			PacketSize size = *(PacketSize*)binary.buffer(offset);
	
			send_packet = BinaryMemoryPtr(new BinaryMemory());
			send_packet.add(binary.buffer(offset), sizeof(PacketSize) + size);
		}

		for(int index = 0; index < route_net_info.target_sockets_size(); ++index){
			if(net_server_->Send(route_net_info.target_sockets(index), send_packet) != eNetSendResult_Ok){
				DEBUG_E("Fail to send message. socket [%d]", route_net_info.target_sockets(index));
			}
		}
	}
	else{ DEBUG_PB_PARSE_FAILURE("route net info."); return false; }

	return true;
}

bool RpcRoute::CommandProcess(Socket socket, const frpublic::BinaryMemory& binary, int32_t offset, frrpc::network::NetInfo& net_info){
	frrpc::route::RouteRequest route_request;
	if(route_request.ParseFromString(net_info.net_binary())){
		DEBUG_P("Receive a route command: ", GetCommandName(route_request.cmd()).c_str());
		switch(route_request.cmd()){
			case eRouteCmd_EventRegister: return EventRegister(socket, route_request.request_binary());
			case eRouteCmd_EventCancel: return EventCancel(socket, route_request.request_binary());
			case eRouteCmd_ServiceRegister: return ServiceRegister(socket, route_request.request_binary());
			case eRouteCmd_ServiceCancel: return ServiceCancel(socket);
			case eRouteCmd_EventNotice : DEBUG_W("Command-Event-Notice is not defined.");
			default : DEBUG_E("Unkonw route command [%d]", route_request.cmd()); return false;
		}
	}
	else{ DEBUG_PB_PARSE_FAILURE("route request."); return false; }

	return true;
}

bool RpcRoute::HeartProcess(Socket socket, const frpublic::BinaryMemory& binary, int32_t offset, frrpc::network::NetInfo& net_info){
	DEBUG_P("net heart packet.");
	PacketSize size = *(const PacketSize*)binary.buffer(offset);
	return net_server_->Send(socket, BinaryMemoryPtr(new BinaryMemory(binary.buffer(offset), size + sizeof(PacketSize)))) == eNetSendResult_Ok;
}

bool RpcRoute::EventRegister(Socket socket, const std::string& binary){
	EventNotice event_notice;
	if(!event_notice.ParseFromString(binary)){
		DEBUG_PB_PARSE_FAILURE("event notice."); return false;
	}

	switch(event_notice.type()){
		case eRouteEventType_Disconnect: return EventNoticeDisconnect(socket, event_notice.event_binary());
		default: DEBUG_E("Unknow event type [%d]", event_notice.type()); return false;
	}
	return false;
}

bool RpcRoute::EventCancel(Socket socket, const std::string& binary){
	auto EraseOperator = [](std::map<Socket, std::set<Socket> >& a_2b, Socket a, Socket b){
		auto a_2b_iter = a_2b.find(a);
		if(a_2b_iter != a_2b.end()){
			a_2b_iter->second.erase(b);
			if(a_2b_iter->second.empty()){
				a_2b.erase(a_2b_iter);
			}
		}
	};

	EventNotice_Disconnect event_notice_disconnect;
	if(!event_notice_disconnect.ParseFromString(binary)){
		DEBUG_E("event notice disconnect. socket [%d]", socket); return false;
	}

	std::lock_guard<mutex> local_lock(mutex_event_disconnect_);
	EraseOperator(event_dis_notice_2listen_, socket, event_notice_disconnect.socket());
	EraseOperator(event_dis_listen_2notice_, event_notice_disconnect.socket(), socket);
	return true;
}

bool RpcRoute::EventNoticeDisconnect(Socket socket, const std::string& binary){
	auto InsertOperator = [](std::map<Socket, std::set<Socket> >& a_2b, Socket a, Socket b){
		auto a_2b_iter = a_2b.find(a);
		if(a_2b_iter == a_2b.end()){
			a_2b.insert(make_pair(a, set<Socket>()));
			auto a_2b_iter = a_2b.find(a);
		}
		a_2b_iter->second.insert(b);
	};

	EventNotice_Disconnect event_notice_disconnect;
	if(!event_notice_disconnect.ParseFromString(binary)){
		DEBUG_E("event notice disconnect. socket [%d]", socket); return false;
	}

	std::lock_guard<mutex> local_lock(mutex_event_disconnect_);

	InsertOperator(event_dis_notice_2listen_, socket, event_notice_disconnect.socket());
	InsertOperator(event_dis_listen_2notice_, event_notice_disconnect.socket(), socket);
	return true;
}

bool RpcRoute::ServiceRegister(Socket socket, const std::string& binary){
	RouteServiceInfo route_service_info;
	if(!route_service_info.ParseFromString(binary)){
		DEBUG_E("Fail to parse route service info. socket [%d]", socket); return false;
	}

	return AddService(socket, route_service_info.name(), route_service_info.addr());
}

bool RpcRoute::ServiceCancel(Socket socket){
	return DeleteService(socket);
}

bool RpcRoute::AddService(Socket service_socket, const std::string& service_name, const std::string& service_addr){
	DEBUG_I("Register new service. socket [%d] service name [%s] service addr [%s]", service_socket, service_name.c_str(), service_addr.c_str());

	std::lock_guard<mutex> local_lock(mutex_service_2info_);
	service_socket_2name_.insert(make_pair(service_socket, service_name));

	auto service_2info_iter = service_2info_.find(service_name);
	if(service_2info_iter == service_2info_.end()){
		service_2info_.insert(make_pair(service_name, RouteServiceInfosPtr(new RouteServiceInfos(service_name))));
		service_2info_iter = service_2info_.find(service_name);
	}

	return service_2info_iter->second->AddService(service_socket, service_addr);
}

bool RpcRoute::DeleteService(Socket service_socket){

	std::lock_guard<mutex> local_lock(mutex_service_2info_);

	auto service_socket_2name_iter = service_socket_2name_.find(service_socket);
	if(service_socket_2name_iter != service_socket_2name_.end()){
		auto service_2info_iter = service_2info_.find(service_socket_2name_iter->second);
		if(service_2info_iter != service_2info_.end()){
			DEBUG_I("Unregister service. socket [%d] service_name [%s]", service_socket, service_2info_iter->first.c_str());
			service_2info_iter->second->DeleteService(service_socket);
			if(service_2info_iter->second->ServiceSize() == 0){
				service_2info_.erase(service_2info_iter);
			}
		}
		else{ DEBUG_E("service name[%s] is wrong.", service_socket_2name_iter->second.c_str()); return false;}

		service_socket_2name_.erase(service_socket_2name_iter);
	}
	else{ DEBUG_E("scoket [%d] is not a service socket.", service_socket); return false;}
	return true;
}

bool RpcRoute::SendEventNotice_Disconnect(Socket disconnect_socket){
	frrpc::route::EventNotice_Disconnect notice_disconnect;
	notice_disconnect.set_socket(disconnect_socket);

	frrpc::route::EventNotice event_notice;
	event_notice.set_type(eRouteEventType_Disconnect);
	if(!notice_disconnect.SerializeToString(event_notice.mutable_event_binary())){
		DEBUG_PB_SERIALIZE_FAILURE("event notice disconnect."); return false;
	}

	frrpc::route::RouteResponse route_response;
	route_response.set_cmd(eRouteCmd_EventNotice);
	if(!event_notice.SerializeToString(route_response.mutable_response_binary())){
		DEBUG_PB_SERIALIZE_FAILURE("event notice. "); return false;
	}

	frrpc::network::NetInfo net_info;
	net_info.set_net_type(eNetType_RouteCmd);
	if(!route_response.SerializeToString(net_info.mutable_net_binary())){
		DEBUG_PB_SERIALIZE_FAILURE("route response."); return false;
	}

	PacketSize size(net_info.ByteSize());

	BinaryMemoryPtr packet(new BinaryMemory());
	packet->add((void*)&size, sizeof(size));
	if(!net_info.SerializeToArray(packet->CopyMemoryFromOut(size), size)){
		DEBUG_PB_SERIALIZE_FAILURE("net info."); return false;
	}

	bool ret(true);
	auto event_dis_listen_2notice_iter = event_dis_listen_2notice_.find(disconnect_socket);
	if(event_dis_listen_2notice_iter != event_dis_listen_2notice_.end()){
		for(auto& socket : event_dis_listen_2notice_iter->second){
			if(!net_server_->Send(socket, packet)){
				DEBUG_E("fail to send notice of disconnect.");
				ret = false;
			}
		}
	}
	return ret;
}

BinaryMemoryPtr RpcRoute::BuildSendPacket(const frpublic::BinaryMemory& binary, int32_t offset, const frrpc::network::NetInfo& net_info){
	BinaryMemoryPtr send_packet(new BinaryMemory());

	PacketSize original_size = *(const PacketSize*)binary.buffer(offset);
	NetInfoSize original_net_info_size = *(const NetInfoSize*)binary.buffer(offset + sizeof(PacketSize));

	int32_t copy_offset = offset + sizeof(PacketSize) + sizeof(NetInfoSize) + original_net_info_size;
	PacketSize copy_size = original_size - sizeof(NetInfoSize) - original_net_info_size;
	
	NetInfoSize cur_net_info_size = net_info.ByteSize();
	PacketSize packet_size = original_size + cur_net_info_size - original_net_info_size;

	send_packet->reserve(sizeof(PacketSize)+ packet_size);
	send_packet->add((void*)&packet_size, sizeof(packet_size));
	send_packet->add((void*)&cur_net_info_size, sizeof(cur_net_info_size));
	if(net_info.SerializeToArray(send_packet->CopyMemoryFromOut(cur_net_info_size), cur_net_info_size)){
		send_packet->add(binary.buffer(copy_offset), copy_size);
		return send_packet;
	}

	return BinaryMemoryPtr();
}

} // namepsace route
} // namepsace frrpc



