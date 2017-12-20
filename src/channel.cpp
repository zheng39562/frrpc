/**********************************************************
 *  \file channel.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "channel.h"

#include <queue>

namespace frrpc{

Channel::Channel(const std::string& ip, Port port)// {{{2
	:net_link(NULL),
	 request_callback_(),
	 default_callback_(),
	 option_()
{
	init_success_ = Init(ip, port);
}// }}}2

Channel::Channel(const std::string& ip, Port port, const ChannelOption& option)// {{{2
	:net_link(NULL),
	 request_callback_(),
	 default_callback_(),
	 option_(option)
{ 
	init_success_ = Init(ip, port); 
}// }}}2

Channel::~Channel(){
	;
}

void Channel::CallMethod(const MethodDescriptor* method, RpcController* controller, const Message* request, Message* response, Closure* done){// {{{2
	if(!init_success_){ DEBUG_E("Initialization is failed. Please check and new again."); return; }

	RpcRequestId request_id(++request_id_);

	if(request_callback_.find(request_id) != request_callback_.end()){
		DEBUG_W("request id is repeated.");
		return ;
	}
	request_callback_.insert(make_pair(request_id, done));

	RpcMeta rpc_meta;
	rpc_meta.set_service_name(service_name);
	rpc_meta.set_method_index(service->GetDescriptor()->FindMethodByName(method_name)->index());
	rpc_meta.mutable_rpc_request_meta()->set_request_id(request_id);
	rpc_meta.mutable_rpc_request_meta()->set_request_size(request->ByteSize());
	rpc_meta.mutable_rpc_response_meta()->set_response_size(0);
	rpc_meta.set_compress_type(option_.compress_type);

	BinaryMemoryPtr binary = BuildBinaryFromRequest(&rpc_meta, request);
	if(binary == NULL){
		DEBUG_E("Fail to build binary.");
	}

	if(!net_link->Send(0, binary)){
		DEBUG_E("Fail to send request.");
	}
}// }}}2

void Channel::RegisterCallback(const MethodDescriptor* method, google::protobuf::Message* response, Closure* permanet_callback){// {{{2
	if(!init_success_){ DEBUG_E("Initialization is failed. Please check and new again."); return; }
	if(method == NULL || response == NULL || permanet_callback == NULL){ DEBUG_E("Not allow parameter is null."); return; }

	string callback_key = method->service()->name() + to_string(method->index());
	if(default_callback_.find(callback_key) != default_callback_.end()){
		if(!ClearCallback(callback_key)){
			DEBUG_E("service [" << method->service()->name() << "] method [" << method->name() << "] is exist.And fail to clear.");
			return ;
		}
	}
	default_callback_.insert(make_pair(callback_key, RegisterCallBack(permanet_callback, response)));
}// }}}2

void Channel::RunCallback(uint32_t run_cb_times){// {{{2
	if(!init_success_){ DEBUG_E("Initialization is failed. Please check and try again."); return; }

	queue<BinaryMemoryPtr> message_queue;
	if(net_link->FetchMessageQueue(message_queue, run_cb_times)){
		while(!message_queue.empty()){
			BinaryMemoryPtr binary = message_queue.top();
			message_queue.pop();

			if(!ParseBinaryAndCall(binary)){
				;
			}
		}
	}
}// }}}2

bool Channel::Init(const std::string& ip, Port port){// {{{2
	switch(option_.link_type)

	net_link = CreateLink(option_.link_type);
	if(net_link == NULL){
		DEBUG_E("Fail to create net link.");
		return false;
	}

	if(!net_link.Start(ip, port)){
		DEBUG_E("Fail to connect server.");
		return false;
	}

	return true;
}// }}}2

NetLink* Channel::CreateLink(eLinkType link_type){// {{{2
	switch(link_type){
		case eLinkType_Server: return new NetClient();
		case eLinkType_Gate: return new NetClient();
		default : return NULL:
	}
	return NULL;
}// }}}2

BinaryMemoryPtr Channel::BuildBinaryFromRequest(const Message* meta, const Message* request){// {{{2
	if(meta == NULL || request == NULL){
		return BinaryMemoryPtr();
	}

	BinaryMemoryPtr binary(new BinaryMemory());
	if(binary != NULL){
		binary->reserve(meta->ByteSize() + request->ByteSize());
		if(binary->buffer() == NULL){
			DEBUG_E("New memory error.");
			return BinaryMemoryPtr();
		}

		if(!meta->SerializeToArray(binary.buffer(), meta->ByteSize())){
			DEBUG_E("Fail to serialize meta to string.");
			return BinaryMemoryPtr();
		}

		if(!request->SerializeToArray(ADDR_MOVE_BY_BYTE(binary.buffer() + meta->ByteSize()), request->ByteSie())){
			DEBUG_E("Fail to serialize request to string.");
			return BinaryMemoryPtr();
		}
	}

	return binary;
}// }}}2

bool Channel::ParseBinaryAndCall(const BinaryMemoryPtr& binary){
	uint32_t rpc_meta_head_offset_ = sizeof(Socket);
	uint32_t rpc_meta_body_offset_ = rpc_meta_head_offset_ + sizeof(RpcMetaSize);
	RpcMetaSize rpc_meta_size = BYTE_CONVERT_TO_TYPE_OFFSET(binary.buffer(), RpcMetaSize, rpc_meta_head_offset_);

	rpc_message.rpc_meta = new RpcMeta();
	if(rpc_message.rpc_meta != NULL && !rpc_message.rpc_meta->ParseFromString(string((const char*)binary.buffer() + rpc_meta_body_offset_, rpc_meta_size))){
		DEBUG_E("Parse rpc meta is failed. hex data [" << binary.print() << "]");
		return false;
	}
	rpc_message.rpc_meta->mutable_rpc_response_meta()->add_sockets(socket);

	service = GetServiceFromName(rpc_message->rpc_meta->service_name);
	if(service == NULL){
		DEBUG_E("Can not find service. service name is [" << rpc_message->rpc_meta->service_name << "]");
		continue;
	}

	if(binary.size() != (sizeof(Socket) + rpc_meta_size + rpc_message.rpc_meta->rpc_request_meta().request_size())){
		DEBUG_E("Binary size is not equal size of rpc.");
		return false;
	}

	uint32_t rpc_request_body_offset = rpc_meta_body_offset_ + rpc_meta_size;

	rpc_message.method_descriptor = service->method(rpc_message.rpc_meta->method_index);
	rpc_message.request = CreateRequest(rpc_message.method_descriptor, (const char*)binary.buffer() + rpc_request_body_offset, rpc_message.rpc_meta->body_size());
	rpc_message.response = CreateResponse(rpc_message.method_descriptor);

	return true;
} // }}}2

} // namespace frrpc{


