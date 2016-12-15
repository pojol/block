# gsf

游戏服务器框架 施工中...<br>

###网络
- [x] 支持多线程（半同步半异步
- [x] 支持逻辑单线程（消费者生产者模式
- [x] 支持数据缓冲区
- [x] 支持消息绑定器（回调至类的成员函数
- [x] IOStream二进制流处理工具
- [x] Message (可定制默认采用IOStream，后期扩充定制的protobuf版本
- [ ] 优化 (ringbuffer 和 不必要的内存分配释放

###分布式&负载均衡
- [ ] zookeeper (分布式协调
- [ ] codis （分布式cache
- [ ] 服务器集群生命周期管理 （开启，关闭，状态检视

###数据
- [ ] json2pb
- [ ] protobuf （数据的序列化，网络间的消息分发
- [ ] redis （db 的cache层
- [ ] mysql
- [ ] mongodb

###日志
- [ ] log4cxx （消息直接写文件，由python工具判断系统空闲时整合到logDB

###工具
####robot
- [x] 模拟网络连接进行echo和并发测试 (基于python3.4+ asyncio

###其他
####timer
- [x] 基于min-heap (插入删除复杂度O(log(n))，获取最小元素复杂度O(1)
- [x] 支持秒级的延时触发
- [x] 支持固定时间点更新 天（例如每天的早上6点10分更新
- [ ] 支持固定时间点更新 周
- [ ] 支持固定时间点更新 月

###依赖
* cmake2.8+
* libevent-2.0.22
* python3.4+
