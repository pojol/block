#ifndef _GSF_APPLICATION_HEADER_
#define _GSF_APPLICATION_HEADER_

#include "event.h"
#include "module.h"

#include <list>
#include <unordered_map>
#include <map>
#include <array>
#include <chrono>

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
		bool is_watch_pref = false;
		std::string name = "default";
	};

	class Application
		: public Module
		, public IEvent
	{

	public:
		Application();

		void init_cfg(const gsf::AppConfig &cfg);

		template <typename T>
		void regist_module(T *module, bool dynamic = false);

		template <typename T>
		uint32_t find_module_id();

		void run();

		void tick();

		void exit();

	protected:
		//！ 临时先写在这里，未来如果支持分布式可能要放在其他地方生成，保证服务器集群唯一。
		int32_t make_module_id();

		void unregist_dynamic_module(uint32_t module_id);

		typedef std::tuple<uint32_t, std::function<void()>, std::function<void()>, std::function<void(Module*, bool)>, Module*> Frame;
		void push_frame(uint64_t index, Frame frame);
		void pop_frame();

	private:
		AppState state_;

		std::list<Module *> module_list_;
		std::list<ModuleID> unregist_list_;

		std::unordered_map<uint32_t, int32_t> module_id_map_;
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

	template <typename T>
	void gsf::Application::regist_module(T *module, bool dynamic /* = false */)
	{
		module_list_.push_back(module);
		
		if (!dynamic) {
			module->set_id(make_module_id());
			auto _type_id = typeid(T).hash_code();
			auto _id_itr = module_id_map_.find(_type_id);
			if (_id_itr != module_id_map_.end()) {
				printf("regist repeated module!\n");
				return;
			}

			module_id_map_.insert(std::make_pair(_type_id, module->get_module_id()));
			module_name_map_.insert(std::make_pair(module->get_module_name(), module->get_module_id()));
		}
	}

	template <typename T>
	uint32_t gsf::Application::find_module_id()
	{
		auto _type_id = typeid(T).hash_code();
		auto _id_itr = module_id_map_.find(_type_id);
		if (_id_itr != module_id_map_.end()){
			return _id_itr->second;
		}

		return 0;
	}
}

#endif
