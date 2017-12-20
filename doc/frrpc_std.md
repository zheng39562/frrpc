## 协议结构
	* 网络消息结构：总包头(4) + LinkType(1) + LinkTypeSize(2) + LinkInfo(?) + RpcMetaSize(2) + RpcMeta(?) + RequestSize(4) + Request(?)
		* 总消息结构不能超过2^32。
		* Request的起始由Meta长度界定.
		* LinkType 和 LinkTypeHead 必然存在，但LinkInfo可能为0字节.
	* Server端的消息结构：Rpc结构消息流结构：Socket(4) + RpcMeta包头(2) + RpcMeta消息(?) + Request包头(4) + Request消息(?)
		* 主要用以兼容网关和监听两种模式。
		  网关模式: 网关会将对应用户的socket打包发给对应server。
		  监听模式: 在监听回调时，将socket填充到结构中.
	
