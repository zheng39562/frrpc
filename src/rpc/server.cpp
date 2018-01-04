/**********************************************************
 *  \file server.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "server.h"

#include "closure.h"
#include "fr_public/pub_tool.h"
#include "net_server.h"

using namespace std;
using namespace fr_public;
using namespace google::protobuf;
using namespace frrpc::network;

namespace frrpc{

// class Server{{{1
Server::Server()// {{{2
	:rpc_net_(NULL),
	 option_(),
	 name_2service_(),
	 work_threads_(),
	 compress_type_(eCompressType_Not),
	 net_event_cb_()
{ ; }// }}}2

Server::Server(ServerOption& option)// {{{2
	:rpc_net_(NULL),
	 option_(option),
	 name_2service_(),
	 work_threads_(),
	 compress_type_(eCompressType_Not),
	 net_event_cb_()
{ ; }// }}}2

Server::~Server(){ // {{{2
	DELETE_POINT_IF_NOT_NULL(rpc_net_);

	Stop();
}// }}}2

bool Server::AddService(::google::protobuf::Service* service){ // {{{2
	string service_name = service->GetDescriptor()->name();
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

	RpcMeta rpc_meta;
	rpc_meta.set_service_name(service_name);
	rpc_meta.set_method_index(service->GetDescriptor()->FindMethodByName(method_name)->index());
	rpc_meta.mutable_rpc_request_meta()->set_request_id(RPC_REQUEST_ID_NULL);
	rpc_meta.set_compress_type(compress_type_);

	if(!rpc_net_->Send(link_ids, rpc_meta, response)){
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

				Closure* done = frrpc::NewPermanentCallback(this, &Server::ReleaseRpcResource, cntl, rpc_message);
				if(rpc_message == NULL || done == NULL){ 
					init_thread = false; 
					DEBUG_E("Can not create closure.(Or operator of new has exception.)"); 
					return ; 
				}

				queue<RpcPacketPtr> packet_queue;
				while(!IsQuit()){
					rpc_net_->FetchMessageQueue(packet_queue, 2000);

					if(packet_queue.empty()){
						FrSleep(1);
					}

					while(!IsQuit() && !packet_queue.empty()){
						RpcPacketPtr package = packet_queue.front();
						packet_queue.pop();

						cntl->set_link_id(package->link_id);
						cntl->set_net_event(package->net_event);

						if(cntl->net_event() == eNetEvent_Method){
							google::protobuf::Service* cur_service(NULL);
							if(ParseBinary(package, *rpc_message, cur_service)){
								cur_service->CallMethod(rpc_message->method_descriptor, cntl, rpc_message->request, rpc_message->response, done);
							}
							else{
								DEBUG_E("Parse rpc message is failed.");
								continue;
							}
						}
						else{
							if(net_event_cb_ != NULL){
								net_event_cb_(cntl);
							}
						}
					}
				}

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

	if(!rpc_message->IsCompleted()){
		DEBUG_W("Message is not completed. (Mabye recall done->Run())");
		return;
	}

	if(!rpc_net_->Send(cntl->link_id(), rpc_message->rpc_meta, *(rpc_message->response))){
		DEBUG_E("Fail to send message.");
		return;
	}

	cntl->set_link_id(RPC_LINK_ID_NULL);
	rpc_message->Clear();
} // }}}2

const ::google::protobuf::Message* Server::CreateRequest(const ::google::protobuf::MethodDescriptor* method_descriptor, const char* buffer, uint32_t size){ // {{{2
	if(method_descriptor != NULL && buffer != NULL && size > 0){
		Message* request = CreateProtoMessage(method_descriptor->input_type());
		if(request != NULL){
			if(!request->ParseFromString(string(buffer, size))){
				DELETE_POINT_IF_NOT_NULL(request);
			}
		}
		return request;
	}
	return NULL;
}///}}}2

::google::protobuf::Message* Server::CreateResponse(const google::protobuf::MethodDescriptor* method_descriptor){ // {{{2
	if(method_descriptor != NULL){
		return CreateProtoMessage(method_descriptor->output_type());
	}
	return NULL;
} // }}}2

google::protobuf::Service* Server::GetServiceFromName(const std::string& service_name){ // {{{2
	auto name_2service_iter = name_2service_.find(service_name);
	if(name_2service_iter != name_2service_.end()){
		return name_2service_iter->second;
	}
	return NULL;
} // }}}2

bool Server::ParseBinary(const RpcPacketPtr& packet, RpcMessage& rpc_message, google::protobuf::Service* service){ // {{{2
	rpc_message.Clear();

	service = GetServiceFromName(packet->rpc_meta.service_name());
	if(service == NULL){
		DEBUG_E("Can not find service. service name is [" << rpc_message.rpc_meta.service_name() << "]");
		return false;
	}

	rpc_message.rpc_meta = packet->rpc_meta;
	rpc_message.method_descriptor = service->GetDescriptor()->method(rpc_message.rpc_meta.method_index());
	rpc_message.request = CreateRequest(rpc_message.method_descriptor, (const char*)packet->binary->buffer(), packet->binary->size());
	rpc_message.response = CreateResponse(rpc_message.method_descriptor);

	return rpc_message.IsCompleted();
} // }}}2

// }}}1

} // namespace frrpc{


