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

Channel::Channel(const ChannelOption& option)// {{{2
	:rpc_net_(NULL),
	 request_callback_(),
	 default_callback_(),
	 option_(option),
	 init_success_(false)
{ 
	;
}// }}}2

Channel::~Channel(){// {{{2
	DELETE_POINT_IF_NOT_NULL(rpc_net_);
}// }}}2

bool Channel::StartServer(const std::string& ip, Port port){// {{{2
	DELETE_POINT_IF_NOT_NULL(rpc_net_);
	rpc_net_ = new RpcChannel_Server(ip, port);
	init_success_ = rpc_net_ != NULL && rpc_net_->Start();
	return init_success_;
}// }}}2

bool Channel::StartGate(const std::string& ip, Port port){// {{{2
	DELETE_POINT_IF_NOT_NULL(rpc_net_);
	rpc_net_ = new RpcChannel_Server(ip, port);
	init_success_ = rpc_net_ != NULL && rpc_net_->Start();
	return init_success_;
}// }}}2

bool Channel::StartMQ(){// {{{2
	init_success_ = false;
	return init_success_;
}// }}}2

void Channel::Stop(){// {{{2
	init_success_ = false;
	rpc_net_->Stop();
}
// }}}2

void Channel::CallMethod(const MethodDescriptor* method, RpcController* controller, const Message* request, Message* response, Closure* done){// {{{2
	if(!init_success_){ RPC_DEBUG_E("Initialization is failed. Please check and new again."); return; }

	RpcRequestId request_id(++request_id_);
	if(request_callback_.find(request_id) != request_callback_.end()){
		RPC_DEBUG_W("request id is repeated.");
		return ;
	}
	request_callback_.insert(make_pair(request_id, RequestCallBack(done, response)));

	RpcMeta rpc_meta;
	rpc_meta.set_service_name(method->service()->name());
	rpc_meta.set_method_index(method->index());
	rpc_meta.mutable_rpc_request_meta()->set_request_id(request_id);
	rpc_meta.set_compress_type(option_.compress_type);

	if(!rpc_net_->Send(rpc_meta, *request)){
		RPC_DEBUG_E("Fail to send request.");
		return ;
	}
}// }}}2

void Channel::RegisterCallback(const MethodDescriptor* method, google::protobuf::Message* response, Closure* permanet_callback){// {{{2
	if(!init_success_){ RPC_DEBUG_E("Initialization is failed. Please check and new again."); return; }
	if(method == NULL || response == NULL || permanet_callback == NULL){ RPC_DEBUG_E("Not allow parameter is null."); return; }

	string callback_key = method->service()->name() + to_string(method->index());
	if(default_callback_.find(callback_key) != default_callback_.end()){
		if(!ClearCallback(callback_key)){
			RPC_DEBUG_E("service [" << method->service()->name() << "] method [" << method->name() << "] is exist.And fail to clear.");
			return ;
		}
	}
	default_callback_.insert(make_pair(callback_key, RegisterCallBack(permanet_callback, response)));
}// }}}2

void Channel::RunCallback(uint32_t run_cb_times){// {{{2
	if(!init_success_){ RPC_DEBUG_E("Initialization is failed. Please check and try again."); return; }

	queue<RpcPacketPtr> packet_queue;
	rpc_net_->FetchMessageQueue(packet_queue, 2000);
	while(!IsAskedToQuit() && !packet_queue.empty()){
		RpcPacketPtr package = packet_queue.front();
		packet_queue.pop();

		RPC_DEBUG_P("Receive packet.");

		if(IsRequestMode(package)){
			RPC_DEBUG_P("request.");

			auto request_callback_iter = request_callback_.find(package->rpc_meta.rpc_request_meta().request_id());
			if(request_callback_iter == request_callback_.end()){
				RPC_DEBUG_E("Can not find request id [" << package->rpc_meta.rpc_request_meta().request_id() << "]");
				return ;
			}

			request_callback_iter->second.response->ParseFromArray(package->binary->buffer(), package->binary->size());
			request_callback_iter->second.callback->Run();
			request_callback_.erase(request_callback_iter);
		}
		else{
			auto default_callback_iter = default_callback_.find(package->rpc_meta.service_name() + to_string(package->rpc_meta.method_index()));
			if(default_callback_iter != default_callback_.end()){
				RPC_DEBUG_E("Can not find key [" << package->rpc_meta.service_name() << to_string(package->rpc_meta.method_index()) << "]");
				return ;
			}
			
			default_callback_iter->second.response->ParseFromArray(package->binary->buffer(), package->binary->size());
			default_callback_iter->second.callback->Run();
			default_callback_.erase(default_callback_iter);
		}
	}
}// }}}2

void Channel::ClearAllSetting(){// {{{2
	Stop();
	request_callback_.clear();
	default_callback_.clear();
}// }}}2

bool Channel::ClearCallback(const std::string& callback_key){// {{{2
	auto default_callback_iter = default_callback_.find(callback_key);
	if(default_callback_iter != default_callback_.end()){
		DELETE_POINT_IF_NOT_NULL(default_callback_iter->second.response);
		DELETE_POINT_IF_NOT_NULL(default_callback_iter->second.callback);
		default_callback_.erase(default_callback_iter);
		return true;
	}
	return false;
}// }}}2

} // namespace frrpc

