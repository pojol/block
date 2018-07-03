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
			获取App的名称
		**/
		std::string getName() const;

		/*!
			通过名字获取某个Module的实例ID， 仅支持静态创建的Module。
		**/
		block::ModuleID getModuleID(const std::string &moduleName) const;

		/*!
		* 	判断app中是否存在该module
 		* */
		bool hasModule(block::ModuleID moduleid);

		/*!
			获取App的配置结构
		**/
		const block::AppConfig & getAppCfg();

		/*!
			获取一个uuid
		**/
		int64_t getUUID();

		/*!
			获取app当前的毫秒数
		**/
		uint64_t getSystemTick();

		/*!
			初始化配置文件
		**/
		void initCfg(const block::AppConfig &cfg);

		/*!
			创建一个静态的Module （只能在App.run之前调用
		**/
		template <typename T>
		void createModule(T *module);

		/*!
			创建一个动态的Module
		**/
		block::ModuleID createDynamicModule(const std::string &moduleType);

		/*!
			从App中删除一个Module
		**/
		void deleteModule(block::ModuleID moduleID);

		/*!
			App的主循环
		**/
		void run();

		/*!
			App退出函数
		**/
		void exit();

		utils::Logger getLogger();
		utils::Timer & getTimer();

	protected:
		void reactorRegist(block::ModuleID moduleID, block::EventID event);
		void reactorDispatch(block::ModuleID self, block::ModuleID target, block::EventID event, block::ArgsPtr args);
		void reactorBoardcast(block::ModuleID self, block::EventID event, block::ArgsPtr args);
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
