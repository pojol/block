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
#include <algorithm>
#include <tuple>
#include <list>
#include <vector>
#include <unordered_map>

#include "args.h"

#include "single.h"

namespace gsf
{
	typedef std::pair<uint32_t, uint32_t> EventPair;
	typedef std::function<void(const ArgsPtr &)> CallbackFunc;
	typedef std::function<void(const ArgsPtr &, CallbackFunc)> EventFunc;

	typedef std::function<void(const ArgsPtr &, bool)> RpcCallback;
	typedef std::function<void(const std::string &, uint32_t, const gsf::ArgsPtr &, gsf::RpcCallback)> RpcFunc;

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
		virtual void listen(Module *self, uint32_t event, EventFunc func);
		virtual void listen(ModuleID self, uint32_t event, EventFunc func);

		/**!
			用于将事件发往不同模块
		*/
		virtual void dispatch(uint32_t target, uint32_t event, const ArgsPtr &args, CallbackFunc callback = nullptr);

		virtual void boardcast(uint32_t event, const ArgsPtr &args, CallbackFunc callback = nullptr);

		/**!
			rpc call ， 在分布式架构中需要远程调用的接口，依赖 NodeModule。
			callback 如果为nullptr， 则不等待 可以重复发起。
			callback 存在，则异步等待callback  如果 result 返回false 则代表这次调用没有成功。 需要进入回滚逻辑
			如果是业务层导致的事件执行失败，则在args中处理， result返回的成功、失败只代表框架层调用失败.
		*/
		virtual void rpc(const std::string &module, uint32_t event, const ArgsPtr &args, RpcCallback callback = nullptr);

		/**!
			移除module在event层上的绑定.
		*/
		virtual void wipeout(ModuleID self);
		virtual void wipeout(ModuleID self, EventID event_id);

		virtual void wipeout(Module *self);
		virtual void wipeout(Module *self, EventID event_id);


		// private - -
		virtual void rpc_listen(RpcFunc rpc_callback);
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

		void bind_event(uint32_t module_id, uint32_t event, EventFunc func);

		void dispatch(uint32_t module_id, uint32_t event, const ArgsPtr &args, CallbackFunc callback = nullptr);
		void boardcast(uint32_t event, const ArgsPtr &args, CallbackFunc callback = nullptr);
		///

		void bind_rpc(RpcFunc rpc_callback);
		void dispatch_rpc(const std::string &module, uint32_t event, const ArgsPtr &args, RpcCallback callback = nullptr);

		void rmv_event(ModuleID module_id);
		void rmv_event(ModuleID module_id, EventID event_id);

#ifdef WATCH_PERF
		std::string get_tick_info(uint32_t count, uint32_t tick_count)
		{
			auto c = tick_consume_ / 1000 / count;
			char buf[20];
			sprintf(buf, "%.3f", c);
			sscanf(buf, "%f", &c);

			std::string _info = get_module_name() + ":" + (buf)+" ms\n";

			typedef std::vector<std::pair<int, std::string>> PFList;
			std::vector<std::pair<int, std::string>> _pf_list;

			for (auto &itr : type_map_)
			{
				for (MIList::iterator militr = itr.second.begin(); militr != itr.second.end(); ++militr)
				{
					_pf_list.push_back(std::make_pair(militr->calls_, "module " + std::to_string(itr.first) 
						+ "\t event " + std::to_string(militr->event_id_) 
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
			EventID event_id_;
			EventFunc event_func_;

#ifdef WATCH_PERF
			uint64_t calls_ = 0;

#endif // WATCH_PERF
		};

		typedef std::vector<ModuleIterfaceObj> MIList;
		typedef std::unordered_map<uint32_t, MIList> ModuleEventMap;

		ModuleEventMap type_map_;

		RpcFunc rpc_;
	};
}

#endif