### 二进制结构 
	* 文档可能滞后, 所有网络传输的pb结构建议以src/pb目录为准.

	* 传输结构： PackageSize + NetInfoSize + NetInfo + RpcMetaSize + RpcMeta + Body
		* PackageSize(4) 包含整体长度(不包含自身)
		* NetInfoSize(2) 用于确定netinfo的长度
		* NetInfo
			* NetType : 0xF0 ~ 0xFF (详见src/pb/net.proto)
				eNetType_Invalid = 0;
				eNetType_Server = 1;
				eNetType_Route = 2;
				eNetType_MQ = 3;
				eNetType_Special = 0xEF;
				eNetType_RouteCmd = 0xFE;
				eNetType_Heart = 0xFF;
			* 不同的连接模式有自己额外的net info.所以使用额外的二进制net_binary 来保存对应模式下的网络信息.
		* RpcMetaSize(2) 用于确定rpc meta的长度
		* RpcMeta  
		* Body 
