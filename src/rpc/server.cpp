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

Server::Server(ServerOption& option)// {{{2
	:rpc_net_server_(NULL),
	 option_(option),
	 name_2service_(),
	 work_threads_(),
	 compress_type_(eCompressType_Not),
	 net_event_cb_()
{ ; }// }}}2

Server::~Server(){ // {{{2
	Stop();
}// }}}2

bool Server::AddService(::google::protobuf::Service* service){ // {{{2
	string service_name = service->GetDescriptor()->name();
	if(name_2service_.find(service_name) == name_2service_.end()){
		name_2service_.insert(make_pair(service_name, service));
		return rpc_net_server_->RegisterService(service_name, option_.service_addr);
	}
	return false;
} // }}}2

bool Server::StartServer(const std::string& ip, Port port){// {{{2
	DELETE_POINT_IF_NOT_NULL(rpc_net_server_);
	rpc_net_server_ = new RpcServer_Server(ip, port);
	if(!rpc_net_server_->Start()){
		RPC_DEBUG_E("Fail to start server.");
		return false;
	}

	if(!InitThreads(option_)){
		RPC_DEBUG_E("Call Init is Failed.");
		return false;
	}
	return true;
}// }}}2

bool Server::StartGate(const vector<tuple<std::string, Port> >& gate_list){// {{{2
	DELETE_POINT_IF_NOT_NULL(rpc_net_server_);
	rpc_net_server_ = new RpcServer_Gate(gate_list);
	if(!rpc_net_server_->Start()){
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
	DELETE_POINT_IF_NOT_NULL(rpc_net_server_);

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
	RPC_DEBUG_P("Server Quit : stop thread and clear resource.");
	return Stop();
}// }}}2

bool Server::SendRpcMessage(frrpc::Controller* cntl, const std::string& service_name, const std::string& method_name, const ::google::protobuf::Message& response){ // {{{2
	Service* service = GetServiceFromName(service_name);
	if(service == NULL){ RPC_DEBUG_E("找不到服务[" << service_name << "]是否未加载改服务."); return false; }

	RpcMeta rpc_meta;
	rpc_meta.set_service_name(service_name);
	rpc_meta.set_method_index(service->GetDescriptor()->FindMethodByName(method_name)->index());
	rpc_meta.mutable_rpc_request_meta()->set_request_id(RPC_REQUEST_ID_NULL);
	rpc_meta.set_compress_type(compress_type_);

	if(!rpc_net_server_->Send(cntl, rpc_meta, response)){
		RPC_DEBUG_E("Fail to send message. cntl info [" << cntl->info() << "]");
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
				cntl->set_service_addr(option_.service_addr);

				RpcMessage* rpc_message = new RpcMessage();

				Closure* done = frrpc::NewPermanentCallback(this, &Server::ReleaseRpcResource, cntl, rpc_message);
				if(rpc_message == NULL || done == NULL){ 
					init_thread = false; 
					RPC_DEBUG_E("Can not create closure.(Or operator of new has exception.)"); 
					return ; 
				}

				RPC_DEBUG_I("Rpc work thread running.");

				queue<RpcPacketPtr> packet_queue;
				while(!IsAskedToQuit()){
					rpc_net_server_->FetchMessageQueue(packet_queue, 2000);

					if(packet_queue.empty()){
						FrSleep(1);
						continue;
					}

					while(!IsAskedToQuit() && !packet_queue.empty()){
						RpcPacketPtr package = packet_queue.front();
						packet_queue.pop();

						cntl->set_link(package->link_id);
						cntl->set_net_event(package->net_event);

						if(cntl->net_event() == eNetEvent_Method){
							google::protobuf::Service* cur_service(NULL);
							if(ParseBinary(package, *rpc_message, &cur_service)){
								RPC_DEBUG_P("Call method. packet info : " 
										<< "link_id [" << package->link_id << "]" << "net_event [" << package->net_event << "]"
										<< "name [" << package->rpc_meta.service_name() << "." << package->rpc_meta.method_index() << "]"
										<< "request [" << rpc_message->request->GetDescriptor()->full_name() << "]" << "response [" << rpc_message->response->GetDescriptor()->full_name() << "]");
								cur_service->CallMethod(rpc_message->method_descriptor, cntl, rpc_message->request, rpc_message->response, done);
							}
							else{
								RPC_DEBUG_E("Parse rpc message is failed.");
								continue;
							}
						}
						else{
							RPC_DEBUG_P("Call net event function. cntl info : " << cntl->info());
							if(net_event_cb_ != NULL){
								net_event_cb_(cntl);
							}
						}
					}
				}

				RPC_DEBUG_I("Rpc work thread stop.");

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
	if(rpc_net_server_ == NULL){ RPC_DEBUG_E("link is null."); return; }
	if(rpc_message == NULL){ RPC_DEBUG_E("point of message is null."); return; }
	if(cntl->link_id() == RPC_LINK_ID_NULL){ RPC_DEBUG_E("link is is zero. Can not send data."); return; }

	if(!rpc_message->IsCompleted()){
		RPC_DEBUG_W("Message is not completed. (Mabye recall done->Run())");
		return;
	}

	RPC_DEBUG_P("Send response " << rpc_message->response->GetDescriptor()->full_name() << ". service name [" << rpc_message->rpc_meta.service_name() << "." << rpc_message->rpc_meta.method_index() << "]");
	if(!rpc_net_server_->Send(cntl, rpc_message->rpc_meta, *(rpc_message->response))){
		RPC_DEBUG_E("Fail to send message.");
		return;
	}

	cntl->Clear();
	rpc_message->Clear();
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
			RPC_DEBUG_E("Fail to parse request message. body binary [" << packet->binary->to_hex() << "]");
		}
	}

	return rpc_message.IsCompleted();
} // }}}2

// }}}1

} // namespace frrpc{


