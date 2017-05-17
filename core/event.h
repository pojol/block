//
// Created by pojol on 2017/2/13.
//

#ifndef _GSF_EVENT_HEADER_
#define _GSF_EVENT_HEADER_

#pragma warning(disable:4819)

#include "module.h"
#include "event_handler.h"
#include "event_list.h"
#include "types.h"

#include <functional>
#include <tuple>
#include <list>
#include <vector>
#include <unordered_map>

#include "args.h"

#include "single.h"

/*
	/
	特性
		1. 模块隔离
		2. 事件通知
		3. 支持脚本编程 (lua
		4. 轻量的分布式集群 (后续支持

	/
	框架结构概览

		app                                 module             interface
		+-----------------------------+     +------------+     X
		|                             |     |X           |
		|  +----------+ +----------+  |     |  ++ ++ ++  |
		|  |X         | |X         |  |     |  ++ ++ ++  |     logic cell
		|  |          | |          |  |     |            |     ++
		|  |          | |          |  |     +------------+     ++
		|  +----------+ +----------+  |
		|  +----------+ +----------+  |
		|  |X         | |X         |  |
		|  |          | |          |  |
		|  |          | |          |  |
		|  +----------+ +----------+  |
		+-----------------------------+

	/
	核心接口概览

		app.init_cfg			进程的初始化
		app.regist_module		管理module
		app.run					进程的运行时

		 进程的运行时
							 execute
					init     +---------------------------------> +          shut
		  app run   +        |                                   |          +       exit
		  ++-+-+-------------+                                   +------------------->
		   ^ ^ ^    +        |                                   |          +
		   | | |             + <---------------------------------+
		   + + +                                                          unregist
		regist module


	/
		SampleModule
			: public gsf::IModule			
			, public gsf::IEvent			
		{
			// 接口类
			// 1. 用于装载逻辑单元、和模块状态控制
			// 2. 用于接收、发送等各种事件的接口申明
		}

	/
		module.before_init		// 模块创建
		module.init
		module.execute			// 服务器每帧调用
		module.shut				// 模块销毁
		module.after_shut		

	/
		event.listen			// 侦听消息
		event.dispatch			// 发送消息
		event.broadcast			// 广播消息
*/

namespace gsf
{
	typedef std::pair<uint32_t, uint32_t> EventPair;
	typedef std::function<void(gsf::Args)> CallbackFunc;
	typedef std::function<void(gsf::Args, CallbackFunc)> EventFunc;

	class Module;

	class IEvent
	{
	public:
		IEvent();
		virtual ~IEvent();

		// --local--
		/**!
			用于侦听模块之间的消息
		*/
		virtual void listen(Module *target, uint32_t event, EventFunc func);
		virtual void listen(uint32_t self, uint32_t event, EventFunc func);

		/**!
			用于将事件发往不同模块
		*/
		virtual void dispatch(uint32_t target, uint32_t event, gsf::Args args, CallbackFunc callback = nullptr);


		/**!
			移除module在event层上的绑定.
		*/
		virtual void bind_clear(uint32_t module_id);
	};

	class EventModule
			: public gsf::utils::Singleton<EventModule>
			, public Module
	{
		friend class IEvent;

	public:
		EventModule();

	protected:
		void execute();

		void bind_event(uint32_t type_id, uint32_t event, EventFunc func);

		void add_cmd(uint32_t type_id, uint32_t event, gsf::Args args, CallbackFunc callback = nullptr);
		///

		void rmv_event(uint32_t module_id);

    private:
		typedef std::unordered_map<uint32_t, EventFunc> InnerMap;
		typedef std::unordered_map<uint32_t, InnerMap> TypeMap;

		typedef std::list<std::tuple<uint32_t, uint32_t, gsf::Args, CallbackFunc>> CmdList;

		TypeMap type_map_;
		CmdList cmd_list_;
	};
}

#endif