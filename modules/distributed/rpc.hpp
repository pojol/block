#pragma once

#include <core/module.h>

#include <vector>
#include <map>
#include <tuple>
#include <functional>

#include <fmt/format.h>
#include <core/application.h>

#include "hiredis.h"

namespace block
{
	namespace modules
	{
         /**!
         * 拥塞检查
         * 连接检查
         * rpc 管道|消息 管理
        */

		class RPCModule
			: public block::Module
		{
		public:

			RPCModule();
			virtual ~RPCModule();

			void before_init() override;
			void init() override;
			void execute() override;
			void shut() override;

		protected:

            void eSubscription(block::ModuleID target, block::ArgsPtr args);
            void eRpc(block::ModuleID target, block::ArgsPtr args);

        protected:
            /*!
				检查连接是否存在，如果断开则尝试重连。
			**/
			bool checkConnect();

            redisReply * command(redisContext *c, const char *format, ...);

            typedef std::vector<std::pair<std::string, std::vector<std::string>>> ObserverVec;
            ObserverVec observerVec_;

        private:
            int32_t timeout_ = 1000;
            std::string redisIp_ = "";
            int32_t redisPort_ = 0;
            int32_t maxlen_ = 1024;

            redisContext *redis_context_ = nullptr;
		};

	}
}
