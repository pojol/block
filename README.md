# GSF
![](https://travis-ci.org/pojol/gsf.svg?branch=master)

## feature
- 组件结构，模块访问隔离
- 基于命令（接口）编程
- 有一定的内省能力（可以运行时开启资源，性能的使用报告
- 支持编写lua module （建议用于配置，热更，或频繁修改的小逻辑
- 支持分布式架构（可依据业务定制，建议多对数据进行分片，少同步
- 跨平台 （linux, windows, apple


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


## depend
- core, distributed, timer --------------- c++1y
- network -------------------------------- libevent 2.0.22+
- log ------------------------------------ glog 0.3.4+
- redis_cache_proxy ---------------------- redis 3.0+


# build & install 
- supported compilers , gcc4.9+, vs2015+
- depend cmake2.8+ 
- build (linux
    - python 3rd/update_3rd.py
    - mkdir build
    - cd build
    - cmake ..
    - make


### License
	MIT
	Copyright (c) 2016 pojol
	
	pojol@outlook.com
