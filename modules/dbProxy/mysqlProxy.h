#ifndef _GSF_MYSQL_PROXY_HEADER_
#define _GSF_MYSQL_PROXY_HEADER_

#include <core/module.h>
#include <core/event.h>

#include "mysqlConnect.h"
#include "redisConnect.h"
#include <queue>

namespace gsf
{
	namespace modules
	{
		class MysqlProxyModule
			: public gsf::Module
			, public gsf::IEvent
		{
		public:
			MysqlProxyModule();
			virtual ~MysqlProxyModule();

			MysqlProxyModule(const MysqlProxyModule &) = delete;
			MysqlProxyModule & operator = (const MysqlProxyModule &) = delete;

		protected:
			void before_init() override;

			void init() override;

			void execute() override;

			void shut() override;

			void after_shut() override;

		private:
			
			void eInit(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);

			/*!
				获取一个实例
			**/
			void eSelect(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			/*!
				创建一个实例
			**/
			void eInsert(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			/*!
				更新一个实例
			**/
			void eUpdate(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			/*!
				执行一条sql语句
			**/
			void eQuery(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);


			void onTimer(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);

		private:

			bool useCache_ = false;
			gsf::TimerID rewriteTimeID_ = gsf::TimerNil;
			const uint32_t executeRewriteDelay_ = 1000 * 60 * 10;
			
			gsf::TimerID commandTimeID_ = gsf::TimerNil;
			const uint32_t executeCommandDelay_ = 1000; //ms

			gsf::ModuleID timerM_ = gsf::ModuleNil;

			RedisPtr redisPtr_ = nullptr;

			struct CallbackInfo
			{
				gsf::ModuleID target_;
				gsf::ArgsPtr ptr_;
			};
			typedef std::queue<CallbackInfo*> CallbackQueue;

			CallbackQueue queue_;
			gsf::ModuleID logM_ = gsf::ModuleNil;

			MysqlConnect conn_;
		};
	}
}

#endif // !_GSF_MYSQL_PROXY_HEADER_
