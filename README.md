# GSF
[![Build Status](https://travis-ci.org/pojol/gsf.svg?branch=master)](https://travis-ci.org/pojol/gsf)
[![License](http://img.shields.io/badge/license-mit-blue.svg?style=flat-square)](https://raw.githubusercontent.com/labstack/echo/master/LICENSE)

## 简介
> *gsf* 是一个 **模块化的跨平台服务器框架** ，通过不同的组织方式可以适应大多数的架构方式。通常您只需要使用 *lua* 就能完成大部分的游戏逻辑编写，但是您也可以使用 *c++* 来完成一些高密度计算的逻辑。

> 在 *gsf* 中 *Module* 是一个非常重要的概念，理论上所有的逻辑都应该被包装在 *Module* 之中，但是如何权衡 *Module* 的粒度需要用户自己抉择，比较简单的概括来说就是尽量做好数据的分片工作，尽可能的减少 *Module* 之间的交互。

> 在使用 *gsf* 之前推荐阅读所有标记 ✦ 的接口和用例，这有助于您全面的理解框架的设计意图。

***

## Event
> 使用✦[Event](https://github.com/pojol/gsf/wiki/Core_Events)来进行 *gsf* 中 *module* 之间的交互，这种方式可以保证 *module* 之间的天然独立性。这种独立性，能显著的降低编码的复杂度和对数据分片的处理难度。
✦ [listen](https://github.com/pojol/gsf/wiki/Core_Events) <br/>
✦ [dispatch](https://github.com/pojol/gsf/wiki/Core_Events) <br/>
✦ [rpc](https://github.com/pojol/gsf/wiki/Core_Events) <br/>
<br/>

getEv
> 通过这个接口可以获得本进程内的 *event* 实例（函数指针），在需要高频调用的时候可以使用这个接口优化掉事件过程中的损耗（但这个接口会一定程度侵入Module），在性能遇到性能瓶颈并且没有其他优化方案可选之前不太推荐使用。 在使用 *event* 实例时一定要注意所在模块销毁后**解除依赖的引用**

***

## Args
> 用于包装 *event* 的内容，✦[Args](https://github.com/pojol/gsf/wiki/Core_Args)由[ArgsPool](https://github.com/pojol/gsf/wiki/Core_ArgsPool)进行分配和回收。
***


## App
> 每个✦[App](https://github.com/pojol/gsf/wiki/Core_Application)代理一个进程，管理所有 *Module* 的创建，销毁，还有状态变化。
***

## Modules
* [network](https://github.com/pojol/gsf/wiki/Modules_Network_TCP)
* [distributed](https://github.com/pojol/gsf/wiki/Modules_Distributed)
* [luaProxy](https://github.com/pojol/gsf/wiki/Modules_LuaProxy)
* [timer](https://github.com/pojol/gsf/wiki/Modules_Timer)
* mysqlProxy
* redisProxy
* [logger](https://github.com/pojol/gsf/wiki/Module_Logger)

***

✦ [接口手册](https://github.com/pojol/gsf/wiki/Events) <br/>
✦ [开发指南](https://github.com/pojol/gsf/wiki/Guide) <br/>
<br/>
[FAQ](https://github.com/pojol/gsf/wiki/FAQ) <br/>
[开发日志](https://github.com/pojol/gsf/wiki/DevLog) <br/>


***

## Information
* [Change Log](https://github.com/pojol/gsf/wiki/ChangeLog)
