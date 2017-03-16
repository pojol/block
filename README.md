# gsf

游戏服务器框架 重构中...

预览
--------
* 框架的驱动方式
  * application
      * 管理所有的module，并在run方法内调度所有module的execute。
  * module 
      * 框架中的所有逻辑都由module包装，注册进application。
* Module之间的通信
  * IEvent 
      * 提供事件接口，如果module需要监听或者派发事件则需要继承这个接口。
      * 任何module都不能直接访问其他module，需要通过事件来传递消息。
  * find_module_id 
      * 接口可以通过静态类型找到目标module的id
  * Args 
      * 结构可以填充事件的参数（可扩充类型，和数量。
* 部署分布式结构

--------
> * apps
   * login
   * dbproxy
   * cell
   * scheduler
   * monitor
> * modules
   * acceptor
   * connector
   * message_queue
   * meta_data
   * timer
    
--------

* depend
    * c++11
    * cmake 2.8+
    * google protobuf   
    * libevent2.x       
    * redis

--------

> * network
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
    
