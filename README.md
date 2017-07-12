# gsf

组件式游戏服务器框架

```c++
/*

/
modules
	1. core					核心模块，主要实现event+args的模型和按帧驱动的框架运行时
	2. network
		- acceptor			TCP网络接收器( 服务端
		- connector			TCP网络连接器( 客户端
		- http_acceptor			HTTP网络接收器 (待实现
	3. timer				时钟类业务
	4. log					日志类业务
	5. distributed		
		- node				分布式单点去网络的业务 （待实现
		- coordinate			分布式协调业务
		- election 			分布式选主业务 （待实现
	6. lua_proxy				c++ module 和 lua module 之间的协调， 还有管理lua状态的职责
	7. redis_cache_proxy			灾备
	8. mysql_proxy				mysql代理 （待实现
	9. aoi					aoi服务（待实现

/
feature
	- 模块访问隔离 (面向事件编程
	- 使用event, arg list 粘合不同模块，服务，语言
	- 支持使用lua编写module
	- 支持分布式架构（可依据业务自行组织
	- 跨平台 (linux, windows, apple

/
	- 在框架中所有的数据都应该被切分到不同的module之中，在分布式的情景下
	 访问非本app的module会进行rpc调用，由root节点协调。 建议将频繁访问的数据限制在本app内部或者同个module内部。
	 
		 root 1~N		
	 +---------------------+	//根节点服务器 （主要挂载coordinate module		
	 ^                     ^		
	 |                     |              		
	 |                     |              db 1~N  //数据库缓存服务器 (主要挂载 redis_cache module		
	 |                     |                 ++		
	 |           +-------------------------->++		
	 |                     ^    game 1~N     ++		
	 |                     |       //游戏服务器 （挂载各种游戏逻辑功能模块     		
       +---+                 +---+                		
      login 1~N               gate 1~N		
	 ^ //登陆服务器CDN负载  ^ //网关服务器		
	 |                     |		
	 +---------------------+		
		  client
	
/
依赖
	core                depend c++11, cmake 2.8+
	
	network module      depend libevent 2.0.22+, protobuf3
	lua_proxy module    depend lua5.3+, sol2 2.16+
	timer module        depend c++11
	log module          depeng glog
	mysql_cache module  depend redis 3.0+
	mongo_cache module  

/
License
	MIT
	Copyright (c) 2016 pojol
	
	pojol@outlook.com
*/
```

