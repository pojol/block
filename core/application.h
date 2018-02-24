#ifndef _GSF_APPLICATION_HEADER_
#define _GSF_APPLICATION_HEADER_

#include "event.h"
#include "module.h"
#include "single.h"

#include <list>
#include <unordered_map>
#include <map>
#include <array>
#include <chrono>

#include <fmt/format.h>

namespace gsf
{
	enum AppState
	{
		BEFORE_INIT = 0,
		INIT,
		EXECUTE,
		SHUT,
		AFTER_SHUT,
	};

	struct AppConfig
	{
		uint32_t tick_count = 20;
		uint32_t pool_args_count = 1024 * 10;
		bool is_watch_pref = false;
		std::string name = "default";
		int32_t machine_ = 0;
	};

	class Application : public gsf::utils::Singleton<Application>
		, public IEvent
	{

	public:
		Application();

		/*!
			获得进程的名称
		**/
		std::string get_app_name() const;

		/*!
			通过名字获取某个Module的实例ID， 仅支持静态创建的Module。
		**/
		gsf::ModuleID get_module(const std::string &modulName) const;

		/*!
			获取进程的机器ID
		**/
		uint32_t get_machine() const;

		/*!
			获得在集群内产生的唯一ID
		**/
		int64_t get_uuid();

		/*!
			初始化进程
		**/
		void init_cfg(const gsf::AppConfig &cfg);

		/*!
			创建一个Module
		**/
		template <typename T>
		void create_module(T *module);

		/*!
			动态创建一个Module
		**/
		gsf::ModuleID create_dynamic_module(const std::string &moduleType);

		/*!
			删除一个Module 
		**/
		void delete_module(gsf::ModuleID moduleID);

		/*!
			进程的主循环
		**/
		void run();

		/*!
			进程的退出逻辑
		**/
		void exit();

		// 只有在 regist log module 之后下面的日志接口才会有效
		// -- 调试日志 --
		template <typename ...P>
		void DEBUG_LOG(std::string module_name, std::string reason, std::string _fmt, P&& ...values);
		void DEBUG_LOG(std::string module_name, std::string reason);

		// -- 诊断日志 --
		template <typename ...P>
		void INFO_LOG(std::string module_name, std::string reason, std::string _fmt, P&& ...values);
		void INFO_LOG(std::string module_name, std::string reason);

		template <typename ...P>
		void ERR_LOG(std::string module_name, std::string reason, std::string _fmt, P&& ...values);
		void ERR_LOG(std::string module_name, std::string reason);

		template <typename ...P>
		void WARN_LOG(std::string module_name, std::string reason, std::string _fmt, P&& ...values);
		void WARN_LOG(std::string module_name, std::string reason);

		// -- 统计日志 -- 
		template <typename ...P>
		void RECORD_LOG(std::string behavior, uint32_t player, uint32_t time, std::string _fmt, P&& ...values);

	protected:

		void tick();

		template <typename T>
		void regist_module(T *module, bool dynamic = false);

		void unregist_module(gsf::ModuleID module_id);

		//！ 临时先写在这里，未来如果支持分布式可能要放在其他地方生成，保证服务器集群唯一。
		int32_t make_module_id();

		uint64_t get_system_tick();
		int64_t uuid();

		//void unregist_dynamic_module(uint32_t module_id);

		typedef std::tuple<uint32_t, std::function<void()>, std::function<void()>, std::function<void(Module*, bool)>, Module*> Frame;
		void push_frame(uint64_t index, Frame frame);
		void pop_frame();

	private:
		AppState state_;
		int sequence_ = 0;
		uint64_t start_time_ = get_system_tick();

		std::list<Module *> module_list_;
		std::multimap<uint64_t, std::pair<uint16_t, Module*>> exit_list_;

		std::unordered_map<std::string, int32_t> module_name_map_;

		std::multimap<uint64_t, Frame> halfway_frame_;

		bool shutdown_;

		uint64_t cur_frame_;

		int32_t module_idx_;

		gsf::AppConfig cfg_;

#ifdef WATCH_PERF
		uint32_t tick_len_;
		int32_t last_tick_;
#endif // WATCH_PERF
	};

	template <typename ...P>
	void gsf::Application::RECORD_LOG(std::string behavior, uint32_t player, uint32_t time, std::string _fmt, P&& ...values)
	{
		std::string _str = "";
		_str.append("behavior:");
		_str.append(behavior);
		_str.append(",player:");
		_str.append(std::to_string(player));
		_str.append(",time:");
		_str.append(std::to_string(time));
		_str.append(fmt::format(_fmt, std::forward<P>(values)...));
		dispatch(get_module("LogModule"), eid::log::print, gsf::make_args(gsf::LogInfo, _str));
	}

	template <typename ...P>
	void gsf::Application::WARN_LOG(std::string module_name, std::string reason, std::string _fmt, P&& ...values)
	{
		std::string _str = "";
		_str.append("[module] ");
		_str.append(module_name);
		_str.append(" [reason] ");
		_str.append(reason);
		_str.append("\n");
		_str.append(fmt::format(_fmt, std::forward<P>(values)...));
		dispatch(get_module("LogModule"), eid::log::print, gsf::make_args(gsf::LogWarning, _str));
	}


	template <typename ...P>
	void gsf::Application::ERR_LOG(std::string module_name, std::string reason, std::string _fmt, P&& ...values)
	{
		std::string _str = "";
		_str.append("[module] ");
		_str.append(module_name);
		_str.append(" [reason] ");
		_str.append(reason);
		_str.append("\n");
		_str.append(fmt::format(_fmt, std::forward<P>(values)...));
		dispatch(get_module("LogModule"), eid::log::print, gsf::make_args(gsf::LogErr, _str));
	}


	template <typename ...P>
	void gsf::Application::INFO_LOG(std::string module_name, std::string reason, std::string _fmt, P&& ...values)
	{
		std::string _str = "";
		_str.append("[module] ");
		_str.append(module_name);
		_str.append(" [reason] ");
		_str.append(reason);
		_str.append("\n");
		_str.append(fmt::format(_fmt, std::forward<P>(values)...));
		dispatch(get_module("LogModule"), eid::log::print, gsf::make_args(gsf::LogInfo, _str));
	}



	template <typename ...P>
	void gsf::Application::DEBUG_LOG(std::string module_name, std::string reason, std::string _fmt, P&& ...values)
	{
		std::string _str = "";
		_str.append("[module] ");
		_str.append(module_name);
		_str.append(" [reason] ");
		_str.append(reason);
		_str.append("\n");
		_str.append(fmt::format(_fmt, std::forward<P>(values)...));
		dispatch(get_module("LogModule"), eid::log::print, gsf::make_args(gsf::LogDebug, _str));
	}

	template <typename T>
	void gsf::Application::create_module(T *module)
	{
		regist_module(module);
	}

	template <typename T>
	void gsf::Application::regist_module(T *module, bool dynamic /* = false */)
	{
		module_list_.push_back(module);
		
		if (!dynamic) {
			module->set_id(make_module_id());
			//auto _type_id = typeid(T).hash_code();
			//auto _id_itr = module_id_map_.find(_type_id);
			//if (_id_itr != module_id_map_.end()) {
			//	printf("regist repeated module!\n");
			//	return;
			//}

			//module_id_map_.insert(std::make_pair(_type_id, module->get_module_id()));
			module_name_map_.insert(std::make_pair(module->get_module_name(), module->get_module_id()));
		}
	}

#define APP gsf::Application::get_ref() 
}

#endif
