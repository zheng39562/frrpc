## 协议结构
	* Network : PackageSize(4) + Message(?) 
		* 总消息结构不能超过2^32。
	* RPC Net Struct(Channel and Server) : Message : NetType(1) + NetTypeSize(2) + NetInfo(?) + RPC消息结构(?)
		* 每个结构会自动解析netinfo.
	* RPC消息结构：Rpc结构消息流结构：RpcMetaSize(2) + RpcMeta(?) + RequestSize(4) + Request(?)
		* Request的起始由Meta长度界定.

