/**********************************************************
 *  \file callback_info.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "callback.h"

/**
 * ChannelCallback
 */
namespace frrpc{

ChannelCallback::ChannelCallback(google::protobuf::Closure* _callback, google::protobuf::RpcController* _cntl, google::protobuf::Message* _response)
	:callback(_callback), cntl(NULL), response(_response), sync_condition(NULL)
{
	cntl = dynamic_cast<frrpc::ChannelController*>(_cntl);
	if(cntl != NULL && cntl->is_sync()){
		sync_condition = new std::condition_variable();
	}
}
ChannelCallback::~ChannelCallback(){
	if(cntl == NULL || !cntl->is_sync()){
		DELETE_POINT_IF_NOT_NULL(cntl); 
		DELETE_POINT_IF_NOT_NULL(response); 
	}
}

void ChannelCallback::Call(){
	if(cntl != NULL && cntl->is_sync()){
		sync_condition->notify_one();
	}
	else{
		if(callback != NULL){
			callback->Run();
		}
	}
}

} // namespace frrpc{

/**
 *
 */
namespace frrpc{

CallbackInfo::CallbackInfo(Channel* _channel, const google::protobuf::MethodDescriptor* _method, google::protobuf::Message* _response)
	:channel_(_channel), service_name_(_method->service()->name()), method_name_(_method->name()), response_(_response)
{}

void CallbackInfo::Release(){
	DELETE_POINT_IF_NOT_NULL(response_);
}

google::protobuf::RpcController* CallbackInfo::createNewCntl(const frrpc::RpcPacketPtr& package){
	frrpc::ChannelController* cntl = new frrpc::ChannelController(channel_, false);
	cntl->SetFailed(package->rpc_meta.rpc_response_meta().error());
	return cntl;
}

google::protobuf::Message* CallbackInfo::createNewResponse(const frrpc::RpcPacketPtr& package){
	google::protobuf::Message* response = response_->New();
	!response->ParseFromArray(package->binary->buffer(), package->binary->size());
	return response;
}

} // namespace frrpc{

