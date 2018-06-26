#pragma once 

#include "../depend/event_list.h"
#include "../utils/single.h"
#include "../utils/timer.hpp"
#include "../utils/logger.hpp"

#include "args.h"
#include "appConfig.hpp"

#include <list>
#include <unordered_map>
#include <map>
#include <array>
#include <chrono>

#include <fmt/format.h>


namespace block
{
	class Module;
	struct MailBox;
	typedef std::shared_ptr<MailBox> MailBoxPtr;

	class AppImpl
	{
		enum AppState
		{
			BEFORE_INIT = 0,
			INIT,
			EXECUTE,
			SHUT,
			AFTER_SHUT,
		};

		friend struct MailBox;
	public:
		AppImpl();

		std::string getName() const;

		block::ModuleID getModuleID(const std::string &moduleName) const;

		bool hasModule(block::ModuleID moduleid);

		const block::AppConfig & getAppCfg();

		uint64_t getSystemTick();

		void initCfg(const block::AppConfig &cfg);

		template <typename T>
		void createModule(T *module);

		block::ModuleID createDynamicModule(const std::string &moduleType);

		void deleteModule(block::ModuleID moduleID);

		void run();

		void exit();

		int64_t uuid();

		utils::Logger getLogger() { return logger_; }
		utils::Timer & getTimer() { return timer_; }

		void reactorRegist(block::ModuleID moduleID, block::EventID event);
		void reactorDispatch(block::ModuleID self, block::ModuleID target, block::EventID event, block::ArgsPtr args);
		void reactorBoardcast(block::ModuleID self, block::EventID event, block::ArgsPtr args);

	protected:
		void tick();

		template <typename T>
		void registModule(T *module, bool dynamic = false);

		void unregist_dynamic_module(block::ModuleID module_id);

		typedef std::tuple<uint32_t, std::function<void()>, std::function<void()>, std::function<void(Module*, bool)>, Module*> Frame;
		void pushFrame(uint64_t index, Frame frame);
		void popFrame();

	private:
		utils::Logger logger_;
		utils::Timer timer_;

		AppState state_;
		int sequence_ = 0;
		uint64_t start_time_ = getSystemTick();

		std::list<Module *> module_list_;
		std::multimap<uint64_t, std::pair<uint16_t, Module*>> exit_list_;

		std::unordered_map<std::string, block::ModuleID> module_name_map_;

		std::multimap<uint64_t, Frame> halfway_frame_;

		std::map<std::pair<block::ModuleID, block::EventID>, MailBoxPtr> mailboxMap_;

		bool shutdown_;

		uint64_t cur_frame_;

		int32_t module_idx_;

		block::AppConfig cfg_;
	};


	template <typename T>
	void block::AppImpl::createModule(T *module)
	{
		registModule(module);
	}

	template <typename T>
	void block::AppImpl::registModule(T *module, bool dynamic /* = false */)
	{
		module_list_.push_back(module);

		if (!dynamic) {
			module->setID(uuid());
			//auto _type_id = typeid(T).hash_code();
			//auto _id_itr = module_id_map_.find(_type_id);
			//if (_id_itr != module_id_map_.end()) {
			//	printf("regist repeated module!\n");
			//	return;
			//}

			//module_id_map_.insert(std::make_pair(_type_id, module->get_module_id()));
			module_name_map_.insert(std::make_pair(module->getModuleName(), module->getModuleID()));
		}
	}
}
