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

using namespace std;
using namespace google::protobuf;

namespace frrpc{

Server::Server()
	:net_link_(NULL),
	 option_(),
	 name_2service_(),
	 work_threads_()
{ ; }

Server::Server(ServerOption& option)
	:net_link_(NULL),
	 option_(option),
	 name_2service_(),
	 work_threads_()
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

bool Server::Init(ServerOption& option){  //{{{2
	if(net_link_ != NULL){ delete net_link; net_link = NULL; }

	net_link_ = CreateNetLink(option_.link_type);
	if(net_link_ == NULL){
		DEBUG_E("Can not carete net_link.");
		return false;
	}

	for(int index = 0; index < option.work_thread_num; ++index){
		work_threads_.push_back(thread(
			[&](){
				queue<BinaryMemoryPtr> message_queue;
				net_link_->FetchMessageQueue(message_queue, 2000);

				Service cur_service(NULL);
				RpcMessage rpc_message;
				while(!message_queue.empty()){
					BinaryMemoryPtr binary = message_queue.top();
					message_queue.pop();

					if(!ParseBinary(*binary, rpc_message, cur_service)){
						DEBUG_E("Parse rpc message is failed.");
						continue;
					}

					cur_service->CallMethod(rpc_message.method_descriptor, NULL, rpc_message.request, rpc_message.response, NULL);
				}

				FrSleep(1);
			}
		));
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
	if(net_link != NULL){
		delete net_link; net_link = NULL;
	}

	for(auto& thread_item : work_threads_){
		thread_item.join();
	}
} //}}}2

const ::google::protobuf::Message* Server::CreateRequest(::google::protobuf::MethodDescriptor* method_descriptor, const char* buffer, uint32_t size){ // {{{2
	if(method_descriptor != NULL && buffer != NULL && size > 0){
		Message* request = message_factory_.GetPrototype(method_descriptor->input_type());
		if(request != NULL){
			if(!request.ParseFromString(string(buffer, size))){
				delete request;
				request = NULL;
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

bool Server::ParseBinary(const BinaryMemory& binary, RpcMessage rpc_message, ::google::protobuf::Service* cur_service){ // {{{2
	rpc_message.Clear();

	rpc_message.socket = *(Socket*)binary.buffer();

	uint32_t rpc_meta_head_offset_ = sizeof(Socket);
	uint32_t rpc_meta_body_offset_ = rpc_meta_head_offset_ + sizeof(RpcMetaSize);
	RpcMetaSize rpc_meta_size = BYTE_CONVERT_TO_TYPE_OFFSET(binary.buffer(), RpcMetaSize, rpc_meta_head_offset_);

	RpcMeta rpc_meta;
	if(!rpc_meta.ParseFromString(string((const char*)binary.buffer() + rpc_meta_body_offset_, rpc_meta_size))){
		DEBUG_E("Parse rpc meta is failed. hex data [" << binary.print() << "]");
		return false;
	}

	uint32_t rpc_request_head_offset_ = rpc_meta_body_offset_ + rpc_meta_size;
	uint32_t rpc_request_body_offset_ = rpc_request_head_offset_ + sizeof(RpcRequestSize);
	RpcRequestSize rpc_meta_size = BYTE_CONVERT_TO_TYPE_OFFSET(binary.buffer(), RpcRequestSize, rpc_request_head_offset_);

	if(binary.size() == (rpc_request_body_offset_ + rpc_request_size)){
		DEBUG_E("Binary size is not equal size of rpc.");
		return false;
	}

	cur_service = GetServiceFromName(rpc_meta.service_name);
	if(cur_service == NULL){
		DEBUG_E("Can not find service. service name is [" << rpc_meta.service_name << "]");
		return false;
	}

	rpc_message.method_descriptor = cur_service->method(rpc_meta.method_index);
	rpc_message.request = CreateRequest(rpc_message.method_descriptor, (const char*)binary.buffer() + rpc_meta_body_offset_, rpc_meta_size);
	rpc_message.response = CreateResponse(rpc_message.method_descriptor);

	return rpc_message.IsCompleted();
} // }}}2

} // namespace frrpc{


