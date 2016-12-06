# gsf

游戏服务器框架 施工中...<br>

#####网络
    封装libevent，使用evbuffer作为缓冲区。支持多线程（半同步，半异步模式），支持逻辑单线程（生产消费模型，可能会采用ringbuffer等无锁队列

#####分布式&负载均衡
* Zookeeper
* redis

#####数据
    代码中维护json格式，落地和消息分发使用pb进行序列化和反序列化，如果是mongo考虑直接用bson落地。
* json2pb
* protobuf
* redis （缓存队列，内存快照
* mysql
* mongodb

#####日志
    直接落地，然后由工具向mangodb log库汇总
* log4cxx

#####工具
* **robot**自动测试工具（python3.x asyncio用于模拟网络用户行为，和压力测试<br>

#####其他
* **timer**基于min-heap的定时器，可以注册类的成员函数，支持任意数量和不同类型的形参。<br>

#####依赖库
* libevent-2.0.22
