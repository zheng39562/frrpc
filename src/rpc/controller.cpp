/**********************************************************
 *  \file controller.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "controller.h"

#include "channel.h"

using namespace std;
using namespace google::protobuf;
using namespace frrpc;
using namespace frrpc::network;

namespace frrpc{

Controller::Controller():error(){}
Controller::~Controller(){ ; }

void Controller::Reset(){
	error.clear();
}

bool Controller::Failed() const{
	return !error.empty();
}

string Controller::ErrorText() const{
	return error;
}

void Controller::StartCancel(){
	// 实现大致思路是潜入channel，然后发送取消。
	// 会涉及到requestmap 注意加锁.
	throw logic_error("function(StartCancel) does not implement.");
}

void Controller::SetFailed(const string& reason){
	error = reason;
}

bool Controller::IsCanceled() const{ throw logic_error("function(IsCanceled)  does not implement."); return false; }
void Controller::NotifyOnCancel(google::protobuf::Closure* callback){ throw logic_error("function(NotifyOnCancel) does not implement."); }

} // namespace frrpc

// ChannelController
namespace frrpc{

ChannelController::ChannelController(Channel* _channel, bool _is_sync)
	:is_sync_(_is_sync), is_cancel_(false), request_id_(RPC_REQUEST_ID_NULL), channel_(_channel), callback_(NULL)
{;}
ChannelController::~ChannelController(){ ; }

void ChannelController::Reset(){
	throw logic_error("function(Reset) does not implement.");
}

void ChannelController::StartCancel(){
	if(channel_ == NULL){
		throw logic_error("channel is null. can not cancel request.");
	}

	is_cancel_ = true;
	if(channel_->CancelRequest(request_id_) && request_id_ != RPC_REQUEST_ID_NULL){;
		if(callback_ != NULL){
			callback_->Run();
		}

		request_id_ = RPC_REQUEST_ID_NULL;
	}
	else{
		RPC_DEBUG_W("Fail to cancel request.");
	}
}

bool ChannelController::IsCanceled() const{
	return is_cancel_;
}

void ChannelController::NotifyOnCancel(google::protobuf::Closure* callback){
	callback_ = callback; 
}

} // namespace frrpc

// ServerController
namespace frrpc{

ServerController::ServerController():link_id_(RPC_LINK_ID_NULL){ ; }
ServerController::~ServerController(){ ; }

void ServerController::Reset(){
	Controller::Reset();

	link_id_ = RPC_LINK_ID_NULL;
}

} // namespace frrpc

