# GSF
[![Build Status](https://travis-ci.org/pojol/gsf.svg?branch=master)](https://travis-ci.org/pojol/gsf)
[![License](http://img.shields.io/badge/license-mit-blue.svg?style=flat-square)](https://raw.githubusercontent.com/labstack/echo/master/LICENSE)

## feature
- 组件结构，模块访问隔离
- 基于命令（接口）编程
- 有一定的内省能力（可以运行时开启资源，性能的使用报告
- 支持使用c/c++, lua编写模块 (lua模块可在运行时重载
- 支持分布式架构（可依据业务定制，建议多对数据进行分片，少同步
- 跨平台 （linux, windows


## modules
- [x] core （提供框架基础的支撑服务
- [x] network （基于libevent的网络层封装
    - [x] acceptor
    - [x] connector
- [ ] distributed （分布式相关的支持
    - [x] node
    - [x] coordinate
    - [ ] election
- [x] timer （简单易用的时钟模块，支持按毫秒，天，周更新
- [x] log （基于glog的封装
- [x] lua_proxy（处理和Lua交互的模块基于sol2和gsf::core::Args
- [x] reids_cache_proxy（灾备模块基于redis
- [x] mysql_proxy（mysql模块


## depend
- core, distributed, timer --------------- c++1y
- network -------------------------------- libevent 2.0.22+
- log ------------------------------------ glog 0.3.4+, fmtlib
- luaProxy ------------------------------- lua5.3, sol2
- redisCacheProxy ---------------------- redis 3.0+


## build & install 
- supported compilers , gcc4.9+, vs2015+
- depend cmake2.8+ 
- build (linux
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
