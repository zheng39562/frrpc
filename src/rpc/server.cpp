/**********************************************************
 *  \file server.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "server.h"

using namespace std;
using namespace frpublic;
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
	Stop();

	for(auto& name_2service_item : name_2service_){
		DELETE_POINT_IF_NOT_NULL(name_2service_item.second);
	}
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
		RPC_DEBUG_E("Fail to start server.");
		return false;
	}

	if(!InitThreads(option_)){
		RPC_DEBUG_E("Call Init is Failed.");
		return false;
	}
	return true;
}// }}}2

bool Server::StartGate(const vector<tuple<const std::string&, Port> >& gate_list){// {{{2
	DELETE_POINT_IF_NOT_NULL(rpc_net_);
	rpc_net_ = new RpcServer_Gate(gate_list);
	if(!rpc_net_->Start()){
		RPC_DEBUG_E("Fail to start server.");
		return false;
	}

	if(!InitThreads(option_)){
		RPC_DEBUG_E("Call Init is Failed.");
		return false;
	}
	return true;
}// }}}2

bool Server::StartMQ(const vector<tuple<const std::string&, Port> >& mq_list){// {{{2
	return false;
}// }}}2

bool Server::Stop(){ // {{{2
	DELETE_POINT_IF_NOT_NULL(rpc_net_);

	for(auto& thread_item : work_threads_){
		if(thread_item.joinable()){
			thread_item.join();
		}
	}
	return true;
} //}}}2

bool Server::RunUntilQuit(){// {{{2
	while(!IsAskedToQuit()){
		FrSleep(1000);
	}
	RPC_DEBUG_P("Unit Quit.");
	return Stop();
}// }}}2

bool Server::SendRpcMessage(const vector<LinkID>& link_ids, const std::string& service_name, const std::string& method_name, const ::google::protobuf::Message& response){ // {{{2
	Service* service = GetServiceFromName(service_name);
	if(service == NULL){ RPC_DEBUG_E("找不到服务[" << service_name << "]是否未加载改服务."); return false; }

	RpcMeta rpc_meta;
	rpc_meta.set_service_name(service_name);
	rpc_meta.set_method_index(service->GetDescriptor()->FindMethodByName(method_name)->index());
	rpc_meta.mutable_rpc_request_meta()->set_request_id(RPC_REQUEST_ID_NULL);
	rpc_meta.set_compress_type(compress_type_);

	if(!rpc_net_->Send(link_ids, rpc_meta, response)){
		RPC_DEBUG_E("Fail to send message.");
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
					RPC_DEBUG_E("Can not create closure.(Or operator of new has exception.)"); 
					return ; 
				}

				queue<RpcPacketPtr> packet_queue;
				while(!IsAskedToQuit()){
					rpc_net_->FetchMessageQueue(packet_queue, 2000);

					if(packet_queue.empty()){
						FrSleep(1);
						continue;
					}

					while(!IsAskedToQuit() && !packet_queue.empty()){
						RpcPacketPtr package = packet_queue.front();
						packet_queue.pop();

						cntl->set_link_id(package->link_id);
						cntl->set_net_event(package->net_event);

						if(cntl->net_event() == eNetEvent_Method){
							google::protobuf::Service* cur_service(NULL);
							if(ParseBinary(package, *rpc_message, &cur_service)){
								cur_service->CallMethod(rpc_message->method_descriptor, cntl, rpc_message->request, rpc_message->response, done);
							}
							else{
								RPC_DEBUG_E("Parse rpc message is failed.");
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
		RPC_DEBUG_E("Fail to create thread.");
		Stop();
		return false;
	}

	return true;
}//}}}2

void Server::ReleaseRpcResource(Controller* cntl, RpcMessage* rpc_message){ // {{{2
	RPC_DEBUG_P("Call ReleaseRpcResource");
	if(rpc_net_ == NULL){ RPC_DEBUG_E("link is null."); return; }
	if(rpc_message == NULL){ RPC_DEBUG_E("point of message is null."); return; }
	if(cntl->link_id() == RPC_LINK_ID_NULL){ RPC_DEBUG_E("link is is zero. Can not send data."); return; }

	if(!rpc_message->IsCompleted()){
		RPC_DEBUG_W("Message is not completed. (Mabye recall done->Run())");
		return;
	}

	RPC_DEBUG_P("Send response.");
	if(!rpc_net_->Send(cntl->link_id(), rpc_message->rpc_meta, *(rpc_message->response))){
		RPC_DEBUG_E("Fail to send message.");
		return;
	}

	cntl->set_link_id(RPC_LINK_ID_NULL);
	rpc_message->Clear();

	RPC_DEBUG_P("End.");
} // }}}2

google::protobuf::Service* Server::GetServiceFromName(const std::string& service_name){ // {{{2
	auto name_2service_iter = name_2service_.find(service_name);
	if(name_2service_iter != name_2service_.end()){
		return name_2service_iter->second;
	}
	return NULL;
} // }}}2

bool Server::ParseBinary(const RpcPacketPtr& packet, RpcMessage& rpc_message, google::protobuf::Service** service){ // {{{2
	if(service == NULL){
		RPC_DEBUG_E("point of service is null.");
		return false;
	}
	rpc_message.Clear();

	google::protobuf::Service* cur_service = GetServiceFromName(packet->rpc_meta.service_name());
	if(cur_service == NULL){
		RPC_DEBUG_E("Can not find service. service name is [" << rpc_message.rpc_meta.service_name() << "]");
		return false;
	}

	rpc_message.rpc_meta = packet->rpc_meta;
	rpc_message.method_descriptor = cur_service->GetDescriptor()->method(rpc_message.rpc_meta.method_index());
	rpc_message.request = cur_service->GetRequestPrototype(rpc_message.method_descriptor).New(); 
	rpc_message.response = cur_service->GetResponsePrototype(rpc_message.method_descriptor).New(); 
	*service = cur_service;

	if(rpc_message.request != NULL){
		if(!rpc_message.request->ParseFromArray(packet->binary->buffer(), packet->binary->size())){
			rpc_message.Clear();
		}
	}

	return rpc_message.IsCompleted();
} // }}}2

// }}}1

} // namespace frrpc{


