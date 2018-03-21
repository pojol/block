//
// Created by pojol on 2017/2/13.
//

#ifndef _GSF_EVENT_HEADER_
#define _GSF_EVENT_HEADER_

#pragma warning(disable:4819)

#include "module.h"
#include "../depend/event_list.h"
#include "types.h"

#include <functional>
#include <algorithm>
#include <tuple>
#include <queue>
#include <vector>
#include <unordered_map>

#include "args.h"

#include "single.h"

namespace gsf
{
	typedef std::pair<uint32_t, uint32_t> EventPair;
	typedef std::function<void(ArgsPtr)> CallbackFunc;
	typedef std::function<void(const std::string &)> LuaCallbackFunc;
	typedef std::function<void (ArgsPtr, CallbackFunc)> DispatchFunc;
	typedef std::function<void (ArgsPtr, DispatchFunc)> EventFunc;

	typedef std::function<void(const ArgsPtr &, int32_t, bool)> RpcCallback;
	typedef std::function<void(uint32_t, int32_t, gsf::ArgsPtr, gsf::RpcCallback)> RpcFunc;

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
		virtual void listen(Module *self, gsf::EventID event, DispatchFunc func);
		virtual void listen(ModuleID self, gsf::EventID event, DispatchFunc func);

		/**!
			用于将事件发往不同模块
		*/
		virtual void dispatch(gsf::ModuleID target, gsf::EventID event, ArgsPtr args, CallbackFunc callback = nullptr);

		virtual void boardcast(uint32_t event, ArgsPtr args);

		/**!
			rpc call ， 在分布式架构中需要远程调用的接口，依赖 NodeModule。
			callback 如果为nullptr， 则不等待 可以重复发起。
			callback 存在，则异步等待callback  如果 result 返回false 则代表这次调用没有成功。 需要进入回滚逻辑
			如果是业务层导致的事件执行失败，则在args中处理， result返回的成功、失败只代表框架层调用失败.
		*/
		virtual void rpc(uint32_t event, int32_t moduleid, ArgsPtr args, RpcCallback callback = nullptr);

		/**!
			移除module在event层上的绑定.
		*/
		virtual void wipeout(ModuleID self);
		virtual void wipeout(ModuleID self, EventID eventID);

		virtual void wipeout(Module *self);
		virtual void wipeout(Module *self, EventID eventID);


		// private - -
		virtual void listenRpc(RpcFunc callbackRpc);
	};

	class EventModule
			: public gsf::utils::Singleton<EventModule>
			, public Module
	{
		friend class IEvent;

	public:
		EventModule();

	protected:
		void execute() override;

		void bindEvent(uint32_t module_id, uint32_t event, DispatchFunc func);

		void dispatch(uint32_t module_id, uint32_t event, ArgsPtr args, CallbackFunc callback = nullptr);

		void boardcast(uint32_t event, ArgsPtr args);
		///

		void bindRpc(RpcFunc rpc_callback);
		void dispatchRpc(uint32_t event, int32_t moduleid, ArgsPtr args, RpcCallback callback = nullptr);

		void rmvEvent(ModuleID module_id);
		void rmvEvent(ModuleID module_id, EventID event_id);

#ifdef WATCH_PERF
		std::string get_tick_info(uint32_t count, uint32_t tick_count) override
		{
			auto c = static_cast<float>(tick_consume_ / 1000 / count);
			char buf[20];
			snprintf(buf, sizeof(buf), "%.3f", c);
#ifdef WIN32
			sscanf_s(buf, "%f", &c);
#else
			sscanf(buf, "%f", &c);
#endif

			std::string _info = getModuleName() + ":" + (buf)+" ms\n";

			typedef std::vector<std::pair<int, std::string>> PFList;
			std::vector<std::pair<int, std::string>> _pf_list;

			for (auto &itr : typeMap_)
			{
				for (MIList::iterator militr = itr.second.begin(); militr != itr.second.end(); ++militr)
				{
					_pf_list.push_back(std::make_pair(militr->calls_, "module " + std::to_string(itr.first) 
						+ "\t event " + std::to_string(militr->eventID_) 
						+ "\t calls " + std::to_string(militr->calls_) + "\n"));
					militr->calls_ = 0;
				}
			}

			std::sort(_pf_list.begin(), _pf_list.end(), [&](PFList::value_type l, PFList::value_type r) ->bool {
				return (l > r);
			});

			int _idx = 0;
			for (auto itr = _pf_list.begin(); itr != _pf_list.end(); ++itr, _idx++)
			{
				if (_idx < 10) {
					_info += itr->second;
				}
				else {
					break;
				}
			}

			tick_consume_ = 0;
			return _info;
		}
#endif // WATCH_PERF


    private:

		struct ModuleIterfaceObj
		{
			EventID eventID_;
			DispatchFunc eventFunc_;

            bool effective_ = true;
#ifdef WATCH_PERF
			uint32_t calls_ = 0;

#endif // WATCH_PERF
		};

        struct EventInfo
        {
            gsf::EventID eventID_;
            gsf::ModuleID target_;
            gsf::ArgsPtr ptr_;
			CallbackFunc callback_;
        };

		typedef std::vector<ModuleIterfaceObj> MIList;
		typedef std::unordered_map<gsf::ModuleID , MIList> ModuleEventMap;
        typedef std::queue<EventInfo*> EventQueue;

		ModuleEventMap typeMap_;
        EventQueue eventQueue_;

		RpcFunc rpc_;
	};
}

#endif