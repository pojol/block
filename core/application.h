#pragma once

#include "appImpl.hpp"

namespace block
{
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
		block::ModuleID getModule(const std::string &moduleName) const;

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
			创建一个Module,(这个函数只能作用于app.run 之前
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

		utils::Logger getLogger();
		utils::Timer getTimer();

	protected:
		void reactorRegist(block::ModuleID moduleID, block::EventID event);
		void reactorDispatch(block::ModuleID self, block::ModuleID target, block::EventID event, block::ArgsPtr args);

	private:
		AppImpl impl_;
	};

	template <typename T>
	void block::Application::createModule(T *module)
	{
		impl_.createModule(module);
	}

#define APP block::Application::get_ref() 

#define TIMER APP.getTimer()

#define WARN_LOG(content) \
	APP.getLogger().WARN(content)	
#define WARN_FMTLOG(content, ...) \
	APP.getLogger().WARN(content, ##__VA_ARGS__)

#define INFO_LOG(content) \
	APP.getLogger().INFO(content)
#define INFO_FMTLOG(content, ...)\
	APP.getLogger().INFO(content, ##__VA_ARGS__)

#define DEBUG_LOG(content)\
	APP.getLogger().DEBUG(content)
#define DEBUG_FMTLOG(content, ...)\
	APP.getLogger().DEBUG(content, ##__VA_ARGS__)

#define ERROR_LOG(content)\
	APP.getLogger().ERR(content)
#define ERROR_FMTLOG(content, ...)\
	APP.getLogger().ERR(content, ##__VA_ARGS__)

}
