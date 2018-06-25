## 进度
	* 已可以进行基本的通讯，打磨细节中(bug,代码和使用).

## 介绍
	* 这是一个和常规rpc不太一样的rpc库。
	  它的立意最初就是仅用于游戏的rpc。故，后续扩展也只会向这方面去考虑.
	  这是一个强调易用性的rpc简易框架. 目标是使用protobuf service简介的定义风格.来接收群发式消息.
	  这是一个会有严重偏向的rpc.原则上不会考虑非游戏领域的通用性.
	* 为何不使用已有的rpc.
	  因为没有找到. :(
	  看过grpc和brpc后，发现在游戏场景下，常规的req-res的方式的rpc并不适用或使用场景严重受限。
	  (考虑过streaming方案，但这就和自己写没区别了.)
	* 包含一个网关
		* 游戏内挂一个gate是非常正常的事情。
		  并且考虑到网关很可能需要了解协议结构，所以干脆提供一个简单的网关。
		* 网关是可选的，并非必须. 提供了直连 和 网关两种模式.

## 使用
	* 详见: example.
	
## 更多
	* 更多细节见 doc.

## TODO
	* example 用例增加更多注释，辅助理解.
	* 支持channel同步功能
	* 研究和理解 lock free(可参考brpc). 考虑是否可以减少锁开销.
		* 慎重替换.
	* (低优先级)消息针对某个id进行序列化处理，保证该key操作一定是序列的.
		* key 可以多层化
		* 此设计未必要做(不确定这个的集成是否合适).
		* 思考来源：如果可以根据地图，用户进行加锁，应用层可以在很多地方不需要再去考虑内存访问冲突的问题.

