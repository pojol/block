#ifndef _REDIS_CACHE_PROXY_HEADER_
#define _REDIS_CACHE_PROXY_HEADER_

#include <core/module.h>

#include <set>

#include "hiredis.h"

#ifdef WIN32
#include <winsock.h>
#endif // WIN32

#include <fmt/format.h>
#include <core/application.h>

namespace gsf
{
	namespace modules 
	{
		// rdb
		//	 在 redis config 文件中
		//   找到 SNAPSHOTTING 模块，打开 save "" 关闭rdb文件
		
		// aof
		//    找到 APPEND ONLY MODE 模块
		//	  appendonly yes 打开aof
		//    appendfilename "" 设置aof文件名
		//    appendfsync 同步方式选择 everysec
		//    no-appendfsync-on-rewrite 设置为 no ，保证数据安全。 如果设置为yes在子进程保存的时候会阻塞redis
		
		// refresh
		//    依据客户端的 BGREWRITEAOF 命令， 如果redis正在进行aof 则等待完成之后执行，反正则立即执行。
		//    #auto-aof-rewrite-percentage 100 关闭自动rewrite
		//    #auto - aof - rewrite - min - size 64mb 关闭自动rewrite

		// block
		/*!
			hash
			[k,v] = (entityID, entityBlob)
		
			list
			[k,v] = (entityID, updateBlob)
		**/

		template <typename T>
		class RedisConnect
		{
		public:

			bool init();
			void uninit();

			/**!
				将对象添加到缓存中，时间到达或者次数到达则写入到数据库
			*/
			bool push(const std::string &field, const std::string &key, const std::string &buf);

			/*!
				pipe line
			**/
			void exec();

			/**!
			写入到数据库
			*/
			std::string pop(const std::string &key);

			/**!
				离线处理
			*/
			void offline();

			/**!
			重刷aof文件，用于灾备
			*/
			void refresh();

			/**！
				服务器正常退出时调用， 清空redis
			*/ 
			void flush_redis_handler();

		protected:

			/*!
				检查连接是否存在，如果断开则尝试重连。
			**/
			bool checkConnect();

			int getListLen(const std::string &key);

		private:

			std::string ip_ = "127.0.0.1";
			uint32_t port_ = 3306;

			//! 判断连接是否正常
			bool is_conn_ = false;

			redisContext *redis_context_;
			uint32_t redis_command_count_ = 0;
		};

		template <typename T>
		bool gsf::modules::RedisConnect<T>::init()
		{
			using namespace std::placeholders;

			struct timeval _timeout = { 1, 500000 };

			redis_context_ = redisConnectWithTimeout(ip_.c_str(), port_, _timeout);
			if (redis_context_) {
				is_conn_ = true;
			}
			else {
				APP.ERR_LOG("RedisConnect", "event_redis_connect err");
			}

			return is_conn_;
		}

		template <typename T>
		void gsf::modules::RedisConnect<T>::uninit()
		{
			redis_command_count_ = 0;
			is_conn_ = false;

			flush_redis_handler();

			redisFree(redis_context_);
		}

		template <typename T>
		bool gsf::modules::RedisConnect<T>::push(const std::string &field, const std::string &key, const std::string &buf)
		{
			if (!checkConnect()) {
				return false;
			}

			
			//! 如果hash中不存在则创建，存在则添加到更新队列中

			redisReply *_replay_ptr;
			_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "HGET %s %s %s", field.c_str(), key.c_str()));

			assert(_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR);

			if (_replay_ptr->type == REDIS_REPLY_ERROR) {
				APP.ERR_LOG("RedisConnect", "rewrite_handler", " {}", _replay_ptr->str);
			}

			freeReplyObject(_replay_ptr);
			/*
				if (REDIS_OK != redisAppendCommand(redis_context_, "lpush %s%s", key.c_str(), buf.c_str())) {
				APP.ERR_LOG("RedisConnect", "redisAppendCommand fail");
				return false;
				}

				redis_command_count_++;
			*/
			return true;
		}
	}
	
	template <typename T>
	void gsf::modules::RedisConnect<T>::exec()
	{
		if (!checkConnect()) {
			return;
		}

		if (redis_command_count_ == 0) return;

		redisReply *_replay_ptr;
		for (uint32_t i = 0; i < redis_command_count_; ++i)
		{
			if (REDIS_OK != redisGetReply(redis_context_, (void**)&_replay_ptr)) {
				APP.ERR_LOG("RedisConnect", "exec fail!");
			}

			freeReplyObject(_replay_ptr);
		}

		redis_command_count_ = 0;
	}

	template <typename T>
	std::string gsf::modules::RedisConnect<T>::pop(const std::string &key)
	{
		if (!checkConnect()) {
			return "";
		}

		int _len = getListLen(key);

		T _t;

		for (int i = 0; i < _len; ++i)
		{
			redisReply *_replay_ptr;
			_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "lpop %s", key.c_str()));

			assert(_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR);

			if (_replay_ptr->type == REDIS_REPLY_ERROR) {
				APP.ERR_LOG("RedisConnect", "rewrite_handler", " {}", _replay_ptr->str);
			}

			if (i == 0) {
				_t.ParseFromArray(_replay_ptr->str, std::strlen(_replay_ptr->str));
			}
			else {
				T _newer;
				_newer.ParseFromArray(_replay_ptr->str, std::strlen(_replay_ptr->str));
				_t.MergeFrom(_newer);
			}

			freeReplyObject(_replay_ptr);
		}

		std::string _buf;
		_t.SerializeToString(&_buf);
		return _buf;
	}

	template <typename T>
	bool gsf::modules::RedisConnect<T>::checkConnect()
	{
		if (!is_conn_) {
			APP.WARN_LOG("RedisConnect", "service terminated! check_connect");
			return false;
		}

		redisReply *_replay_ptr;
		_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "ping"));

		if (_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR) {
			return true;
		}
		else {	// 试着重连一下
			APP.WARN_LOG("RedisConnect", "connect fail! try reconnect!");

			struct timeval _timeout = { 1, 500000 };

			redisFree(redis_context_);

			redis_context_ = redisConnectWithTimeout(ip_.c_str(), port_, _timeout);
			if (redis_context_) {
				return true;
			}
			else {
				APP.ERR_LOG("RedisConnect", "service terminated! can't connect!");
				return false;
			}
		}
	}

	template <typename T>
	void gsf::modules::RedisConnect<T>::refresh()
	{
		if (!checkConnect()) {
			return;
		}

		redisReply *_replay_ptr;
		_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "bgrewriteaof"));

		assert(_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR);

		if (_replay_ptr->type == REDIS_REPLY_ERROR) {
			APP.ERR_LOG("RedisConnect", "rewrite_handler", " {}", _replay_ptr->str);
		}

		freeReplyObject(_replay_ptr);
	}

	template <typename T>
	void gsf::modules::RedisConnect<T>::flush_redis_handler()
	{
		if (!checkConnect()) {
			return;
		}

		redisReply *_replay_ptr;

		_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "flushall"));

		assert(_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR);

		if (_replay_ptr->type == REDIS_REPLY_ERROR) {
			APP.ERR_LOG("RedisConnect", "flush_redis_handler", " {}", _replay_ptr->str);
		}

		freeReplyObject(_replay_ptr);
	}

	template <typename T>
	int gsf::modules::RedisConnect<T>::getListLen(const std::string &key)
	{
		if (!checkConnect()) {
			return 0;
		}

		redisReply *_replay_ptr;
		_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "llen %s", key.c_str()));

		assert(_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR);

		if (_replay_ptr->type == REDIS_REPLY_ERROR) {
			APP.ERR_LOG("RedisConnect", "rewrite_handler", " {}", _replay_ptr->str);
		}

		int _len = _replay_ptr->integer;
		freeReplyObject(_replay_ptr);

		return _len;
	}
}

#endif
