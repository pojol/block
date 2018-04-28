#pragma once

#include "../depend/event_list.h"
#include "../utils/single.h"
#include "../utils/timer.hpp"
#include "../utils/logger.hpp"

#include "args.h"

#include <list>
#include <unordered_map>
#include <map>
#include <array>
#include <chrono>

#include <fmt/format.h>

namespace block
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

	class Module;
	struct MailBox;
	typedef std::shared_ptr<MailBox> MailBoxPtr;

	class utils::Logger;
	class utils::Timer;
	typedef std::shared_ptr<utils::Logger> LoggerPtr;
	typedef std::shared_ptr<utils::Timer> TimerPtr;

	class Application : public block::utils::Singleton<Application>
	{
		friend struct MailBox;
	public:
		Application();

		/*!
			获得进程的名称
		**/
		std::string getName() const;

		/*!
			通过名字获取某个Module的实例ID， 仅支持静态创建的Module。
		**/
		block::ModuleID getModule(const std::string &modulName) const;

		/*!
			获取进程的机器ID
		**/
		uint32_t getMachine() const;

		/*!
			获得在集群内产生的唯一ID
		**/
		int64_t getUUID();

		/*!
			获得当前的毫秒数
		**/
		uint64_t getSystemTick();

		/*!
			初始化进程
		**/
			
		void initCfg(const block::AppConfig &cfg);

		/*!
			创建一个Module
		**/
		template <typename T>
		void createModule(T *module);

		/*!
			动态创建一个Module
		**/
		block::ModuleID createDynamicModule(const std::string &moduleType);

		/*!
			删除一个Module 
		**/
		void deleteModule(block::ModuleID moduleID);

		/*!
			进程的主循环
		**/
		void run();

		/*!
			进程的退出逻辑
		**/
		void exit();

		LoggerPtr getLogger() { return logger_; }
		TimerPtr getTimer() { return timer_; }

	protected:

		void tick();

		template <typename T>
		void registModule(T *module, bool dynamic = false);

		void unregistModule(block::ModuleID module_id);

		//！ 临时先写在这里，未来如果支持分布式可能要放在其他地方生成，保证服务器集群唯一。
		int32_t makeModuleID();

		int64_t uuid();

		//void unregist_dynamic_module(uint32_t module_id);

		typedef std::tuple<uint32_t, std::function<void()>, std::function<void()>, std::function<void(Module*, bool)>, Module*> Frame;
		void pushFrame(uint64_t index, Frame frame);
		void popFrame();

	protected:
		void reactorRegist(block::ModuleID moduleID, block::EventID event);
		void reactorDispatch(block::ModuleID self, block::ModuleID target, block::EventID event, block::ArgsPtr args);

		LoggerPtr logger_ = nullptr;
		TimerPtr timer_ = nullptr;

	private:
		AppState state_;
		int sequence_ = 0;
		uint64_t start_time_ = getSystemTick();

		std::list<Module *> module_list_;
		std::multimap<uint64_t, std::pair<uint16_t, Module*>> exit_list_;

		std::unordered_map<std::string, int32_t> module_name_map_;

		std::multimap<uint64_t, Frame> halfway_frame_;

		std::unordered_map<block::EventID, MailBoxPtr> mailboxMap_;

		bool shutdown_;

		uint64_t cur_frame_;

		int32_t module_idx_;

		block::AppConfig cfg_;
	};

	template <typename T>
	void block::Application::createModule(T *module)
	{
		registModule(module);
	}

	template <typename T>
	void block::Application::registModule(T *module, bool dynamic /* = false */)
	{
		module_list_.push_back(module);
		
		if (!dynamic) {
			module->setID(makeModuleID());
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

#define APP block::Application::get_ref() 

#define TIMERPTR APP.getTimer()

#define WARN_LOG(content) \
	APP.getLogger()->WARN(content)	
#define WARN_FMTLOG(content, ...) \
	APP.getLogger()->WARN(content, ##__VA_ARGS__)

#define INFO_LOG(content) \
	APP.getLogger()->INFO(content)
#define INFO_FMTLOG(content, ...)\
	APP.getLogger()->INFO(content, ##__VA_ARGS__)

#define DEBUG_LOG(content)\
	APP.getLogger()->DEBUG(content)
#define DEBUG_FMTLOG(content, ...)\
	APP.getLogger()->DEBUG(content, ##__VA_ARGS__)

#define ERROR_LOG(content)\
	APP.getLogger()->ERR(content)
#define ERROR_FMTLOG(content, ...)\
	APP.getLogger()->ERR(content, ##__VA_ARGS__)

}
