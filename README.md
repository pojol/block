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
	void before_init()
	void init()

	void execute()

	void shut()
	void after_shut()
```

3 event layer
----------
> 事件层，用于在不同模块之间传递消息。

```python
    void listen()
    void dispatch()
```
