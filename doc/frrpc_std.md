## 协议结构
	* Network : PackageSize(4) + Message(?) 
		* 总消息结构不能超过2^32。
	* RPC Net Struct(Channel and Server) : Message : NetTypeSize(2) + NetInfo(?) + RPC消息结构(?)
		* 每个结构会自动解析netinfo.
		* NetType : 0xF0 ~ 0xFF 作为通用的特殊标识预留。具体含义如下(如无指定，则作为未来的预留)
			* 0xFF 心跳包 : 客户端(Channel) 主动发送。服务端做回应(Server)
	* RPC消息结构：Rpc结构消息流结构：RpcMetaSize(2) + RpcMeta(?) + Request(?)
		* Request的起始由Meta长度界定.

