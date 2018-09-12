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
	:link_id_(RPC_LINK_ID_NULL)
{
	;
}

Controller::~Controller(){
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

