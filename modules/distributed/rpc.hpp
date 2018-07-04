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

        struct RpcRedisReply
        {
            RpcRedisReply(redisReply *reply)
                : reply_(reply) {

            }

            ~RpcRedisReply() {
                freeReplyObject(reply_);
            }

            bool check(const std::string &flag) {
                assert(reply_ && reply_->type != REDIS_REPLY_ERROR);

                if (reply_->type == REDIS_REPLY_ERROR) {
                    ERROR_FMTLOG("[BLOCK] RedisReply cmd : {} err : {}", flag , reply_->str);
                    return false;
                }

                return true;
            }

            int type() {
                return reply_->type;
            }

            int64_t integer() {
                return reply_->integer;
            }

            std::string str() {
                std::string _t(reply_->str, reply_->len);
                return _t;
            }

            redisReply * reply_;
        };
        typedef std::shared_ptr<RpcRedisReply> RpcReplyPtr;

        /**!
         * 拥塞检查
         * 连接检查
         * rpc 管道|消息 管理
        */

       //! 注： 如果rpc服务单机压力过大，也是可以进行水平分片的。 （但估计这种可能非常小

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

            RpcReplyPtr command(redisContext *c, const char *format, ...);

        private:
            int32_t timeout_ = 1000;
            std::string redisIp_ = "";
            int32_t redisPort_ = 0;
            int32_t maxlen_ = 1024;

            redisContext *redis_context_ = nullptr;
		};

	}
}
