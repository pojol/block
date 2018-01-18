# GSF
[![Build Status](https://travis-ci.org/pojol/gsf.svg?branch=master)](https://travis-ci.org/pojol/gsf)
[![License](http://img.shields.io/badge/license-mit-blue.svg?style=flat-square)](https://raw.githubusercontent.com/labstack/echo/master/LICENSE)

## feature
- 组件结构，模块访问隔离
- 支持c++，lua 模块，支持运行时添加移除模块
- 支持分布式架构
- 跨平台 （linux, windows

## event
|```c++  ```|```lua  ```|comment|
|:----:|:----:|:----|
|[listen](#listen)|[listen](#listen)|监听某个模块发送的事件|
|[dispatch](#dispatch)| [dispatch](#dispatch)|向某个模块发送一个事件|
|[rpc](#rpc)|[rpc](#rpc)|发送一个跨进程的事件，如果有callback参数则必定会有异步返回|

## modules
| module        |       event           |             comment                  |    test case    |
| :----      |:----                 | :-----                       | :-----  |
| core          | [core](#core)   | 提供框架基础的支撑服务        |nil|
| acceptor      |      [network](#network)        |   基于libevent的接收器封装    |gsf_sample/echo_server|
| connector      |      [network](#network)       |    基于libevent的连接器封装   |gsf_sample/echo_client |
| coordinat| [distributed](#distributed)| 处理集群中的协调服务 | gsf_sample/distributed|
|node| [distributed](#distributed) | 处理集群中的远程分发和绑定相关服务 |gsf_sample/distributed|
|lua_proxy| [lua_proxy](#lua) | 管理LuaModule的状态以及和c++层的交互 |nil|
|timer| [timer](#timer)| 定时器相关|gsf_sample/timer|
|mysql_proxy| [mysql_proxy](#mysql_proxy)| mysql封装 |gsf_sample/db|
|redis_proxy| [redis_proxy](#redis_proxy)| redis的封装目前主要作用于灾备和mysql更新优化 |nil|
|logger| [logger](#logger)| 基于glog的日志模块 |nil|



## build & install 
- supported compilers , gcc4.9+, vs2015+
- depend cmake2.8+ 
- linux
    - cd gsf
    - python 3rd/update_3rd.py
    - mkdir build
    - cd build
    - cmake ..
    - make

	
## sample
- https://github.com/pojol/gsf_sample
- [x] timer
- [x] echo
- [x] db_proxy
- [x] distributed

# interface
## listen
### c++
```c++
listen(module_id, event_id, [&](const gsf::StreamPtr &args) {
    auto _xx = args.pop_xx();
    ...
    
	return nullptr;
});
```
### lua
```lua
-- resArgs = { ... }
listen(module_id, event_id, function(resArgs) 
    
    return ""
end)
```
## dispatch
### c++
```c++
retStream = dispatch(target_module_id, event_id, gsf::make_args(...))
retStream->pop_xxx()
...

```
### lua
```lua
resArgs = dispatch(target_module_id, event_id, { reqArgs ... })
```
## rpc
### c++
```c++
rpc(rpc_event, module_id, gsf::make_args(values ... ), [&](const gsf::ArgsPtr &args, int32_t process, bool result) {
    if (result) {
    
    	auto _ret = args->pop_xxx();
        ...
    }
    else {
        printf(args->pop_err())
    }
});
```
### lua
```lua
rpc(rpc_event, module_id, { reqArgs ... }, function(resArgs, progress, succ)

    if succ == true then
	dump(resArgs)

    else
	logWarn("module_name", resArgs[1])
    end

end)
```

# events

## core
```c++
enum base
{
	/*!
		comment: 通过字符串获得module的id， 只能获取静态显示声明的module。
		args: string module_name
		type: dispatch
		res : int32_t module_id 
	**/
	get_module,

	/*!
		comment: 运行过程中创建Module，需要通过 REGISTER_CLASS 提前进行类型定义。
		args: string module_type
		type: dispatch 
		res : int32_t module_id
	**/
	new_dynamic_module,
	
	/*!
		comment: 移除注册在App中的某个Module
		args: int32_t module_id
		type: dispatch
		res : nil
	**/
	delete_module,

	/*!
		comment: 在集群中创建一个唯一ID，需要用户通过config.machine 为App做好区分。
		args: nil
		type: dispatch
		res : int64_t uuid
	**/
	uuid,
};
```
## network
```c++
enum network
{
	/*!
		comment: 创建一个接收器
		args: int32_t module_id, string ip, int32_t port
		type: dispatch
		res : bool succ or fail, string reason
	**/
	make_acceptor = 2001,

	/*!
		comment: 创建一个连接器
		args: module_id, ip, port
		type: dispatch
		res : bool succ or fail, string reason
	**/
	make_connector,

	/*!
		comment: 踢掉某个现有的连接
		args: int32_t fd
		type: dispatch
		res : bool succ or fail, string reason
	**/
	kick_connect,

	/*!
		comment: 发送一条网络消息
		args: int32_t fd, int32_t msgid, stream block
		type: dispatch
		res : nil
	**/
	send,

	/*!
		comment: 接收到一条网络消息
		args: nil
		type: listen
		res : int32_t fd, int32_t msgid, stream block
	**/
	recv,
	
	/*!
		comment: 接收到一个新的连接（适用于 acceptor
		args: nil
		type: listen
		res : int32_t fd
	**/
	new_connect,

	/*!
		comment: 连接被断开
		args: int32_t fd
		type: listen
		res : int32_t fd, string reason
	**/
	dis_connect,
};
```
## distributed
```c++
//! 凡是gsf集群内使用rpc交互的消息都应该定义在distributed区块
enum distributed
{
	/*!
		comment: 将Node绑定到Coordinator
		args: string node_type, int32_t node_id, string root_ip, int32_t root_port, [{string module_name, int32_t module_id, int32_t module_fature} ... ]
		type: rpc
		res : stream args, int32_t progress, bool succ
	**/
	coordinat_regist,
	
	/***/
	coordinat_unregit,

	/*!
		comment: 调整Node在Coordinator中的权重
		args: int32_t node_id, string module_name, int32_t module_fature, int32_t +- weight 
		type: rpc
		res : stream args, int32_t progress, bool succ
	**/
	coordinat_adjust_weight,

	/*!
		comment: 通过ModuleName和ModuleFeature选取一个适合自己的Node
		args: string module_name, int32_t module_fature
		type: rpc
		res : stream args, int32_t progress, bool succ
	**/
	coordinat_select,

	/**!
		comment: 查询Mysql数据库， args中存放单条查询信息， 如果返回的是数组 progress 则代表当前进度 -1 代表eof
		args: int32_t module_id, string sql
		type: rpc
		res : stream args, int32_t progress, bool succ
	*/
	mysql_query,
};
```
## lua
```c++
enum lua_proxy
{
	/*!
		comment: 创建 Lua Script Module ,proxy会自动完成c++/lua的相关绑定 
		args: int32_t module_id, string script_dir, string script_name
		type: dispatch
		res : bool succ
	**/
	create = 2301,

	/*!
		comment: 重新装载 Lua Script Module, 会走标准的退出和进入流程 init, shut 。即便持有状态也可以方便的热更
		args: int32_t module_id
		type: dispatch
		res : bool succ
	**/
	reload,

	/*!
		comment: 移除 Lua Script Module
		args: int32_t module_id
		type: dispatch
		res : bool succ
	**/
	destroy,
};
```

## timer
```c++
//timer module 插入删除复杂度O(log(n))，获取最小元素复杂度O(1)，触发时间 >= app.config.tick_count
enum timer
{
	/*!
		comment: 延迟若干毫秒触发
		args: int32_t module_id, int32_t milliseconds
		type: dispatch
		res : int64_t timer_id
	**/
	delay_milliseconds = 2201,

	/*!
		comment: 隔天触发
		args: int32_t module_id, int32_t hour, int32_t minute
		type: dispatch
		res : int64_t timer_id
	**/
	delay_day,

	/*!
		comment: 从定时器中移除某个timer
		args: int64_t timer_id
		type: dispatch
		res : bool succ
	**/
	remove_timer,

	/*!
		comment: 触发Timer
		args: nil
		type: listen
		res : int64_t timer_id
	**/
	timer_arrive,
};
```

## mysql_proxy
```c++
enum db_proxy
{
	/*!
		comment: 建立一个新的Mysql连接
		args: string host, string user, string password, string dbName, int32_t port
		type: dispatch
		res : bool succ
	**/
	mysql_connect,

	/**!
		comment: 查询Mysql数据库， args中存放单条查询信息， 如果返回的是数组 progress 则代表当前进度 -1 代表eof
		args: moduleid, sql
		type: rpc
		res : stream args, int32_t progress, bool succ
	*/
	mysql_query,
};
```

## redis_proxy
```c++

```

## logger
```c++
// 如果需要单独部署logServer则可以在distributed加入rpc事件
enum log
{
	/*!
		comment: 输出日志
		args: int32_t loglv, string title, string context
		type: listen
		res : nil 
	**/
	print = 2101,
};
```
