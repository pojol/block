# Block
[![Build Status](https://travis-ci.org/pojol/block.svg?branch=master)](https://travis-ci.org/pojol/block)
[![License](http://img.shields.io/badge/license-mit-blue.svg?style=flat-square)](https://raw.githubusercontent.com/labstack/echo/master/LICENSE)

## 简介
> *block* 是一个 **模块化的跨平台游戏服务器框架** ，通过不同的 [*Module*](https://github.com/pojol/block/wiki/Core_Module) 组装方式可以适应大多数的架构。通常您只需要使用 **lua** 就能完成大部分的游戏逻辑编写，但是您也可以使用 **c++** 来完成一些高密度计算的逻辑。

> 在 *block* 中 *Module* 是一个非常重要的概念，理论上所有的逻辑都应该被包装在 *Module* 之中，但是如何权衡 *Module* 的粒度需要用户自己抉择，比较简单的概括来说就是尽量做好数据的分片工作，尽可能的减少 *Module* 之间的交互。

> 在使用 *block* 之前推荐阅读[Wiki](https://github.com/pojol/block/wiki)中标记 ✦ 的接口和用例，这有助于您全面的理解框架的设计意图。
***

#### 特性
* 完全逻辑独立的模块，通过Mailbox拉取,推送消息
* 支持分布式构建，支持rpc
* 支持脚本热更（带状态退出，重入
* 支持基于redis的灾备机制和数据缓存
***
#### 内置模块
* [tcp](https://github.com/pojol/gsf/wiki/Modules_Network_TCP)
* [distributed](https://github.com/pojol/gsf/wiki/Modules_Distributed)
* [luaAdapter](https://github.com/pojol/gsf/wiki/Modules_LuaProxy)
* [timer](https://github.com/pojol/gsf/wiki/Modules_Timer)
* [dbProxy](https://github.com/pojol/gsf/wiki/Guide_Mysql)
* [logger](https://github.com/pojol/gsf/wiki/Module_Logger)

***

#### 依赖编译环境
* linux (gcc4.9+
* windows (vs2015+
***
#### 编译&安装
* install.py
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
