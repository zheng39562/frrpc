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

Controller::Controller()// {{{2
	:net_event_(eNetEvent_Invalid),
	 link_ids_(NULL),
	 service_addr_(NULL)
{
	;
}//}}}2

Controller::~Controller(){// {{{2
	DELETE_POINT_IF_NOT_NULL(link_ids_);
	DELETE_POINT_IF_NOT_NULL(service_addr_);
}//}}}2

void Controller::Reset(){// {{{2
	;
}//}}}2

bool Controller::Failed() const{// {{{2
	return true;
}//}}}2

string Controller::ErrorText() const{// {{{2
	return "";
}//}}}2

void Controller::StartCancel(){// {{{2
	;
}//}}}2

void Controller::SetFailed(const string& reason){// {{{2
	;
}//}}}2

bool Controller::IsCanceled() const{// {{{2
	return false;
}//}}}2

void Controller::NotifyOnCancel(Closure* callback){// {{{2
	;
}//}}}2

void Controller::Clear(){// {{{2
	net_event_ = eNetEvent_Invalid;

	if(link_ids_ != NULL){ link_ids_->clear(); }
	if(service_addr_ != NULL){ service_addr_->clear(); }
}//}}}2

std::string Controller::info()const{//{{{2
	std::string info;
	info += " link_id: ";
	if(link_ids_ == nullptr){
		info += "nullptr.";
	}
	else{
		for(auto link_id : *link_ids_){
			info += std::to_string(link_id) + ",";
		}
	}

	info += " service_addr: [" + service_addr() + "]";
	info += " net_type: [" + std::to_string(net_event()) + "]";

	return info;
}//}}}2

}// namespace frrpc{

