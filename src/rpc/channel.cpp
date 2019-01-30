/**********************************************************
 *  \file channel.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "channel.h"

#include "rpc/frrpc_log.h"
#include "frpublic/pub_tool.h"

using namespace std;
using namespace frpublic;
using namespace frrpc::network;
using namespace google::protobuf;

//#define MSG_DEBUG_CHANNEL_RSP_ERROR(channel_name, request_id, error) MSG_DEBUG_FORMAT ("opt|%s,request_id|%d,error|%s", channel_name.c_str(), request_id, error)

namespace frrpc{

Channel::Channel(const ChannelOption& _option)
	:init_success_(false),
	 run_thread_(false),
	 option_(_option),
	 rpc_net_(NULL),
	 parse_threads_(),
	 work_threads_(),
	 request_id_(0),
	 mutex_request_map_(),
	 mutex_register_map_(),
	 request_callback_(),
	 register_callback_(),
	 already_callback_queue_(),
	 timeout_events_(),
	 net_event_cb_()
{ 
	;
}

Channel::~Channel(){
	Stop();

	{
		lock_guard<mutex> lock_request(mutex_request_map_);
		for(auto& request_callback_item : request_callback_){
			DELETE_POINT_IF_NOT_NULL(request_callback_item.second);
		}
	}

	{
		lock_guard<mutex> lock_callbak(mutex_register_map_);
		for(auto& register_callback_item : register_callback_){
			register_callback_item.second->Release();
			DELETE_POINT_IF_NOT_NULL(register_callback_item.second);
		}
	}
}

bool Channel::StartRoute(const std::string& ip, Port port){
	DELETE_POINT_IF_NOT_NULL(rpc_net_);
	rpc_net_ = new RpcChannel_Route(ip, port);
	init_success_ = rpc_net_ != NULL && rpc_net_->Start();

	if(init_success_){
		InitThreads();
	}

	return init_success_;
}

bool Channel::StartMQ(){
	init_success_ = false;
	return init_success_;
}

void Channel::Stop(){
	if(init_success_){
		RPC_DEBUG_I("Stop channel.");

		init_success_ = false;

		if(rpc_net_ != NULL){
			rpc_net_->Stop();
			DELETE_POINT_IF_NOT_NULL(rpc_net_);
		}

		run_thread_ = false;

		for(auto& thread_item : parse_threads_){
			if(thread_item.joinable()){
				thread_item.join();
			}
		}
		for(auto& thread_item : work_threads_){
			if(thread_item.joinable()){
				thread_item.join();
			}
		}
	}
}


void Channel::CallMethod(const MethodDescriptor* method, RpcController* cntl, const Message* request, Message* response, Closure* done){
	if(!init_success_){ RPC_DEBUG_E("Initialization is failed. Please check and new again."); return; }
	if(response == NULL){ RPC_DEBUG_E("response is null."); return ; }

	frrpc::ChannelController* channel_cntl = dynamic_cast<frrpc::ChannelController*>(cntl);
	if(channel_cntl == NULL){ RPC_DEBUG_E("channel cntl is null."); return ; }
	if(!channel_cntl->is_sync() && done == NULL){ RPC_DEBUG_E("response or done is null."); return ; }

	RpcRequestId request_id(++request_id_);
	MSG_DEBUG_CHANNEL_REQ(option_.channel_name, method->service()->name(), method->name(), request_id, request);

	channel_cntl->SetRequestId(request_id);

	ChannelCallback* callback = NULL;
	{
		lock_guard<mutex> lock_request(mutex_request_map_);
		if(request_callback_.find(request_id) != request_callback_.end()){
			RPC_DEBUG_W("request id is repeated."); return ;
		}
		callback = new ChannelCallback(done, channel_cntl, response);

		request_callback_.insert(make_pair(request_id, callback));
	}

	RpcMeta rpc_meta;
	rpc_meta.set_service_name(method->service()->name());
	rpc_meta.set_method_index(method->index());
	rpc_meta.mutable_rpc_request_meta()->set_request_id(request_id);
	rpc_meta.set_compress_type(option_.compress_type);

	if(!rpc_net_->Send(rpc_meta, *request)){
		RPC_DEBUG_E("Fail to send request."); return ;
	}

	timeout_events_.push_back(new RequestTimeoutEvent(GetLocalTimeMs() + option_.request_timeout, request_id_));
	if(callback->sync_condition != NULL){
		unique_lock<mutex> condition_mutex_lock(mutex_request_map_);
		callback->sync_condition->wait(condition_mutex_lock);
	}
}

void Channel::RegisterCallback(std::string callback_key, CallbackInfo* callback_info){
	if(!init_success_){ RPC_DEBUG_E("Initialization is failed. Please check and new again."); return; }
	if(callback_info == NULL){ RPC_DEBUG_E(""); return; }
	lock_guard<mutex> lock_callbak(mutex_register_map_);

	if(register_callback_.find(callback_key) != register_callback_.end()){
		if(!DelCallback(callback_key)){
			RPC_DEBUG_E("callback key [%s] is exist.And fail to clear.", callback_key.c_str());
			return ;
		}
	}

	register_callback_.insert(make_pair(callback_key, callback_info));
}

bool Channel::RegisterService(::google::protobuf::Service* service, const std::string& service_addr){
	return rpc_net_->RegisterService(service->GetDescriptor()->name(), service_addr);
}

bool Channel::DelCallback(const std::string& callback_key){
	lock_guard<mutex> lock_callback(mutex_register_map_);

	auto default_callback_iter = register_callback_.find(callback_key);
	if(default_callback_iter != register_callback_.end()){
		default_callback_iter->second->Release();
		DELETE_POINT_IF_NOT_NULL(default_callback_iter->second);

		register_callback_.erase(default_callback_iter);
		return true;
	}
	return false;
}

void Channel::InitThreads(){  
	run_thread_ = true;

	bool init_thread(true);
	for(int index = 0; index < option_.parse_thread_num; ++index){
		parse_threads_.push_back(thread(
		[&](){
			queue<RpcPacketPtr> packet_queue;
			while(run_thread_){
				rpc_net_->FetchMessageQueue(packet_queue, option_.parse_num_once);
				if(packet_queue.empty()){
					FrSleep(option_.thread_sleep_time);
				}

				while(run_thread_ && !packet_queue.empty()){
					RpcPacketPtr package = packet_queue.front();
					packet_queue.pop();

					RPC_DEBUG_P("Receive packet. info : %s", package->info().c_str());

					if(package->net_event == eRpcEvent_Method){
						if(IsRequestMode(package)){
							DoRequestPacket(package);
						}
						else{
							DoRegisterPacket(package);
						}
					}
					else{
						MSG_DEBUG_CHANNEL_EVENT(option_.channel_name, eRpcEvent_Name(package->net_event)); if(net_event_cb_ != NULL){
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

				ClearTimeoutRequest();
			}
		}
		));
	}

	if(!init_thread){
		RPC_DEBUG_E("Fail to create thread.");
		Stop();
	}

	for(int index = 0; index < option_.work_thread_num; ++index){
		work_threads_.push_back(thread(
		[&](){
			while(run_thread_){
				queue<ChannelCallback*> request_queue;
				already_callback_queue_.pop(request_queue, option_.deal_msg_num_once);

				if(!request_queue.empty()){
					while(run_thread_ && !request_queue.empty()){
						ChannelCallback* callback = request_queue.front();
						request_queue.pop();

						callback->Call();
						DELETE_POINT_IF_NOT_NULL(callback);
					}
				}
				else{
					FrSleep(option_.thread_sleep_time);
				}
			}
		}
		));
	}

	if(!init_thread){
		RPC_DEBUG_E("Fail to create thread.");
		Stop();
	}
}

void Channel::DoRequestPacket(const frrpc::RpcPacketPtr& package){
	lock_guard<mutex> lock_request(mutex_request_map_);

	int request_id = package->rpc_meta.rpc_request_meta().request_id();
	auto request_callback_iter = request_callback_.find(request_id);
	if(request_callback_iter == request_callback_.end()){
		RPC_DEBUG_D("request is not found.(maybe : request timeout.)");
		return ;
	}

	ChannelCallback* callback = request_callback_iter->second;
	if(callback == NULL){
		RPC_DEBUG_E("callback is null. request id [%d]", request_callback_iter->first);
		return ;
	}

	callback->cntl->SetFailed(package->rpc_meta.rpc_response_meta().error());
	if(!callback->cntl->Failed()){
		callback->response->ParseFromArray(package->binary->buffer(), package->binary->size());
		MSG_DEBUG_CHANNEL_RSP(option_.channel_name, package->rpc_meta.rpc_request_meta().request_id(), callback->response);
	}
	else{
		MSG_DEBUG_CHANNEL_EVENT(option_.channel_name, callback->cntl->ErrorText());
	}

	already_callback_queue_.push(callback);
	request_callback_.erase(request_callback_iter);
}

void Channel::DoRegisterPacket(const frrpc::RpcPacketPtr& package){
	lock_guard<mutex> lock_callback(mutex_register_map_);

	auto default_callback_iter = register_callback_.find(ConvertMethodKey(package->rpc_meta.service_name(), package->rpc_meta.method_index()));
	if(default_callback_iter == register_callback_.end()){
		RPC_DEBUG_E("Can not find key [%s.%d]", package->rpc_meta.service_name().c_str(), package->rpc_meta.method_index());
		return ;
	}
	CallbackInfo* callback_info = default_callback_iter->second;
	MSG_DEBUG_CHANNEL_NOTIFY(option_.channel_name, callback_info->service_name(), callback_info->method_name(), callback_info->response());

	ChannelCallback* callback = callback_info->BuildCallback(package);
	if(callback == NULL){
		RPC_DEBUG_E("callback is null. service name [%s], method name [%s]", callback_info->service_name().c_str(), callback_info->method_name().c_str());
		return ;
	}

	already_callback_queue_.push(callback);
}

void Channel::ClearTimeoutRequest(){
	lock_guard<mutex> lock_request(mutex_request_map_);

	time_t cur_time = GetLocalTimeMs();
	auto timeout_event_iter = timeout_events_.begin();
	while(timeout_event_iter != timeout_events_.end()){
		RequestTimeoutEvent* timeout_event = *timeout_event_iter;
		if(cur_time < timeout_event->timeout){
			return;
		}

		string error = "request is timeout. request id " + to_string(timeout_event->request_id);
		MSG_DEBUG_CHANNEL_EVENT(option_.channel_name, error);

		auto request_callback_iter = request_callback_.find(timeout_event->request_id);
		if(request_callback_iter != request_callback_.end()){
			ChannelCallback* callback = request_callback_iter->second;
			callback->cntl->SetFailed("request timeout");

			already_callback_queue_.push(callback);
			request_callback_.erase(request_callback_iter);
		}

		timeout_event_iter = timeout_events_.erase(timeout_event_iter);
		DELETE_POINT_IF_NOT_NULL(timeout_event);
	}
}

bool Channel::CancelRequest(RpcRequestId request_id){
	lock_guard<mutex> lock_request(mutex_request_map_);

	if(request_callback_.find(request_id) != request_callback_.end()){
		RPC_DEBUG_I("cancel request %d", request_id);
		request_callback_.erase(request_id);
		return true;
	}
	return false;
}

} // namespace frrpc

