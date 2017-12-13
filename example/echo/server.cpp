/**********************************************************
 *  \file server.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/

#include "frrpc/frrpc_function.h"
#include "frrpc/server.h"

class EchoServiceImpl : public example::EchoService{
	public:
		//...
};

int main(int argc, char* argv[]){
	frrpc::Server server;

	EchoServiceImpl service = new EchoServiceImpl();

	server.AddService(service);
	
	string ip("0.0.0.0");
	Port port(12345);
	if(!server.Start(ip, port)) {
		return 1;
	}

	while(frrpc::RunUntilStop()){
		;
	}

	delete service; 
	service = NULL;

	return 0;
}

