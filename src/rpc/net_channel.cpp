/**********************************************************
 *  \file net_client.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "net_channel.h"

using namespace std;
using namespace frpublic;
using namespace frnet;
using namespace google::protobuf;

namespace frrpc{
namespace network{

RpcChannel_Server::RpcChannel_Server(const std::string &ip, Port port)// {{{2
	:net_client_(CreateNetClient(this)),
	 ip_(ip),
	 port_(port),
	 net_info_()
{
	net_info_.set_net_type(eNetType_Server);
}// }}}2

RpcChannel_Server::~RpcChannel_Server(){// {{{2
	;
}// }}}2

bool RpcChannel_Server::Start(){// {{{2
	if(!net_client_->Start(ip_.c_str(), port_)){
		RPC_DEBUG_E("Fail to start. ip[" << ip_ << "] port[" << port_ << "]");
		return false;
	}
	return true;
}// }}}2

bool RpcChannel_Server::Stop(){// {{{2
	return net_client_->Stop();
}// }}}2

bool RpcChannel_Server::Disconnect(LinkID link_id){// {{{2
	return false;
}// }}}2

bool RpcChannel_Server::Send(const RpcMeta& meta, const Message& body){// {{{2
	bool ret(false);

	BinaryMemoryPtr binary = BuildBinaryFromMessage(net_info_, meta, body);
	if(binary != NULL){
		ret = net_client_->Send(binary);
		if(!ret){
			RPC_DEBUG_E("Fail to send binary.");
		}
	}
	return ret;
}// }}}2

bool RpcChannel_Server::Send(LinkID link_id, const RpcMeta& meta, const Message& body){// {{{2
	return false;
}// }}}2

bool RpcChannel_Server::Send(const vector<LinkID>& link_ids, const RpcMeta& meta, const Message& body){// {{{2
	return false;
}// }}}2

bool RpcChannel_Server::GetRemoteAddress(LinkID link_id, std::string& ip, Port& port){// {{{2
	ip = ip_;
	port = port_;
	return true;
}// }}}2

bool RpcChannel_Server::OnReceive(Socket socket, const frpublic::BinaryMemory& binary, size_t& read_size){// {{{2
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

			if(net_info.net_type() == net_info_.net_type()){
				PushMessageToQueue(packet);
			}
			else{
				return ReturnError("net type is wrong.");
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

void RpcChannel_Server::OnConnect(Socket socket){// {{{2
}// }}}2

void RpcChannel_Server::OnDisconnect(Socket socket){// {{{2
}// }}}2

void RpcChannel_Server::OnClose(){// {{{2
}// }}}2

void RpcChannel_Server::OnError(const NetError& net_error){// {{{2
	NET_DEBUG_E("Receive error : [" << net_error.err_no << "]");
}//}}}2

bool RpcChannel_Server::ReturnError(const std::string& error_info){// {{{2
	RPC_DEBUG_E(error_info);
	return false;
}// }}}2

bool RpcChannel_Server::IsChannel()const{// {{{2
	return true;
}// }}}2

bool RpcChannel_Server::SendHeart(LinkID link_id){// {{{2
	NetInfo net_info;
	net_info.set_net_type(eNetType_Heart);

	BinaryMemoryPtr binary = BuildBinaryFromMessage(net_info_);
	if(binary != NULL){
		if(!net_client_->Send(binary)){
			RPC_DEBUG_E("Fail to send binary.");
			return false;
		}
	}
	return true;
}// }}}2

} // namespace network
} // namespace frrpc

