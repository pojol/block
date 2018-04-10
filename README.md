# Block
[![Build Status](https://travis-ci.org/pojol/block.svg?branch=master)](https://travis-ci.org/pojol/block)
[![License](http://img.shields.io/badge/license-mit-blue.svg?style=flat-square)](https://raw.githubusercontent.com/labstack/echo/master/LICENSE)

## 简介
> *block* 是一个 **模块化的跨平台游戏服务器框架** ，通过不同的组织方式可以适应大多数的架构方式。通常您只需要使用 **lua** 就能完成大部分的游戏逻辑编写，但是您也可以使用 **c++** 来完成一些高密度计算的逻辑。

> 在 *block* 中 *Module* 是一个非常重要的概念，理论上所有的逻辑都应该被包装在 *Module* 之中，但是如何权衡 *Module* 的粒度需要用户自己抉择，比较简单的概括来说就是尽量做好数据的分片工作，尽可能的减少 *Module* 之间的交互。

> 在使用 *block* 之前推荐阅读所有标记 ✦ 的接口和用例，这有助于您全面的理解框架的设计意图。
***
✦ [开发指南](https://github.com/pojol/gsf/wiki/Guide) <br/>
✦ [接口手册](https://github.com/pojol/gsf/wiki/Events) <br/>
<br/>

## 概览
#### App
> 每个✦[App](https://github.com/pojol/gsf/wiki/Core_Application)代理一个进程，管理所有 *Module* 的创建，销毁，还有状态变化。
<br/>

#### Module & Mailbox
> 使用✦[Event](https://github.com/pojol/gsf/wiki/Core_Events)来处理 *block* 中 ✦[Module](https://github.com/pojol/gsf/wiki/Core_Module) 之间的交互，这种方式可以保证 *module* 之间的天然独立性。这种独立性，能显著的降低编码的复杂度和对数据分片的处理难度。

> 每个Module都持有一个mailbox，负责订阅|发送事件，外部发送过来的消息都会存放在Mailbox的FIFO队列中，如果订阅过这个消息则会在每帧中处理（消耗）掉。

✦ [listen](https://github.com/pojol/gsf/wiki/Core_Events) <br/>
✦ [dispatch](https://github.com/pojol/gsf/wiki/Core_Events) <br/>
✦ [rpc](https://github.com/pojol/gsf/wiki/Core_Events) <br/>
<br/>

#### Args
> 用于包装 *event* 的内容，✦[Args](https://github.com/pojol/gsf/wiki/Core_Args)由[ArgsPool](https://github.com/pojol/gsf/wiki/Core_ArgsPool)进行分配和回收。

***

#### Modules
* [network](https://github.com/pojol/gsf/wiki/Modules_Network_TCP)
* [distributed](https://github.com/pojol/gsf/wiki/Modules_Distributed)
* [luaAdapter](https://github.com/pojol/gsf/wiki/Modules_LuaProxy)
* [timer](https://github.com/pojol/gsf/wiki/Modules_Timer)
* [dbProxy](https://github.com/pojol/gsf/wiki/Guide_Mysql)
* [logger](https://github.com/pojol/gsf/wiki/Module_Logger)

***

[FAQ](https://github.com/pojol/gsf/wiki/FAQ) <br/>
[开发日志](https://github.com/pojol/gsf/wiki/DevLog) <br/>


***

#### Information
* [Change Log](https://github.com/pojol/gsf/wiki/ChangeLog)
