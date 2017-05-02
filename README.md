# gsf

组件式游戏服务器框架

Features
--------
- [x] 模块隔离
- [x] 事件响应
- [x] 支持脚本（lua
- [ ] 分布式
 
### 程序的运行时
```python
# frame (每一帧的处理逻辑
#       ->1. 驱动当前App状态下对应的Module执行函数
#       ->2. 检查是否有动态创建的Module
#       ->3. 将事件队列中的元素依次弹出处理
```
```python
# app:init state = BEFORE_INIT
# app:regist ，static module 的绑定阶段
# app:run   |
#           |frame 1 -> modules:before_init
#           |        ->
#           |        -> event_layer:execute
#           |
#     INIT  |frame 2 -> modules:init
#           |        -> 
#           |        -> event_layer:execute
#           |
#   EXECUTE |frame 3 -> modules:execute
#           |        ->
#           |        -> event_layer:execute
#           | 
#           |frame ... 
#           |
#           |frame 20 -> modules:execute                                    -/dynamic create module
#           |         -> it = halfway_frame:pop == 0 then it:before_init     |1.查看是否存在元素
#           |         -> event_layer:execute                                 |2.弹出顶部元素，查看元素的帧是否和当前帧相符
#           |                                                                |3.查看标记位
#           |frame 21 -> modules:execute                                     |3.1 ==0则call before_init
#           |         -> it = halfway_frame:pop == 1 then it:init            |3.2 ==1则call init
#           |         -> event_layer:execute                                 |3.3 ==2则将module注册进app
#           |                                                                |        call execute
#           |frame 22 -> modules:execute
#           |         -> it = halfway_frame:pop == 2 then app.regist(it);it:execute
#           |         -> event_layer:execute
#           |
#           |frame ...
#           | 
#     SHUT  |frame 101 -> modules:shut
#           |          -> it = halfway_frame:pop then ... 
#           |          -> event_layer:execute
#           |
# AFTER_SHUT|frame 102 -> modules:after_shut
#           |          -> exit()
#           ↓
```
```python
其他   框架保证不同阶段中，事件处理必然是在不同的帧上
       相比其他框架这里多了一个中间调度的事件层
           使用hash容器保存事件到目标函数之间的绑定 find O(1)
           使用list容器保存正真运行时创建的event对象 pop O(1) , 这里后续会用池优化
           因此可预料的每帧最少会消耗 len(list) * (O(1) + O(1))
       框架虽然提供了动态创建module的功能，但并不推荐使用除非无法避免。
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

timer c++ & lua
--------
```c++
uint32_t _timer_module_id = 0;

dispatch(eid::app_id, eid::get_module, gsf::Args(std::string("TimerModule")), [&](gsf::Args args) {
   _timer_module_id = args.pop_uint32(0);
});

dispatch(_timer_module_id, eid::timer::delay_milliseconds, gsf::Args(get_module_id(), uint32_t(1000)), [=](gsf::Args args) {
   std::cout << "timer!" << std::endl;
});
```
```lua
module.init = function(module_id)

	local function delay_1000(args)
		print("timer!")
	end

	local function _callback(args)
		timer_module_id = args:pop_uint32(0)
		dispatch(timer_module_id, delay_milliseconds, module_id, 1000, delay_1000)
	end

	dispatch(app_id, get_module, "TimerModule", _callback)
end

```
