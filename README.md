# gsf

组件式游戏服务器框架

```c++
/*
	特性
	1. 模块隔离
	2. 事件通知
	3. 支持脚本编程 (lua
	4. 轻量的分布式集群 (后续支持


	框架结构概览

	app                                 module             interface
	+-----------------------------+     +------------+     X
	|                             |     |X           |
	|  +----------+ +----------+  |     |  ++ ++ ++  |
	|  |X         | |X         |  |     |  ++ ++ ++  |     logic cell
	|  |          | |          |  |     |            |     ++
	|  |          | |          |  |     +------------+     ++
	|  +----------+ +----------+  |
	|  +----------+ +----------+  |
	|  |X         | |X         |  |
	|  |          | |          |  |
	|  |          | |          |  |
	|  +----------+ +----------+  |
	+-----------------------------+


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

	
	SampleModule
		: public gsf::IModule			
		, public gsf::IEvent			
	{
		// 接口类
		// 1. 用于装载逻辑单元、和模块状态控制
		// 2. 用于接收、发送等各种事件的接口申明
	}


	module.before_init  // 模块创建
	module.init
	module.execute      // 服务器每帧调用
	module.shut         // 模块销毁
	module.after_shut		


	event.listen        // 侦听消息
	event.dispatch      // 发送消息
	event.broadcast     // 广播消息
*/
```
 
depend
--------
* c++11
* cmake 2.8+
* google protobuf 3.2.0
* google glog 0.3.4
* libevent 2.0.22
* lua 5.3
* redis


MIT License

Copyright (c) 2016 pojol
