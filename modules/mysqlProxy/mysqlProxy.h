#ifndef _GSF_MYSQL_PROXY_HEADER_
#define _GSF_MYSQL_PROXY_HEADER_

#include <core/module.h>
#include <core/event.h>

#include "mysqlConnect.h"
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
			void eQuery(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			void eUpdate(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);

		private:

			struct CallbackInfo
			{
				gsf::ModuleID target_;
				gsf::ArgsPtr ptr_;
			};
			typedef std::queue<CallbackInfo*> CallbackQueue;

			CallbackQueue queue_;

			MysqlConnect conn_;
		};
	}
}

#endif // !_GSF_MYSQL_PROXY_HEADER_
