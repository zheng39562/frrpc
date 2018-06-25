/**********************************************************
 *  \file net_server_server.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "net_server.h"

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

RpcServer_Server::RpcServer_Server(const std::string &ip, Port port)
	:server_(CreateNetServer(this)),
	 ip_(ip),
	 port_(port),
	 net_info_(),
	 rpc_heart_()
{ 
	net_info_.set_net_type(eNetType_Server); 
}

RpcServer_Server::~RpcServer_Server(){ 
	Stop();
}

bool RpcServer_Server::Start(){
	if(!server_->Start(ip_.c_str(), port_)){
		RPC_DEBUG_E("Fail to start server");
		return false;
	}
	rpc_heart_.RunServer(server_);
	return true;
}

bool RpcServer_Server::Stop(){
	if(server_ != NULL){
		if(!server_->Stop()){
			RPC_DEBUG_E("Fail to stop server.");
			return false;
		}
		DELETE_POINT_IF_NOT_NULL(server_);
	}
	rpc_heart_.StopHeartCheck();

	RPC_DEBUG_I("stop server.");
	return true;
}

bool RpcServer_Server::Disconnect(LinkID link_id){
	return server_->Disconnect(BuildLinkID(link_id));
}

bool RpcServer_Server::Send(Controller* cntl, const RpcMeta& meta, const google::protobuf::Message& body){
	bool ret(false);

	BinaryMemoryPtr binary = BuildBinaryFromMessage(net_info_, meta, body);
	if(binary != NULL){
		ret = true;
		for(int index = 0; index < cntl->link_size(); ++index){
			RPC_DEBUG_P("link id[%d] service[%s.%d] binary [%s]", cntl->link_id(index), meta.service_name().c_str(), meta.method_index(), binary->to_hex().c_str());
			if(server_->Send(GetSocket(cntl->link_id(index)), binary) != eNetSendResult_Ok){
				RPC_DEBUG_E("Fail to send body. Detail : link id [%d] binary [%s]", cntl->link_id(index), binary->to_hex().c_str());
				ret = false;
				continue;
			}
		}
	}
	else{
		RPC_DEBUG_E("Fail to build binary(bianry is null).");
	}
	return ret;
}

bool RpcServer_Server::RegisterService(const std::string& service_name, const std::string& service_addr){
	return true;
}

bool RpcServer_Server::OnReceive(Socket socket, const frpublic::BinaryMemory& binary, size_t& read_size){
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
}

void RpcServer_Server::OnConnect(Socket socket){
	PushMessageToQueue(RpcPacketPtr(new RpcPacket(BuildLinkID(socket), eNetEvent_Connection)));
	rpc_heart_.AddSocket(socket);
}

void RpcServer_Server::OnDisconnect(Socket socket){
	PushMessageToQueue(RpcPacketPtr(new RpcPacket(BuildLinkID(socket), eNetEvent_Disconnection)));
	rpc_heart_.DelSocket(socket);
}

void RpcServer_Server::OnClose(){
	NET_DEBUG_D("close net server.");
}

void RpcServer_Server::OnError(const NetError& net_error){
	NET_DEBUG_D("Receive error [%d]", net_error.err_no);
}

bool RpcServer_Server::ReturnError(Socket socket, const std::string& error_info){
	RPC_DEBUG_E("%s Disconnect socket [%d]", error_info.c_str(), socket);
	Disconnect(BuildLinkID(socket));
	return false;
}

}
}

