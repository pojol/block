#ifndef _REDIS_CACHE_PROXY_HEADER_
#define _REDIS_CACHE_PROXY_HEADER_

#include <core/module.h>
#include <core/event.h>

#include <set>

#include "hiredis.h"

namespace gsf
{
	namespace modules 
	{
		// 在 redis config 文件中
		// 找到 SNAPSHOTTING 模块，打开 save "" 关闭rdb文件
		// 找到 APPEND ONLY MODE 模块
		//    appendonly yes 打开aof
		//    appendfilename "" 设置aof文件名
		//    appendfsync 同步方式选择 everysec
		//    no-appendfsync-on-rewrite 设置为 no ，保证数据安全。 如果设置为yes在子进程保存的时候会阻塞redis
		//    rewrite
		//    依据客户端的 BGREWRITEAOF 命令， 如果redis正在进行aof 则等待完成之后执行，反正则立即执行。
		//    #auto-aof-rewrite-percentage 100 关闭自动rewrite
		//    #auto - aof - rewrite - min - size 64mb 关闭自动rewrite


		class RedisCacheProxyModule
			: public gsf::Module
			, public gsf::IEvent
		{
		public:

			void before_init() override;

			void init() override;

			void shut() override;

		private:

			void event_redis_connect(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			void event_redis_command(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			void event_redis_avatar_offline(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);

			void start_update_redis_timer(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			
			bool check_connect();
			
			void cmd_handler();
			void rewrite_handler();

			// 服务器异常关闭时恢复数据到内存
			void resume_redis_handler();
			// 服务器正常退出时调用
			void flush_redis_handler();

			uint32_t timer_m_;
			uint32_t log_m_;

		private:

			std::string ip_ = "";
			uint32_t port_ = 0;

			bool is_open_ = false;

			gsf::TimerID cmd_timer_id_ = 0;
			const uint32_t redis_delay_time_ = 1000;	//ms
			gsf::TimerID rewrite_timer_id_ = 0;
			const uint32_t redis_rewrite_time_ = 1000 * 60 * 10;

			redisContext *redis_context_;
			uint32_t redis_command_count_ = 0;

			std::set<std::string> field_set_;
		};
	}
}

#endif
