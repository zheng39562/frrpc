/**********************************************************
 *  \file net_channel_route.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "net_channel.h"

#include "pb/route.pb.h"
#include "public/rpc_serializable.h"

using namespace std;
using namespace frpublic;
using namespace frnet;
using namespace google::protobuf;


namespace frrpc{
namespace network{

RpcChannel_Route::RpcChannel_Route(const std::string &ip, Port port)
	:net_client_(CreateNetClient(this)),
	 ip_(ip),
	 port_(port),
	 rpc_heart_()
{
}

RpcChannel_Route::~RpcChannel_Route(){
	DELETE_POINT_IF_NOT_NULL(net_client_);
}

bool RpcChannel_Route::Start(){
	if(!net_client_->Start(ip_.c_str(), port_)){
		RPC_DEBUG_E("Fail to start. ip[%s:%d", ip_.c_str(), port_);
		return false;
	}
	rpc_heart_.RunClient(net_client_);
	return true;
}

bool RpcChannel_Route::Stop(){
	rpc_heart_.StopHeartCheck();
	return net_client_->Stop();
}

bool RpcChannel_Route::Disconnect(LinkID link_id){
	PushMessageToQueue(RpcPacketPtr(new RpcPacket(link_id, eNetEvent_Disconnection)));
	return true;
}

bool RpcChannel_Route::Send(Controller* cntl, const RpcMeta& meta, const google::protobuf::Message& body){
	frrpc::network::NetInfo net_info;
	net_info.set_net_type(eNetType_Route);

	frrpc::route::RouteNetInfo route_net_info;
	route_net_info.set_is_channel_packet(true);
	route_net_info.set_service_name(meta.service_name());
	route_net_info.set_service_addr(cntl->service_addr());
	if(!route_net_info.SerializeToString(net_info.mutable_net_binary())){
		RPC_DEBUG_E("Fail to serialize route net info."); return false;
	}

	if(net_client_->Send(BuildBinaryFromMessage(net_info, meta, body)) != eNetSendResult_Ok){
		RPC_DEBUG_E("Fail to send binary."); return false;
	}

	return true;
}

bool RpcChannel_Route::OnReceive(Socket socket, const frpublic::BinaryMemory& binary, size_t& read_size){
	int32_t offset(0);
	while((binary.size() - read_size) > sizeof(PacketSize)){
		PacketSize size(0);
		if(!GetAndCheckPacketSize(binary.buffer(offset), size)){ return false; }

		if(size + sizeof(size) > (binary.size() - read_size)){ 
			return true; 
		}

		NetInfo net_info;
		RpcPacketPtr packet(new RpcPacket(0, eNetEvent_Method));
		if(packet != NULL){
			if(!GetMessageFromBinary(binary, offset, net_info, packet)){
				RPC_DEBUG_E("Error : GetMessageFromBinary."); return false;
			}

			switch(net_info.net_type()){
				case eNetType_Route: PushMessageToQueue(packet); break;
				case eNetType_Heart: break;
				case eNetType_RouteCmd: break;
				default : RPC_DEBUG_E("net type[%d] cat not to handle.", net_info.net_type()); return false;
			}
		}
		else{
			RPC_DEBUG_E("Fail to new packet."); return false;
		}

		read_size += size + sizeof(size);
		offset += size + sizeof(size);
	}
	return true;
}

void RpcChannel_Route::OnConnect(Socket socket){
}

void RpcChannel_Route::OnDisconnect(Socket socket){
}

void RpcChannel_Route::OnClose(){
}

void RpcChannel_Route::OnError(const NetError& net_error){
	RPC_DEBUG_E("Receive error : [%d]", net_error.err_no);
}

bool RpcChannel_Route::IsChannel()const{
	return true;
}

bool RpcChannel_Route::SendHeart(LinkID link_id){
	NetInfo net_info;
	net_info.set_net_type(eNetType_Heart);

	BinaryMemoryPtr binary = BuildBinaryFromMessage(net_info);
	if(binary != NULL){
		if(net_client_->Send(binary) != eNetSendResult_Ok){
			RPC_DEBUG_E("Fail to send binary.");
			return false;
		}
	}
	return true;
}

} // namespace network
} // namespace frrpc




