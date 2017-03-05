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
	listen_callback(event_id::timer::make_timer_success, [=](gsf::Args args) {
		std::cout << "success by event id : " << args.pop_uint32(0) << std::endl;
	});

	listen_callback(event_id::timer::make_timer_fail, [=](gsf::Args args) {
		std::cout << "fail by error id : " << args.pop_uint32(0) << std::endl;
	});

	gsf::Args args;
	args << get_door_id() << 1000;

	dispatch(event_id::timer::delay_milliseconds , args
		, make_callback(&TestClickModule::test_1, this, std::string("hello,timer!")));
```
