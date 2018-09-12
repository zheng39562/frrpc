/**********************************************************
 *  \file controller.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "controller.h"

using namespace std;
using namespace google::protobuf;
using namespace frrpc;
using namespace frrpc::network;

namespace frrpc{

Controller::Controller()
	:link_ids_(NULL),
	 service_addr_(NULL)
{
	;
}

Controller::~Controller(){
	DELETE_POINT_IF_NOT_NULL(link_ids_);
	DELETE_POINT_IF_NOT_NULL(service_addr_);
}

void Controller::Reset(){
	;
}

bool Controller::Failed() const{
	return true;
}

string Controller::ErrorText() const{
	return "";
}

void Controller::StartCancel(){
	;
}

void Controller::SetFailed(const string& reason){
	;
}

bool Controller::IsCanceled() const{
	return false;
}

void Controller::NotifyOnCancel(Closure* callback){
	;
}

void Controller::Clear(){
	link_id_ = RPC_LINK_ID_NULL;
}

}// namespace frrpc{

