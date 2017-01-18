# gsf

游戏服务器框架 施工中...

###network
- [x] IO多线程（半同步半异步
- [x] 逻辑单线程（消费者生产者模式
- [x] 支持数据缓冲区
- [x] 支持消息绑定器
- [x] IOStream（二进制流处理
- [x] Message（可定制默认采用IOStream，后期扩充定制的protobuf版本
- [ ] 优化中... （目前 intel core i7-4790k cpu @4.00GHz QPS 70w+


###distributed
- [ ] zookeeper (分布式协调
- [ ] 服务器集群生命周期维护(开启，关闭，状态检视

```python
#           client *n
#----------------------------------
#           login *n
#----------------------------------
#       gate *n     gatemgr *1
#----------------------------------
#       service *n  servicemgr *1
#----------------------------------
#       dbcache *1  access *1
#----------------------------------
## login    为client分配gate，可以部署在多台物理机分摊压力
## gate     客户端和服务器的交互通道，由login指定
## gatemgr  管理gate，并负责gate的负载均衡
## service  游戏业务，可以部署在多台物理机
## servicemgr 管理所有的service,并负责负载均衡
## access   负责介入第三方相关的业务
## dbcache  高速缓存，数据库代理（mongodb,mysql
```

###database (mysql or mongodb
- [ ] dbproxy (redis

###数据采集和分析
- [ ] log4cxx （消息直接写文件，由python工具判断系统空闲时整合到logDB

#####robot
- [x] 模拟网络连接进行echo和并发测试 (基于python3.4+ asyncio
- [ ] 基于行为树生成的文件,模拟玩家行为。（用于游戏功能测试

###Utils
#####timer
- [x] 基于min-heap (插入删除复杂度O(log(n))，获取最小元素复杂度O(1)
- [x] 支持毫秒级的延时触发
- [x] 支持固定时间点更新 天（例如每天的早上6点10分更新
- [ ] 支持固定时间点更新 周
- [ ] 支持固定时间点更新 月
#####string

###depend
* c++11
* cmake2.8+
* libevent-2.0.22
* python3.4+

###platform
- [x] windows
- [x] linux
- [ ] apple
