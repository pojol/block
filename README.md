# gsf

游戏服务器框架 重构中...

预览
--------
* 框架的驱动方式
  * application
  * module 
* Module之间的通信
  * IEvent 
  * find_module_id 
  * Args 
* 部署分布式结构
  * t
  
--------
```python
---------------------------------------------------------->app
    ↑ regist    ↑      ↑        ↑         ↑       ↑
 network       log   event    dbproxy   timer   script
 
 #程序所有的逻辑都由module实现，通过注册由app统一驱动。每个module都拥有唯一ID。
 #如果是分布式则通过协调服务器生成后统一通知到每个app
 #module 提供beforeinit, init, execute, shut, aftershut 五个方法，其中execute在每个服务器帧中调用。
 #module之间不能相互访问，可以通过event 和 module_id 来实现访问。
 #script module 由lua_state实现，使用方式同c++ module。 用户可以选择用自己喜欢的方式实现module
```

--------

* depend
    * c++11
    * cmake 2.8+
    * google protobuf 3.2.0
    * google glog 0.3.4
    * libevent 2.0.22
    * lua 5.3
    * redis
--------

> * network sample
```c++
class Client2LoginProxy
	: public gsf::Module
	, public gsf::IEvent
{
public:
	void init()
	{
		//! find acceptor id
		uint32_t _c2l_id = AppRef.find_module_id<C2LAcceptorModule>();
        
		listen(this, event_id::network::new_connect
			, [=](gsf::Args args, gsf::EventHandlerPtr callback) {
			std::cout << "new connect fd = " << args.pop_uint32(0) << std::endl;
		});

		listen(this, event_id::network::dis_connect
			, [=](gsf::Args args, gsf::EventHandlerPtr callback){
			std::cout << "dis connect fd = " << args.pop_uint32(0) << std::endl;
		});

		//! 开启侦听 socket accept , 并和acceptor 建立绑定关系。
		gsf::Args args;
		args << get_module_id() << std::string("127.0.0.1") << uint32_t(8001);
		dispatch(_c2l_id, event_id::network::make_acceptor, args);
	}
};

class EntityMgr
	: public gsf::Module
	, public gsf::IEvent
{
public:

	void init()
	{
		uint32_t _em_id = AppRef.find_module_id<gsf::EventModule>();
        
        	using std::placeholders;
		auto arr = {
			std::make_pair(uint32_t(1001), std::bind(&EntityMgr::fun_msg1001, this, _1, _2)),
			std::make_pair(uint32_t(1002), std::bind(&EntityMgr::fun_msg1002, this, _1, _2)),
		};

		for (auto nod : arr)
		{
			//! 将消息回调绑定
			gsf::Args args;
			args << get_module_id() << nod.first << nod.second;
			dispatch(_em_id, event_id::network::bind_remote_callback, args);
		}
	}

	void fun_msg1001(uint32_t fd, gsf::BlockPtr blockptr)
	{
		test_network::Info _info;
		_info.ParseFromArray(blockptr->buf_, blockptr->size_);
		std::cout << _info.id() << " " << _info.name() << std::endl;

		_info.set_name("world");
        
        	//! 向client发出消息
		AppRef.sendmsg<C2LAcceptorModule>(this, fd, 1002, _info);
	}
};
```
    
