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

namespace frrpc{

Controller::Controller(){
	;
}

Controller::~Controller(){
	;
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

}// namespace frrpc{

