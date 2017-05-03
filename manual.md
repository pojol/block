# gsf

组件式游戏服务器框架

Features
--------
- [x] 模块隔离
- [x] 事件响应
- [x] 支持脚本（lua
- [ ] 支持轻量的分布式架构

###模块的初始化
```python
'''
                                                    |
 c++                                               -+-   lua
//创建一个类继承module，ievent                       | -- 创建一个table，按需声明好对应的status func .lua
class EntityMgr                                     | module = {
    : public gsf::Module                            |   before_init = function() end,
    , public gsf::IEvent                            |   init = function() end,
{                                                   |   execute = function() end,
public:                                             |   shut = function() end,
	EntityMgr()                                 |   after_shut = function() end,
		: Module("EntityMgr")               | }
	{}                                          |
	~EntityMgr(){}                              |//lua module 需要lua_binder在c++层辅助绑定  .cpp
	// 按需声明实现函数                           | class EntityMgrLua
	void before_init() override {}              |       : public gsf::module
	void init() override {}                     |       , public gsf::ievent
	void execute() override {}                  | {
	void shut() override {}                     |       void before_init() override
	void after_shut() override {}               |       {
};                                                  |           dispatch(eid::app_id, eid::get_module
                                                    |              , gsf::args("LuaScriptBinder") 
int main() {                                        |              , [&](gsf::args args){
    gsf::application app; //构建一个进程管理器        |                  _binder = args.pop_uint32(0);
    app.init(); //初始化进程管理器                    |               });
                                                    |       }
    //绑定module到进程，frame中的调用会按判定次序      |       void init() override
    app.regist_module( new EntityMgr );             |       { //只需向lua_binder module 发送绑定命令就完成了c++/lua的绑定 创建了一个独立的lua_state
                                                    |            dispatch(_binder, eid::lua_proxy::create
    app.run();                                      |               , gsf::args(module_id, lua_path));
    return 0; //退出逻辑由每个module的shut实现。      |       }
}                                                   | };
                                                    | //将EntityMgrLua Module 注册到app就完成了绑定。
'''
```

###内置模块的使用
```c++
-- timer --                                         |
c++                                                -+-  lua
                                                    |
dispatch(eid::app_id, eid::get_module               | local function _delay_1000ms(args)
    , gsf::Args("TimerModule")                      |       print("hello,timer!")
    , [&](gsf::Args args){                          | end
	_timer_module_id = args.pop_uint32(0);      |
    });                                             | local function _get_timer_module(args)
                                                    |       _timer_module_id = args:pop_uint32(0)
dispatch(_timer_module_id                           |
    , eid::timer::delay_milliseconds                |       dipatch(_timer_module_id
    , gsf::Args(get_module_id(), 1000               |           , eid.timer.delay_milliseconds
    , [&](gsf::Args args){                          |           , module_id, 1000, _delay_1000ms)
        std::cout << "hello,timer!" << std::endl    | end
    });                                             |
                                                    | -- 通过get_module命令获取timer的module id
                                                    | dispatch(eid.app_id
                                                    |           , eid.get_module
                                                    |           , "TimerModule", _get_timer_module)
more events :                                       |
eid.timer.delay_day                                 |
eid.timer.delay_week                                |
eid.timer.remove_timer                              |
----                                            ----+----
                                                    |
-- acceptor --                                      +
                                                    |
----                                                |

-- connector --
----

-- log --
----
'''
```

### 动态构建module

### 程序的运行时
```python
'''
+--------->----------->--------------+
↑                                    ↓  one frame
+--------<------------<--------------+
while(1) 
    1. modules call status function
    2. proc dynamic module list
    3. proc active event list
    frame++;  
    sleep(fixed_frame_time - cosume);
'''
```
```python
# app:init status = BEFORE_INIT
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
