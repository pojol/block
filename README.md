# gsf

组件式游戏服务器框架

Features
--------

 

--------
```python
---------------------------------------------------------->app loop
    ↑ regist    ↑      ↑        ↑         ↑       ↑
 network       log   event    dbproxy   timer   script
 
 # 程序所有的逻辑都由module实现，通过注册由app统一驱动，每个module都拥有唯一ID。 如果是分布式则通过协调服务器生成后统一通知到每个app。
 
 # module 提供beforeinit, init, execute, shut, aftershut 五个方法，其中execute在每个服务器帧中调用。
 
 # module之间不能相互访问，可以通过event 和 module_id 来实现访问。
 
 # script module 由lua_state实现，使用方式同c++ module。 用户可以选择用自己喜欢的方式实现module
```

--------

* depend
    * c++11
    * cmake 2.8+
    * google protobuf 3.2.0
    * google glog 0.3.4
    * libevent 2.0.22
    * lua 5.3
    * redis
--------
    
