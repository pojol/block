# gsf

组件式游戏服务器框架

```c++
/*

modules
	1. core					核心模块，主要实现event+args的模型和按帧驱动的框架运行时
	2. network
		- acceptor			TCP网络接收器( 服务端
		- connector			TCP网络连接器( 客户端
		- http_acceptor			HTTP网络接收器 (待实现
	3. timer				负责时钟类业务
	4. log					负责日志类业务
	5. distributed		
		- node				负责分布式单点去网络的业务 （待实现
		- coordinate			负责分布式协调业务 （待实现
		- election 			负责分布式选主业务 （待实现
	6. lua_proxy				负责c++ module 和 lua module 之间的协调， 还有管理lua状态的职责
	7. redis_cache_proxy	负责灾备

/
feature
	- 模块访问隔离
	- 使用event, arg list 粘合不同模块，服务，语言
	- 支持使用lua编写module
	- 支持分布式架构（可依据业务自行组织
	- 跨平台 (linux, windows, apple

/
框架结构概览

	+---------+
	|   app   |
	+----+----+
	     |
             | 组合
             |
	+----+-------+
	| module 1~N |
	+---+--------+
            |     ╲
       组合  |       ╲  组合
	    |          ╲
	+---+-----+  +-----------+
	|interface|  | logic 1~N |
	+---------+  +-----------+

/
核心接口概览

	app.init_cfg			进程的初始化
	app.regist_module		管理module
	app.run	
	
	进程的运行时
			     execute
	            init     +---------------------------------> +          shut
	  app run   +        |                                   |          +       exit
	  ++-+-+-------------+                                   +------------------->
	   ^ ^ ^    +        |                                   |          +
	   | | |             + <---------------------------------+
	   + + +                                                          unregist
	regist module


/
	SampleModule
		: public gsf::IModule			
		, public gsf::IEvent			
	{
		// 接口类
		// 1. 用于装载逻辑单元、和模块状态控制
		// 2. 用于接收、发送等各种事件的接口申明
	}


/
	module.before_init  // 模块创建
	module.init
	module.execute      // 服务器每帧调用
	module.shut         // 模块销毁
	module.after_shut		


/
	event.listen        // 侦听消息
	event.dispatch      // 发送消息
	event.broadcast     // 广播消息
	
	
/
依赖
	core                depend c++11, cmake 2.8+
	
	network module      depend libevent 2.0.22+, protobuf3
	lua_proxy module    depend lua5.3+, sol2 2.16+
	timer module        depend c++11
	log module          depeng glog
	mysql_cache module  depend redis 3.0+
	mongo_cache module  
	
License
	MIT
	Copyright (c) 2016 pojol
	
	pojol@outlook.com
*/
```

