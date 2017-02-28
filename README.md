# gsf

游戏服务器框架 重构中...

framework
----------

```python
    ##
    #3      door
    #   
    #2    network     mailbox      timer    other_module    user_module              
    ##       ↓           ↓          ↓         ↓               ↓
    #1------------------------------------------------------------------> run
```

layer 1
----------

> 游戏的主循环，所有执行的module都需要注册进来。

layer 2
----------

> 游戏中的模块

```python
    virtual void before_init();
	virtual void init();

	virtual void execute();

	virtual void shut();
	virtual void after_shut();
```

layer 3
----------

> 事件层

```python
    virtual void listen()
    virtual void dispatch()
```
