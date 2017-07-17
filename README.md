# GSF

## feature
- 组件结构，模块访问隔离
- 基于命令（接口）编程
- 有一定的内省能力（可以运行时开启资源，性能的使用报告
- 支持编写lua module （建议用于配置，热更，或频繁修改的小逻辑
- 支持分布式架构（可依据业务定制，建议多对数据进行分片，少同步
- 跨平台 （linux, windows, apple

***
## modules
- [x] core
- [x] network
    - [x] acceptor
    - [x] connector
- [ ] distributed
    - [x] node
    - [x] coordinate
    - [ ] election
- [x] timer
- [x] log
- [x] lua_proxy
- [x] reids_cache_proxy
- [ ] mysql_proxy

***
## sample
### echo
```c++

```

### timer
```c++

```

### distributed
```c++
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
```

***
## depend
- core, distributed, timer --------------- c++1y & cmake 2.8+
- network -------------------------------- libevent 2.0.22+
- log ------------------------------------ glog 0.3.4+
- redis_cache_proxy ---------------------- redis 3.0+

***
### License
	MIT
	Copyright (c) 2016 pojol
	
	pojol@outlook.com
