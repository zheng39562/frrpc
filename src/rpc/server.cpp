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

// class Server{{{1
Server::Server()
	:rpc_net_(new NetServer()),
	 option_(),
	 name_2service_(),
	 work_threads_(),
	 compress_type_(eCompressType_Not)
{ ; }

Server::Server(ServerOption& option)
	:rpc_net_(new NetServer()),
	 option_(option),
	 name_2service_(),
	 work_threads_(),
	 compress_type_(eCompressType_Not)
{ ; }

Server::~Server(){ 
	DELETE_POINT_IF_NOT_NULL(rpc_net_);

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

bool Server::StartServer(const std::string& ip, Port port){// {{{2
	DELETE_POINT_IF_NOT_NULL(rpc_net_);
	rpc_net_ = new RpcServer_Server(ip, port);
	if(!rpc_net_->Start()){
		DEBUG_E("Fail to start server.");
		return false;
	}

	if(!InitThreads(option_)){
		DEBUG_E("Call Init is Failed.");
		return false;
	}
	return true;
}// }}}2

bool Server::StartGate(const vector<tuple<const std::string&, Port> >& gate_list){// {{{2
	DELETE_POINT_IF_NOT_NULL(rpc_net_);
	rpc_net_ = new RpcServer_Gate(gate_list);
	if(!rpc_net_->Start()){
		DEBUG_E("Fail to start server.");
		return false;
	}

	if(!InitThreads(option_)){
		DEBUG_E("Call Init is Failed.");
		return false;
	}
	return true;
}// }}}2

bool Server::StartMQ(const vector<tuple<const std::string&, Port> >& mq_list){// {{{2
	return false;
}// }}}2

void Server::Stop(){ // {{{2
	DELETE_POINT_IF_NOT_NULL(rpc_net_);

	for(auto& thread_item : work_threads_){
		thread_item.join();
	}
} //}}}2

bool Server::SendRpcMessage(const vector<LinkID>& link_ids, const std::string& service_name, const std::string& method_name, const ::google::protobuf::Message& response){ // {{{2
	Service* service = GetServiceFromName(service_name);
	if(service == NULL){ DEBUG_E("找不到服务[" << service_name << "]是否未加载改服务."); return false; }

	RpcMeta rpc_meta = new RpcMeta();
	rpc_meta.set_service_name(service_name);
	rpc_meta.set_method_index(service->GetDescriptor()->FindMethodByName(method_name)->index());
	rpc_meta.mutable_rpc_request_meta()->set_request_id(RPC_REQUEST_ID_NULL);
	rpc_meta.set_compress_type(compress_type_);

	if(!rpc_net_->Send(link_ids, rpc_meta, response){
		DEBUG_E("Fail to send message.");
		return false;
	}

	return true;
} // }}}2

bool Server::InitThreads(ServerOption& option){  //{{{2
	bool init_thread(true);
	for(int index = 0; index < option.work_thread_num; ++index){
		work_threads_.push_back(thread(
			[&](){
				Controller* cntl = new Controller();

				RpcMessage* rpc_message = new RpcMessage();
				Closure* done = new NewPermanentCallback(this, Server::ReleaseRpcResource, cntl, rpc_message);
				if(rpc_message == NULL || done == NULL){ 
					init_thread = false; 
					DEBUG_E("Can not create closure.(Or operator of new has exception.)"); 
					return ; 
				}

				queue<RpcServerPacket> packet_queue;
				while(!IsQuit()){
					rpc_net_->FetchMessageQueue(packet_queue, 2000);

					if(packet_queue.empty()){
						FrSleep(1);
					}

					while(!IsQuit() && !packet_queue.empty()){
						RpcServerPacketPtr package = packet_queue.top();
						packet_queue.pop();

						google::protobuf::Service* cur_service(NULL);
						if(ParseBinary(package, *rpc_message, cur_service)){
							cntl->set_link_id(package->link_id);
							cur_service->CallMethod(rpc_message.method_descriptor, cntl, rpc_message.request, rpc_message.response, done);
						}
						else{
							DEBUG_E("Parse rpc message is failed.");
							continue;
						}
					}
				}

				packet_queue.clear();

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

void Server::ReleaseRpcResource(Controller* cntl, RpcMessage* rpc_message){ // {{{2
	if(rpc_net_ == NULL){ DEBUG_E("link is null."); return; }
	if(rpc_message == NULL){ DEBUG_E("point of message is null."); return; }
	if(cntl->link_id() != RPC_LINK_ID_NULL){ DEBUG_E("link is is zero. Can not send data."); return; }

	if(!rpc_message.IsCompleted()){
		DEBUG_W("Message is not completed. (Mabye recall done->Run())");
		return;
	}

	if(!rpc_net_->Send(cntl->link_id(), rpc_message->rpc_meta, rpc_message->response)){
		DEBUG_E("Fail to send message.");
		return;
	}

	cntl->set_link_id(RPC_LINK_ID_NULL);
	rpc_message->Clear();
} // }}}2

const ::google::protobuf::Message* Server::CreateRequest(::google::protobuf::MethodDescriptor* method_descriptor, const char* buffer, uint32_t size){ // {{{2
	if(method_descriptor != NULL && buffer != NULL && size > 0){
		Message* request = CreateProtoMessage(method_descriptor->input_type());
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
		return CreateProtoMessage(method_descriptor->output_type());
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

bool Server::ParseBinary(const RpcServerPacketPtr& package, RpcMessage& rpc_message, google::protobuf::Service* service){ // {{{2
	rpc_message.Clear();

	service = GetServiceFromName(package->rpc_meta.service_name);
	if(service == NULL){
		DEBUG_E("Can not find service. service name is [" << rpc_message->rpc_meta->service_name << "]");
		continue;
	}

	rpc_message.request_id = package->rpc_meta.rpc_request_meta().request_id();
	rpc_message.method_descriptor = service->method(package->rpc_meta.method_index());
	rpc_message.request = CreateRequest(rpc_message.method_descriptor, (const char*)binary.buffer(), binary.size());
	rpc_message.response = CreateResponse(rpc_message.method_descriptor);

	return rpc_message.IsCompleted();
} // }}}2

// }}}1

} // namespace frrpc{


