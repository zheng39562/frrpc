## 协议结构
### 二进制结构
	* 结构： PackageSize + NetInfoSize + NetInfo + RpcMetaSize + RpcMeta + Body
		* PackageSize(4) 包含整体长度(不包含自身)
		* NetInfoSize(2) 用于确定netinfo的长度
		* NetInfo
		* RpcMetaSize(2) 用于确定rpc meta的长度
		* RpcMeta  
		* Body 
	
	* NetInfo
		* NetType : 0xF0 ~ 0xFF 作为通用的特殊标识预留。具体含义如下(如无指定，则作为未来的预留)
			* 0xFF 心跳包 : 客户端(Channel) 主动发送。服务端做回应(Server)
		* 当前使用的消息号.(详见src/pb/net.proto)
			eNetType_Invalid = 0;
			eNetType_Server = 1;
			eNetType_Route = 2;
			eNetType_MQ = 3;
			eNetType_Special = 0xEF;
			eNetType_RouteCmd = 0xFE;
			eNetType_Heart = 0xFF;
	* RpcMeta
	* Body(Response)

### protobuf中各组件的意义和理解
	* 因route 和 本身理解误差 可能和最初pb的设计有些许不同.

#### Channel

#### Server

#### Controller

