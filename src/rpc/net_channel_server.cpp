/**********************************************************
 *  \file net_channel_server.cpp
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

RpcChannel_Server::RpcChannel_Server(const std::string &ip, Port port)
	:net_client_(CreateNetClient(this)),
	 ip_(ip),
	 port_(port),
	 rpc_heart_()
{
}

RpcChannel_Server::~RpcChannel_Server(){
	DELETE_POINT_IF_NOT_NULL(net_client_);
}

bool RpcChannel_Server::Start(){
	if(!net_client_->Start(ip_.c_str(), port_)){
		RPC_DEBUG_E("Fail to start. ip[%s:%d]", ip_.c_str(), port_);
		return false;
	}
	rpc_heart_.RunClient(net_client_);
	return true;
}

bool RpcChannel_Server::Stop(){
	rpc_heart_.StopHeartCheck();
	return net_client_->Stop();
}

bool RpcChannel_Server::Disconnect(LinkID link_id){
	return false;
}

bool RpcChannel_Server::Send(Controller* cntl, const RpcMeta& meta, const google::protobuf::Message& body){
	NetInfo net_info;
	net_info.set_net_type(eNetType_Server);
	BinaryMemoryPtr binary = BuildBinaryFromMessage(net_info, meta, body);
	if(binary != NULL){
		if(net_client_->Send(binary) != eNetSendResult_Ok){
			RPC_DEBUG_E("Fail to send binary.");
			return false;
		}
	}
	return true;
}

bool RpcChannel_Server::GetRemoteAddress(LinkID link_id, std::string& ip, Port& port){
	ip = ip_;
	port = port_;
	return true;
}

bool RpcChannel_Server::OnReceive(Socket socket, const frpublic::BinaryMemory& binary, size_t& read_size){
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
			if(!GetMessageFromBinary(binary, offset, net_info, packet)){
				return ReturnError("Error : GetMessageFromBinary.");
			}

			switch(net_info.net_type()){
				case eNetType_Server: 
					PushMessageToQueue(packet); 
					break;
				case eNetType_Heart: RPC_DEBUG_D("Receive heart response."); break;
				default: RPC_DEBUG_E("net type[%d] cat not to handle.", net_info.net_type()); return false;
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

void RpcChannel_Server::OnConnect(Socket socket){
}

void RpcChannel_Server::OnDisconnect(Socket socket){
	PushMessageToQueue(RpcPacketPtr(new RpcPacket(socket, eNetEvent_Disconnection)));
}

void RpcChannel_Server::OnClose(){
}

void RpcChannel_Server::OnError(const NetError& net_error){
	NET_DEBUG_E("Receive error : [%d]", net_error.err_no);
}

bool RpcChannel_Server::ReturnError(const std::string& error_info){
	RPC_DEBUG_E("%s", error_info.c_str());
	return false;
}

} // namespace network
} // namespace frrpc



