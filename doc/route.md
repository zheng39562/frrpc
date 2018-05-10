## RouteNetInfo 不一致的问题 
	* RouteNetInfoChannel, RouteNetInfoServer : 分两个的原因
		* 发送的东西是不一样的.
		* server存在群发的问题. 当群发量起来以后 target socket 可能会耗费非常多流量.

