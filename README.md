# Block
[![Build Status](https://travis-ci.org/pojol/block.svg?branch=master)](https://travis-ci.org/pojol/block)
[![License](http://img.shields.io/badge/license-mit-blue.svg?style=flat-square)](https://raw.githubusercontent.com/labstack/echo/master/LICENSE)

## 简介
> *block* 是一个 **模块化的跨平台游戏服务器框架** ，通过不同的 [*Module*](https://github.com/pojol/block/wiki/Core_Module) 组装方式可以适应大多数的架构。通常您只需要使用 **lua** 就能完成大部分的游戏逻辑编写，但是您也可以使用 **c++** 来完成一些高密度计算的逻辑。


#### 特性
* 完全逻辑独立的模块，通过Mailbox拉取,推送消息
* 支持分布式构建，支持rpc
* 支持脚本热更（带状态退出，重入
* 支持基于redis的灾备机制和数据缓存
***

#### 依赖编译环境
* linux (gcc4.9+
* windows (vs2015+
***

#### 编译&安装
* install.py
***

#### 文档
* [document](https://github.com/pojol/block/wiki)
***

#### 依赖库
* [fmtlib](https://github.com/fmtlib/fmt)
* [lua](https://github.com/lua/lua)
* [sol2](https://github.com/ThePhD/sol2)
* [protobuf](https://github.com/google/protobuf)
* [pbc](https://github.com/cloudwu/pbc)
* [libevent](https://github.com/nmathewson/Libevent)
* [glog](https://github.com/google/glog)
* [mysql-connector/c](https://dev.mysql.com/downloads/connector/c/)
* [hiredis](https://github.com/redis/hiredis)
* [redis/windows](https://github.com/MicrosoftArchive/redis)
