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
using namespace fr_public;
using namespace google::protobuf;

namespace frrpc{
namespace network{

RpcChannel_Server::RpcChannel_Server(const std::string &ip, Port port)// {{{2
	:net_client_(this),
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
		ret = net_client_->Send((const Byte*)binary->buffer(), binary->size());
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

EnHandleResult RpcChannel_Server::OnConnect(ITcpClient* pSender, Socket socket){// {{{2
	return HR_OK;
}// }}}2

EnHandleResult RpcChannel_Server::OnSend(ITcpClient* pSender, Socket socket, const BYTE* pData, int iLength){// {{{2
	return HR_OK;
}// }}}2

EnHandleResult RpcChannel_Server::OnReceive(ITcpClient* pSender, Socket socket, int iLength){// {{{2
	ITcpPullClient* client = ITcpPullClient::FromS(pSender);

	if(client != NULL){
		uint32_t size(0); 
		while(iLength > 0 && iLength >= sizeof(uint32_t) && FR_OK == client->Peek((Byte*)&size, sizeof(size))){
			if((size + sizeof(size)) > (uint32_t)iLength){ break; }

			if(size >= NET_PACKET_MAX_SIZE){ return ReturnError("net_type_size is bigger than buffer. Please reset buffer size(recompile)."); }
			if(size == 0){ return ReturnError("size is zero."); }
			if(FR_OK != client->Fetch((Byte*)&size, sizeof(size))){ return ReturnError("Fail to fetch head."); }

			BinaryMemory binary;
			binary.reserve(size);
			if(FR_OK != client->Fetch((Byte*)binary.CopyMemoryFromOut(size), size)){ 
				return ReturnError("Fail to fetch body."); 
			}

			iLength -= (size + sizeof(size));

			NetInfo net_info;
			RpcPacketPtr packet(new RpcPacket(0, eNetEvent_Method));
			if(packet != NULL){
				if(!GetMessageFromBinary(binary, net_info, packet)){
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
				return HR_ERROR;
			}
		}
	}
	return HR_OK;
}// }}}2

EnHandleResult RpcChannel_Server::OnClose(ITcpClient* pSender, Socket socket, EnSocketOperation enOperation, int iErrorCode){// {{{2
	return HR_OK;
}// }}}2

EnHandleResult RpcChannel_Server::ReturnError(const std::string& error_info){// {{{2
	RPC_DEBUG_E(error_info);
	Stop();
}// }}}2

bool RpcChannel_Server::IsChannel()const{// {{{2
	return true;
}// }}}2

bool RpcChannel_Server::SendHeart(LinkID link_id){// {{{2
	NetInfo net_info;
	net_info.set_net_type(eNetType_Heart);

	BinaryMemoryPtr binary = BuildBinaryFromMessage(net_info_);
	if(binary != NULL){
		if(!net_client_->Send((const Byte*)binary->buffer(), binary->size())){
			RPC_DEBUG_E("Fail to send binary.");
			return false;
		}
	}
	return true;
}// }}}2

} // namespace network
} // namespace frrpc

