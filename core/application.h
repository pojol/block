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
			鑾峰緱杩涚▼鐨勫悕绉?
		**/
		std::string getName() const;

		/*!
			閫氳繃鍚嶅瓧鑾峰彇鏌愪釜Module鐨勫疄渚婭D锛?浠呮敮鎸侀潤鎬佸垱寤虹殑Module銆?
		**/
		block::ModuleID getModuleID(const std::string &moduleName) const;

		/*!
			鑾峰彇杩涚▼鐨勬満鍣↖D
		**/
		const block::AppConfig & getAppCfg();

		/*!
			鑾峰緱鍦ㄩ泦缇ゅ唴浜х敓鐨勫敮涓€ID
		**/
		int64_t getUUID();

		/*!
			鑾峰緱褰撳墠鐨勬绉掓暟
		**/
		uint64_t getSystemTick();

		/*!
			鍒濆鍖栬繘绋?
		**/
		void initCfg(const block::AppConfig &cfg);

		/*!
			鍒涘缓涓€涓狹odule,(杩欎釜鍑芥暟鍙兘浣滅敤浜巃pp.run 涔嬪墠
		**/
		template <typename T>
		void createModule(T *module);

		/*!
			鍔ㄦ€佸垱寤轰竴涓狹odule
		**/
		block::ModuleID createDynamicModule(const std::string &moduleType);

		/*!
			鍒犻櫎涓€涓狹odule 
		**/
		void deleteModule(block::ModuleID moduleID);

		/*!
			杩涚▼鐨勪富寰幆
		**/
		void run();

		/*!
			杩涚▼鐨勯€€鍑洪€昏緫
		**/
		void exit();

		utils::Logger getLogger();
		utils::Timer & getTimer();

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
