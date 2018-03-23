#ifndef _REDIS_CACHE_PROXY_HEADER_
#define _REDIS_CACHE_PROXY_HEADER_

#include <core/module.h>

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


		class RedisProxy
		{
		public:

			bool init();
			void uninit();

			void command(const std::string &field, const std::string &key, const std::string &block);

			void event_redis_avatar_offline(gsf::ArgsPtr args, gsf::CallbackFunc callback = nullptr);
			
			/*!
				检查连接是否存在，如果断开则尝试重连。
			**/
			bool checkConnect();
			
			void execCommand();
			void execRewrite();

			// 服务器异常关闭时恢复数据到内存
			void resume_redis_handler();
			// 服务器正常退出时调用
			void flush_redis_handler();

			uint32_t timerM_;

		private:

			std::string ip_ = "127.0.0.1";
			uint32_t port_ = 3306;

			bool is_open_ = false;

			redisContext *redis_context_;
			uint32_t redis_command_count_ = 0;

			//! 模块的集合
			std::set<std::string> field_set_;
		};

		typedef std::shared_ptr<RedisProxy> RedisPtr;
	}
	
}

#endif
