# gsf

游戏服务器框架 重构中...

framework
----------

```python
    ##
    #3    door
    #   
    #2    network     mailbox      timer    other_module    user_module              
    ##       ↓           ↓          ↓         ↓               ↓
    #1------------------------------------------------------------------> run
```

1 app layer 
----------
> 应用层，主要控制游戏的主循环，管理所有注册进来的module。

2 module layer
----------
> 模块层，游戏中的任何业务逻辑，包括底层组件都由module构成。module包含以下方法，不同的module之间只能通过event交互。

```python
	virtual void before_init()
	virtual void init()

	virtual void execute()

	virtual void shut()
	virtual void after_shut()
```

3 event layer
----------
> 事件层，用于在不同模块之间传递消息。

```python
    virtual void listen()
    virtual void dispatch()
```

timer
----------
```c++
	listen(make_event<gsf::modules::TimerModule>(event_id::timer::make_timer_success)
		, [=](gsf::Args args, gsf::EventHandlerPtr callback) {
		std::cout << "success by event id : " << args.pop_uint32(0) << std::endl;
	});

	gsf::Args args;
	args << get_door_id<TestClickModule>() << uint32_t(1000);

	dispatch(make_event<gsf::modules::TimerModule>(event_id::timer::delay_milliseconds)
		, args
		, make_callback(&TestClickModule::test_1, this, std::string("hello,timer!")));
```

network
----------
```c++
	listen(make_event<Client2LoginModule>(event_id::network::new_connect)
		, [=](gsf::Args args, gsf::EventHandlerPtr callback) {
		std::cout << "new connect fd = " << args.pop_uint32(0) << std::endl;
	});

	listen(make_event<Client2LoginModule>(event_id::network::dis_connect)
		, [=](gsf::Args args, gsf::EventHandlerPtr callback){
		std::cout << "dis connect fd = " << args.pop_uint32(0) << std::endl;
	});

	gsf::Args args;
	args << get_door_id<Client2LoginProxy>() << std::string("127.0.0.1") << uint32_t(8001);
	dispatch(make_event<Client2LoginModule>(event_id::network::make_acceptor), args);
```
