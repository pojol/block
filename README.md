# gsf

游戏服务器框架 施工中<br>

**网络**核心用libevent，但采用多线程的方式（半同步，半异步）。同时保证逻辑层是单线程的（生产消费模型）

**分布式|负载均衡**
Zookeeper
redis

**数据**的组织形式用json，落地和消息分发用protobuf进行序列化。 如果是mongo考虑直接用bson落地。
json2pb
protobuf
redis （缓存队列，内存快照
mysql
mongodb

**日志**直接落地，判断cpu是否空闲后由脚本汇总到mongodb log库

**工具**自动测试工具（python3.x asyncio用于模拟网络用户行为，和压力测试

**其他**
timer基于min-heap的定时器，可以注册类的成员函数，和任意数量和类型的形参。
