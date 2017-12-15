/**********************************************************
 *  \file server.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "server.h"

#include "net_client.h"
#include "net_server.h"
#include "closure.h"

using namespace std;
using namespace google::protobuf;

namespace frrpc{

Server::Server()
	:net_link_(NULL),
	 option_(),
	 name_2service_(),
	 work_threads_(),
	 compress_type_(eCompressType_Not)
{ ; }

Server::Server(ServerOption& option)
	:net_link_(NULL),
	 option_(option),
	 name_2service_(),
	 work_threads_(),
	 compress_type_(eCompressType_Not)
{ ; }

Server::~Server(){ 
	Stop();
}

bool Server::AddService(::google::protobuf::Service* service_){ // {{{2
	string service_name = service_->GetDescriptor()->name();
	if(name_2service_.find(service_name) == name_2service_.end()){
		name_2service_.insert(make_pair(service_name, service));
		return true;
	}
	return false;
} // }}}2

bool Server::Start(const string& ip, Port port){ // {{{2
	if(!Init(option_)){
		DEBUG_E("Call Init is Failed.");
		return false;
	}

	if(!net_link->Start(ip, port)){
		DEBUG_E("Fail to start linker.");
	}

	return true;
} // }}}2

bool Server::SendRpcMessage(const vector<Socket>& sockets, const std::string& service_name, const std::string& method_name, const ::google::protobuf::Message& response){ // {{{2
	Service* service = GetServiceFromName(service_name);
	if(service == NULL){ DEBUG_E("找不到服务[" << service_name << "]是否未加载改服务."); return false; }

	RpcMeta rpc_meta = new RpcMeta();
	rpc_meta.set_service_name(service_name);
	rpc_meta.set_method_index(service->GetDescriptor()->FindMethodByName(method_name)->index());
	rpc_meta.mutable_rpc_request_meta()->set_request_id(RPC_REQUEST_ID_NULL);
	rpc_meta.mutable_rpc_request_meta()->set_request_size(0);
	rpc_meta.mutable_rpc_response_meta()->set_response_size(response.ByteSize());
	rpc_meta.set_compress_type(compress_type_);

	for(auto socket : sockets){
		rpc_meta.mutable_rpc_response_meta()->add_sockets(socket);
	}

	BinaryMemoryPtr binaey = BuildBinaryFromResponse(&rpc_meta, &response);
	if(!SendRpcMessage(const RpcMeta* meta, const BinaryMemoryPtr& binary)){
		DEBUG_E("Fail to send message.");
		return false;
	}
	return true;
} // }}}2

bool Server::Init(ServerOption& option){  //{{{2
	DELETE_POINT_IF_NOT_NULL(net_link);

	net_link_ = CreateNetLink(option_.link_type);
	if(net_link_ == NULL){
		DEBUG_E("Can not carete net_link.");
		return false;
	}

	bool init_thread(true);
	for(int index = 0; index < option.work_thread_num; ++index){
		work_threads_.push_back(thread(
			[&](){
				RpcMessage* rpc_message = new RpcMessage();
				Closure* done = NewPermanentCallback(*this, Server::ReleaseRpcResource, rpc_message);
				if(rpc_message == NULL || done == NULL){ 
					init_thread = false; 
					DEBUG_E("Can not create closure.(Or operator of new has exception.)"); 
					return ; 
				}

				queue<BinaryMemoryPtr> message_queue;
				while(!IsQuit()){
					net_link_->FetchMessageQueue(message_queue, 2000);

					if(message_queue.empty()){
						FrSleep(1);
					}

					while(!IsQuit() && !message_queue.empty()){
						BinaryMemoryPtr binary = message_queue.top();
						message_queue.pop();

						Service cur_service(NULL);
						if(!ParseBinary(*binary, *rpc_message, cur_service)){
							DEBUG_E("Parse rpc message is failed.");
							continue;
						}

						cur_service->CallMethod(rpc_message.method_descriptor, NULL, rpc_message.request, rpc_message.response, done);
					}
				}

				message_queue.clear();

				DELETE_POINT_IF_NOT_NULL(rpc_message);
				DELETE_POINT_IF_NOT_NULL(done);
			}
		));
	}

	if(!init_thread){
		DEBUG_E("Fail to create thread.");
		Stop();
		return false;
	}

}//}}}2

NetLink* Server::CreateNetLink(eLinkType link_type){ // {{{2
	switch(link_type){
		case eLinkType_Server:
			return new NetServer();
		case eLinkType_Gate:
			return new NetClient();
		case eLinkType_MQ:
			DEBUG_E("MQ type does not defination.");
			return NULL;
		default:
			DEBUG_W("Receive type does not include [Listen] or [Gate].Check your option. receive_type [" << receive_type << "]");
			return NULL;
	}
	return NULL;
} // }}}2

void Server::Stop(){ // {{{2
	DELETE_POINT_IF_NOT_NULL(net_link);

	for(auto& thread_item : work_threads_){
		thread_item.join();
	}
} //}}}2

void Server::ReleaseRpcResource(RpcMessage* rpc_message){ // {{{2
	if(net_link == NULL){ DEBUG_E("link is null."); return; }
	if(rpc_message == NULL){ DEBUG_E("point of message is null."); return; }

	if(!rpc_message.IsCompleted()){
		DEBUG_W("Message is not completed. (Mabye recall done->Run())");
		return;
	}

	BinaryMemoryPtr binary = BuildBinaryFromResponse(rpc_message->rpc_meta, rpc_message->response);
	if(!SendRpcMessage(rpc_message->rpc_meta, binary)){
		DEBUG_E("Fail to send message.");
		return;
	}

	rpc_message->Clear();
} // }}}2

BinaryMemoryPtr Server::BuildBinaryFromResponse(Message* meta, Message* response){// {{{2
	if(meta == NULL || response == NULL){
		return BinaryMemoryPtr();
	}

	BinaryMemoryPtr binary(new BinaryMemory());
	if(binary != NULL){
		binary->reserve(meta->ByteSize() + response->ByteSize());
		if(binary->buffer() == NULL){
			DEBUG_E("New memory error.");
			return BinaryMemoryPtr();
		}

		if(!meta->SerializeToArray(binary.buffer(), meta->ByteSize())){
			DEBUG_E("Fail to serialize meta to string.");
			return BinaryMemoryPtr();
		}

		if(!response->SerializeToArray(ADDR_MOVE_BY_BYTE(binary.buffer() + meta->ByteSize()), response->ByteSie())){
			DEBUG_E("Fail to serialize response to string.");
			return BinaryMemoryPtr();
		}
	}

	return binary;
}// }}}2

bool Server::SendRpcMessage(RpcMeta meta, const BinaryMemoryPtr& binary){  // {{{2
	if(binary == NULL || binary->buffer() == NULL){ 
		DEBUG_E("Can not send empty data.");
		return false;
	}

	switch(option_.link_type){
		case eLinkType_Server: {
			bool success(true);
			for(int index = 0; index < rpc_message->rpc_meta->rpc_response_meta().sockets_size(); ++index){
				success &= net_link->Send(rpc_message->rpc_meta->rpc_response_meta().sockets(index), binary);
			}
			return success;
		}
		case eLinkType_Gate:
			return net_link->Send(0, binary);
		default: 
			DEBUG_E("Unkown link type [" << option_.link_type << "]");
			return false;
	}

	return false;
} // }}}2

const ::google::protobuf::Message* Server::CreateRequest(::google::protobuf::MethodDescriptor* method_descriptor, const char* buffer, uint32_t size){ // {{{2
	if(method_descriptor != NULL && buffer != NULL && size > 0){
		Message* request = message_factory_.GetPrototype(method_descriptor->input_type());
		if(request != NULL){
			if(!request.ParseFromString(string(buffer, size))){
				DELETE_POINT_IF_NOT_NULL(request);
			}
		}
		return request;
	}
	return NULL;
}///}}}2

::google::protobuf::Message* Server::CreateResponse(::google::protobuf::MethodDescriptor* method_descriptor){ // {{{2
	if(method_descriptor != NULL){
		return message_factory_.GetPrototype(method_descriptor->output_type());
	}
	return NULL;
} // }}}2

const ::google::protobuf::Service* Server::GetServiceFromName(const std::string& service_name){ // {{{2
	auto name_2service_iter = name_2service_.find(service_name);
	if(name_2service_iter != name_2service_.end()){
		return name_2service_iter->second;
	}
	return NULL;
} // }}}2

bool Server::ParseBinary(const BinaryMemory& binary, RpcMessage& rpc_message, ::google::protobuf::Service* cur_service){ // {{{2
	rpc_message.Clear();

	Socket socket = *(Socket*)binary.buffer();
	if(socket == 0){
		DEBUG_E("Unknow socket.");
		return false;
	}

	uint32_t rpc_meta_head_offset_ = sizeof(Socket);
	uint32_t rpc_meta_body_offset_ = rpc_meta_head_offset_ + sizeof(RpcMetaSize);
	RpcMetaSize rpc_meta_size = BYTE_CONVERT_TO_TYPE_OFFSET(binary.buffer(), RpcMetaSize, rpc_meta_head_offset_);

	rpc_message->rpc_meta = new RpcMeta();
	if(rpc_message->rpc_meta != NULL && !rpc_message->rpc_meta->ParseFromString(string((const char*)binary.buffer() + rpc_meta_body_offset_, rpc_meta_size))){
		DEBUG_E("Parse rpc meta is failed. hex data [" << binary.print() << "]");
		return false;
	}
	rpc_message->rpc_meta->mutable_rpc_response_meta()->add_sockets(socket);

	if(binary.size() != (sizeof(Socket) + rpc_meta_size + rpc_message->rpc_meta->rpc_request_meta().request_size())){
		DEBUG_E("Binary size is not equal size of rpc.");
		return false;
	}

	cur_service = GetServiceFromName(rpc_message->rpc_meta->service_name);
	if(cur_service == NULL){
		DEBUG_E("Can not find service. service name is [" << rpc_message->rpc_meta->service_name << "]");
		return false;
	}

	uint32_t rpc_request_body_offset = rpc_meta_body_offset_ + rpc_meta_size;

	rpc_message.method_descriptor = cur_service->method(rpc_message->rpc_meta->method_index);
	rpc_message.request = CreateRequest(rpc_message.method_descriptor, (const char*)binary.buffer() + rpc_request_body_offset, rpc_message->rpc_meta->body_size());
	rpc_message.response = CreateResponse(rpc_message.method_descriptor);

	return rpc_message.IsCompleted();
} // }}}2

} // namespace frrpc{


