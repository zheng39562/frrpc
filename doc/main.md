## 介绍
	* 一个简易的rpc库。拥有两种使用方式: 直连和网关模式
	  直连模式(Direct Mode): client 和 server 直接连接
	  网关模式(Route Mode): client <-> route <-> server. 该模式必须开启route.
	
	* 使用方式: 详见example

	* 特点:
		* 除传统的request <-> response外，增加了server通知消息的监听回调。
	
### 为何使用双模式(Direct and Route)?
	* 在写另外一个项目的前期调研时，研究过grpc和brpc的使用方式和大致的运行机制. 发现无法满足需求: 
	  1. server通知消息的接收(流方式实际是需要自己解析的) 
	  2. 自主网关(实现要在上层再套一层，还是等于要自己写一个route).
	  基于以上两点的同时，考虑自己写一个rpc会更容易去修改. 且完成一个rpc会理解rpc pb等也有一定好处.
	* Direct Mode 详解(细节和考虑): 详见diect_mode.md
	* Route Mode 详解(细节和考虑): 详见route_mode.md

### 协议结构
	* 见rpc_std.md

## 实现细节

## 设计考虑

## 注意事项
	* 此项目基本是基于游戏的rpc。在后续功能方面，也偏向考虑这个方面。如果是传统的req-res的方式，更加推荐brpc.因为此项目最初目的是个人使用，所以在走向工业级方面的时间是无法确认的。
	* 2018/05/21 此日期而言，此项目依然是一个demo产品。后续必然会完善.
