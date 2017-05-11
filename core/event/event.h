//
// Created by pojol on 2017/2/13.
//

#ifndef _GSF_EVENT_HEADER_
#define _GSF_EVENT_HEADER_

#pragma warning(disable:4819)

#include "../module/module.h"
#include "event_handler.h"
#include "../../common/event_list.h"

#include <functional>
#include <tuple>
#include <list>
#include <vector>
#include <unordered_map>

#include <args/args.h>
#include <args/block.h>

#include "../../common/single.h"

namespace gsf
{
	typedef std::pair<uint32_t, uint32_t> EventPair;
	typedef std::function<void(gsf::Args)> CallbackFunc;
	typedef std::function<void(gsf::Args, CallbackFunc)> EventFunc;
	typedef std::function<void(uint32_t, uint32_t, BlockPtr)> RemoteEventFunc;

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

		void bind_remote_event(uint32_t type_id, RemoteEventFunc func);

		void add_cmd(uint32_t type_id, uint32_t event, gsf::Args args, CallbackFunc callback = nullptr);

		void add_remote_cmd(uint32_t type_id, uint32_t fd_list, uint32_t msg_id, BlockPtr blockptr);

		///

		void rmv_event(uint32_t module_id);

    private:
		typedef std::unordered_map<uint32_t, EventFunc> InnerMap;
		typedef std::unordered_map<uint32_t, InnerMap> TypeMap;

		typedef std::list<std::tuple<uint32_t, uint32_t, gsf::Args, CallbackFunc>> CmdList;

		typedef std::list<std::tuple<uint32_t, uint32_t, uint32_t, BlockPtr>> RemoteCallbackList;

		TypeMap type_map_;
		CmdList cmd_list_;
	};
}

#endif