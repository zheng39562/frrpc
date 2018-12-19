/**********************************************************
 *  \file server.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author zheng39562@163.com
**********************************************************/
#include "frpublic/pub_log.h"
#include "frpublic/pub_memory.h"
#include "frpublic/pub_tool.h"
#include "network/frnet_epoll.h"

#include <sys/types.h> 
#include <signal.h> 

using namespace std;
using namespace frnet;
using namespace fr_public;

// Exit function list. {{{1

void AskToQuit();

void SignalHandle(int sign){
	switch(sign){
		case SIGINT: {
			AskToQuit();
			break;
		}
	}
}
void RegisterQuitSignal(){
	sighandler_t ret = signal(SIGINT, SignalHandle);
	if(ret != SIG_DFL || ret != SIG_IGN){
		if(ret == SIG_ERR){
			abort();
		}
	}
	else{
		abort();
	}
}
static bool g_is_quit = false;
static pthread_once_t frrpc_func_s_signal_handle_once = PTHREAD_ONCE_INIT;
void AskToQuit(){
	g_is_quit = true;
}
bool IsAskedToQuit(){
	pthread_once(&frrpc_func_s_signal_handle_once, RegisterQuitSignal);
	return g_is_quit;
}
//}}}1

// TODO:
//  * epoll 监听一个不存在的socket 和 监听一个已经close的socket 会产生什么情况？
//  * call close socket 是触发什么事件?
//  * 主连接断开后，观察其他连接是不是会自动断开?
//
//	* 基础的一个demo：用来测试功能的完备性.
//	* 需要一个用例去测试，多个连接开开关关，会不会真正完全都关闭了——主要关注连接的socket有没有都正常的关闭了。避免长期开启的fd资源耗尽
//		* 单独一个测试用例
//	* 测试40k个链接的可行性
//	* 优化日志
//		* 提供选择方式：可以设置net的日志key 和 日志等级的设置
//		* 在P和D版本内增加更多日志。
//	* 独立network为一个项目
//	* 跑随机数据数天看是否会崩溃或有数据不一致的问题。用家里多核cpu去跑
//	* lockfree的实现(放在网关后面)
//	* 对应的消息文档：可以写到doc里面

// class EchoServer {{{1
class EchoServer : public NetServer_Epoll{
	public:
		EchoServer():NetServer_Epoll(){}
		EchoServer(size_t min_cache_size, size_t max_cache_size, int32_t _max_listen_num):NetServer_Epoll(min_cache_size, max_cache_size, _max_listen_num){ }
		virtual ~EchoServer()=default;
	protected:
		virtual bool OnReceive(Socket sockfd, const fr_public::BinaryMemory& binary, size_t& read_size){
			DEBUG_D("receive [" << string((const char*)binary.buffer(), binary.size()) << "]");
			BinaryMemoryPtr write_binary(new BinaryMemory(binary));
			if(Send(sockfd, write_binary) == eNetSendResult_Ok){;
				read_size = binary.size();
				return true;
			}
			else{
				DEBUG_E("Fail to send response.[" << string((const char*)binary.buffer(), binary.size()) << "]");
			}
			return false;
		}

		virtual void OnClose(){
			DEBUG_D("Stop Server.");
		}

		virtual void OnError(const NetError& error){
			DEBUG_D("Receive error [" << error.err_no << "]");
		}

		virtual bool OnDisconnect(Socket sockfd){
			DEBUG_D("Disconnect socket [" << sockfd << "]");
			return true;
		}
};
//}}}1

int main(int argc, char* argv[]){
	fr_public::SingleLogServer::GetInstance()->InitLog("./log", 10 * 1024);
	fr_public::SingleLogServer::GetInstance()->set_log_level("server", fr_public::eLogLevel_Program);
	fr_public::SingleLogServer::GetInstance()->set_default_log_key("server");

	EchoServer server;
	if(server.Start("127.0.0.1", 12345)){
		while(!IsAskedToQuit()){
			FrSleep(100);
		}
	}
	else{
		DEBUG_E("Fail to start server.");
	}

	cout << "close" << endl;
	return 0;
}

