#ifndef _GSF_MYSQL_PROXY_HEADER_
#define _GSF_MYSQL_PROXY_HEADER_

#include <core/module.h>

#include "mysqlConnect.h"
#include "redisConnect.h"
#include <queue>

namespace gsf
{
	namespace modules
	{
		class MysqlProxyModule
			: public gsf::Module
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
			
			void eInit(gsf::ModuleID target, gsf::ArgsPtr args);

			/*!
				获取一个实例
			**/
			void eLoad(gsf::ModuleID target, gsf::ArgsPtr args);
			/*!
				创建一个实例
			**/
			void eInsert(gsf::ModuleID target, gsf::ArgsPtr args);
			/*!
				更新一个实例
			**/
			void eUpdate(gsf::ModuleID target, gsf::ArgsPtr args);

			/*!
				执行一条sql语句
			**/
			void eExecSql(gsf::ModuleID target, gsf::ArgsPtr args);


			/*！
				开启灾备&cache功能后调用
			**/
			void onTimer(gsf::ModuleID target, gsf::ArgsPtr args);

		private:

			//! 开启redis
			bool useCache_ = false;
			
			enum TimerType
			{
				tt_rewrite,
				tt_command
			};
			const uint32_t executeRewriteDelay_ = 1000 * 60 * 10;
			const uint32_t executeCommandDelay_ = 1000; //ms

			gsf::ModuleID timerM_ = gsf::ModuleNil;

			RedisPtr redisPtr_ = nullptr;

			struct CallbackInfo
			{
				gsf::ModuleID target_ = gsf::ModuleNil;
				gsf::ArgsPtr ptr_ = nullptr;
			};
			typedef std::queue<CallbackInfo*> CallbackQueue;

			CallbackQueue queue_;

			MysqlConnect conn_;
		};
	}
}

#endif // !_GSF_MYSQL_PROXY_HEADER_
