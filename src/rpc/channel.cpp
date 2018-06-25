/**********************************************************
 *  \file channel.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "channel.h"

using namespace std;
using namespace frpublic;
using namespace frrpc::network;
using namespace google::protobuf;

namespace frrpc{

Channel::Channel(const ChannelOption& option)
	:init_success_(false),
	 rpc_net_(NULL),
	 request_callback_(),
	 register_callback_(),
	 option_(option),
	 request_id_(0),
	 net_event_cb_()
{ 
	;
}

Channel::~Channel(){
	Stop();

	for(auto& register_callback_item : register_callback_){
		register_callback_item.second.release();
	}

	for(auto& request_callback_item : request_callback_){
		request_callback_item.second.release();
	}
}

bool Channel::StartServer(const std::string& ip, Port port){
	DELETE_POINT_IF_NOT_NULL(rpc_net_);
	rpc_net_ = new RpcChannel_Server(ip, port);
	init_success_ = rpc_net_ != NULL && rpc_net_->Start();
	return init_success_;
}

bool Channel::StartRoute(const std::string& ip, Port port){
	DELETE_POINT_IF_NOT_NULL(rpc_net_);
	rpc_net_ = new RpcChannel_Route(ip, port);
	init_success_ = rpc_net_ != NULL && rpc_net_->Start();
	return init_success_;
}

bool Channel::StartMQ(){
	init_success_ = false;
	return init_success_;
}

void Channel::Stop(){
	RPC_DEBUG_I("Stop channel.");

	init_success_ = false;
	rpc_net_->Stop();
	DELETE_POINT_IF_NOT_NULL(rpc_net_);
}


void Channel::CallMethod(const MethodDescriptor* method, RpcController* controller, const Message* request, Message* response, Closure* done){
	if(!init_success_){ RPC_DEBUG_E("Initialization is failed. Please check and new again."); return; }

	frrpc::Controller* cntl = dynamic_cast<frrpc::Controller*>(controller);
	if(cntl == NULL){
		RPC_DEBUG_E("Fail to convert cntl point. contrller is wrong."); return ;
	}

	RpcRequestId request_id(++request_id_);
	if(request_callback_.find(request_id) != request_callback_.end()){
		RPC_DEBUG_W("request id is repeated."); return ;
	}
	request_callback_.insert(make_pair(request_id, RequestCallBack(done, response)));

	RpcMeta rpc_meta;
	rpc_meta.set_service_name(method->service()->name());
	rpc_meta.set_method_index(method->index());
	rpc_meta.mutable_rpc_request_meta()->set_request_id(request_id);
	rpc_meta.set_compress_type(option_.compress_type);

	if(!rpc_net_->Send(cntl, rpc_meta, *request)){
		RPC_DEBUG_E("Fail to send request."); return ;
	}
}

void Channel::RegisterCallback(const MethodDescriptor* method, google::protobuf::Message* response, Closure* permanet_callback){
	if(!init_success_){ RPC_DEBUG_E("Initialization is failed. Please check and new again."); return; }
	if(method == NULL || response == NULL || permanet_callback == NULL){ RPC_DEBUG_E("Not allow parameter is null."); return; }

	string callback_key = method->service()->name() + "." + to_string(method->index());
	if(register_callback_.find(callback_key) != register_callback_.end()){
		if(!ClearCallback(callback_key)){
			RPC_DEBUG_E("service [%s] method [%s] is exist.And fail to clear.", method->service()->name().c_str(), method->name().c_str());
			return ;
		}
	}

	RPC_DEBUG_I("Register function. callback_key [%s] listen method [%s.%d] response name [%s] addr [%p]", callback_key.c_str(), method->full_name().c_str(), method->index(), response->GetDescriptor()->full_name().c_str(), response);
	register_callback_.insert(make_pair(callback_key, RegisterCallBack(permanet_callback, response)));
}

void Channel::RunCallback(uint32_t run_cb_times){
	if(!init_success_){ RPC_DEBUG_E("Initialization is failed. Please check and try again."); return; }

	queue<RpcPacketPtr> packet_queue;
	rpc_net_->FetchMessageQueue(packet_queue, 2000);
	while(!IsAskedToQuit() && !packet_queue.empty()){
		RpcPacketPtr package = packet_queue.front();
		packet_queue.pop();

		RPC_DEBUG_P("Receive packet. info : %s", package->info().c_str());

		if(package->net_event == eNetEvent_Method){
			if(IsRequestMode(package)){

				auto request_callback_iter = request_callback_.find(package->rpc_meta.rpc_request_meta().request_id());
				if(request_callback_iter == request_callback_.end()){
					RPC_DEBUG_E("Can not find request id [%d]", package->rpc_meta.rpc_request_meta().request_id());
					return ;
				}

				request_callback_iter->second.response->ParseFromArray(package->binary->buffer(), package->binary->size());
				request_callback_iter->second.callback->Run();
				request_callback_.erase(request_callback_iter);
			}
			else{
				auto default_callback_iter = register_callback_.find(ConvertMethodKey(package->rpc_meta.service_name(), package->rpc_meta.method_index()));
				if(default_callback_iter == register_callback_.end()){
					RPC_DEBUG_E("Can not find key [%s.%d]", package->rpc_meta.service_name().c_str(), package->rpc_meta.method_index());
					return ;
				}

				default_callback_iter->second.response->ParseFromArray(package->binary->buffer(), package->binary->size());
				default_callback_iter->second.callback->Run();

				default_callback_iter->second.response->Clear();
			}
		}
		else{
			if(net_event_cb_ != NULL){
				Controller* cntl = new Controller();
				if(cntl == NULL){
					RPC_DEBUG_E("Fail to create controller."); continue;
				}

				net_event_cb_(package->net_event);
				DELETE_POINT_IF_NOT_NULL(cntl);
			}
			else{
				RPC_DEBUG_P("callbacl of net event is not setted.");
			}
		}
	}
}

void Channel::ClearAllSetting(){
	Stop();
	request_callback_.clear();
	register_callback_.clear();
}

bool Channel::ClearCallback(const std::string& callback_key){
	auto default_callback_iter = register_callback_.find(callback_key);
	if(default_callback_iter != register_callback_.end()){
		default_callback_iter->second.release();
		register_callback_.erase(default_callback_iter);
		return true;
	}
	return false;
}

} // namespace frrpc

