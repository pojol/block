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

namespace block
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

		struct BlockRedisReply
		{
			BlockRedisReply(redisReply *reply)
				: reply_(reply) {

			}

			~BlockRedisReply() {
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

		typedef std::shared_ptr<BlockRedisReply> ReplyPtr;

		/**!
			hash 用于保存entity实例
			list 用于存放entity的更新操作
			在mergeTimeSpace_ 和 mergeCountSpace_ 两个值到达预设的时候会合并list中的更新操作，覆盖到hash

			这里序列化的方法 以及 合并数据的方法应该由外部提供。
		*/
		template <typename T>
		class RedisConnect
		{
		public:

			bool init();
			void uninit();

			/**!
				将对象添加到缓存中，时间到达或者次数到达则写入到数据库
			*/
			bool push(const std::string &field, uint32_t key, const std::string &buf);

			/**!
			 * 获得缓存中的实例, 会自动执行一次合并以保证获取到最新的实例.
			*/
			std::string get(const std::string &field, uint32_t key);
			std::vector<std::pair<uint32_t, std::string>> getAll();

			/**!
				从缓存中清除某个对象
			*/
			bool remove(const std::string &key);

			/**!
				在进程的主循环中调用
			*/
			void run();

			/**！
				服务器正常退出时调用， 清空redis
			*/ 
			void flush_redis_handler();

		protected:

			/*!
				检查连接是否存在，如果断开则尝试重连。
			**/
			bool checkConnect();

			/**!
				重刷aof文件，体积优化
			*/
			void refresh();

			std::pair<uint32_t, std::string> pop(uint32_t key);

			int getListLen(uint32_t key);

			ReplyPtr command(redisContext *c, const char *format, ...);

			void merge(uint32_t key);

		private:

			std::string ip_ = "127.0.0.1";
			uint32_t port_ = 6379;

			//! 判断连接是否正常
			bool is_conn_ = false;

			redisContext *redis_context_;
			uint32_t pipeLineCount_ = 0;
			std::set<uint32_t> keys_;
			
			std::string field_ = "";
			uint32_t updateCount_ = 0;
			uint64_t updateTime_ = APP.getSystemTick();
			uint64_t refreshTime_ = APP.getSystemTick();

			const uint32_t mergeTimeSpace_ = 1000 * 60 * 2;
			const uint32_t mergeCountSpace_ = 100;
			const uint32_t refreshSpace_ = 1000 * 60 * 10;
		};

		template <typename T>
		ReplyPtr block::modules::RedisConnect<T>::command(redisContext *c, const char *format, ...)
		{
			va_list ap;
			void *reply = NULL;
			va_start(ap, format);
			reply = redisvCommand(c, format, ap);
			va_end(ap);

			return std::make_shared<BlockRedisReply>(static_cast<redisReply*>(reply));
		}

		template <typename T>
		bool block::modules::RedisConnect<T>::init()
		{
			using namespace std::placeholders;

			struct timeval _timeout = { 1, 500000 };

			redis_context_ = redisConnectWithTimeout(ip_.c_str(), port_, _timeout);
			if (redis_context_) {
				is_conn_ = true;
			}
			else {
				ERROR_LOG("[BLOCK] RedisConnect init fail");
			}

			return is_conn_;
		}

		template <typename T>
		void block::modules::RedisConnect<T>::uninit()
		{
			updateCount_ = 0;
			pipeLineCount_ = 0;
			is_conn_ = false;

			flush_redis_handler();

			redisFree(redis_context_);
		}

		template <typename T>
		bool block::modules::RedisConnect<T>::push(const std::string &field, uint32_t key, const std::string &buf)
		{
			bool _flag = false;

			do{
				if (field_ != "" && field_ != field){
					ERROR_LOG("[BLOCK] RedisConnect push field cannot be changed!");
					break;
				}

				if (!checkConnect()){
					break;
				}

				std::string _keystr = std::to_string(key);

				auto hgetReply = command(redis_context_, "HGET %s %s", field.c_str(), _keystr.c_str());
				if (!hgetReply->check("hget")) {
					break;
				}

				if (hgetReply->type() == REDIS_REPLY_NIL){

					auto hsetReplay = command(redis_context_, "HSET %s %s %b", field.c_str(), _keystr.c_str(), buf.c_str(), buf.length());
					if (!hsetReplay->check("hset")) {
						break;
					}
				}
				else {
					if (REDIS_OK != redisAppendCommand(redis_context_, "RPUSH %s %b", _keystr.c_str(), buf.c_str(), buf.length()))
					{
						ERROR_LOG("[BLOCK] RedisConnect push redis append command fail!");
					}

					pipeLineCount_++;
				}

				keys_.insert(key);
				field_ = field;
				_flag = true;
			} while (0);

			return _flag;
		}
	}

	template <typename T>
	std::string block::modules::RedisConnect<T>::get(const std::string &field, uint32_t key)
	{
		merge(key);

		std::string _keystr = std::to_string(key);

		auto replyPtr = command(redis_context_, "HGET %s %s", field.c_str(), _keystr.c_str());
		if (!replyPtr->check("get")) {
			return "";
		}

		if (replyPtr->type() == REDIS_REPLY_NIL) {
			return "";
		}

		return replyPtr->str();
	}


	template <typename T>
	std::vector<std::pair<uint32_t, std::string>> block::modules::RedisConnect<T>::getAll()
	{
		std::vector<std::pair<uint32_t, std::string>> _vec;

		for (auto key : keys_)
		{
			merge(key);

			std::string _keystr = std::to_string(key);

			auto replyPtr = command(redis_context_, "HGET %s %s", field_.c_str(), _keystr.c_str());
			if (!replyPtr->check("get all")) {
				break;
			}

			_vec.push_back(std::make_pair(key, _keystr));
		}

		return _vec;
	}
	
	template <typename T>
	void block::modules::RedisConnect<T>::run()
	{

		if (pipeLineCount_ == 0) {
			return;
		}

		if (!checkConnect()) {
			return;
		}

		redisReply *_replay_ptr;
		for (uint32_t i = 0; i < pipeLineCount_; ++i)
		{
			if (REDIS_OK != redisGetReply(redis_context_, (void**)&_replay_ptr)) {
				ERROR_LOG("[BLOCK] RedisConnect exec fail!");
			}

			freeReplyObject(_replay_ptr);
		}

		updateCount_ += pipeLineCount_;
		using namespace std::chrono;
		auto _curTime = APP.getSystemTick();
		if ((_curTime - updateTime_) > mergeTimeSpace_ || updateCount_ > mergeCountSpace_){

			//! 更新到hash
			for (auto key : keys_)
			{
				merge(key);
			}

			updateTime_ = _curTime;
			updateCount_ = 0;
		}

		if ((_curTime - refreshTime_) > refreshSpace_) {
			refresh();
		}

		pipeLineCount_ = 0;
	}

	template <typename T>
	std::pair<uint32_t, std::string> block::modules::RedisConnect<T>::pop(uint32_t key)
	{
		if (!checkConnect()) {
			return std::make_pair(0, "");
		}

		std::string _keystr = std::to_string(key);

		int _len = getListLen(key);
		if (_len == 0) {
			return std::make_pair(0, "");
		}

		T _t;

		for (int i = 0; i < _len; ++i)
		{
			auto replayPtr = command(redis_context_, "LPOP %s", _keystr.c_str());
			if (!replayPtr->check("pop")) {
				break;
			}

			if (i == 0) {
				_t.ParseFromString(replayPtr->str());
			}
			else {
				T _newer;
				_newer.ParseFromString(replayPtr->str());
				_t.MergeFrom(_newer);
			}
		}

		std::string _buf;
		int32_t _id = _t.id();
		_t.SerializeToString(&_buf);
		
		return std::make_pair(_id, _buf);
	}

	template <typename T>
	bool block::modules::RedisConnect<T>::checkConnect()
	{
		if (!is_conn_) {
			WARN_LOG("[BLOCK] RedisConnect service terminated! check_connect");
			return false;
		}

		auto replyPtr = command(redis_context_, "PING");

		if (replyPtr->check("ping")) {
			return true;
		}
		else {
			WARN_LOG("[BLOCK] RedisConnect connect fail! try reconnect!");

			struct timeval _timeout = { 1, 500000 };

			redisFree(redis_context_);

			redis_context_ = redisConnectWithTimeout(ip_.c_str(), port_, _timeout);
			if (redis_context_) {
				return true;
			}
			else {
				ERROR_LOG("[BLOCK] RedisConnect service terminated! can't connect!");
				return false;
			}
		}
	}

	template <typename T>
	void block::modules::RedisConnect<T>::refresh()
	{
		if (!checkConnect()) {
			return;
		}

		auto replyPtr = command(redis_context_, "bgrewriteaof");
		replyPtr->check("bgrewriteaof");
	}

	template <typename T>
	void block::modules::RedisConnect<T>::flush_redis_handler()
	{
		if (!checkConnect()) {
			return;
		}

		auto replyPtr = command(redis_context_, "flushall");
		replyPtr->check("flushall");
	}

	template <typename T>
	void block::modules::RedisConnect<T>::merge(uint32_t key)
	{
		auto _p = pop(key);
		if (_p.first != 0) {

			auto hsetReplay = command(redis_context_, "HSET %s %s %b", field_.c_str(), std::to_string(_p.first).c_str(), _p.second.c_str(), _p.second.length());
			hsetReplay->check("exec");
		}
	}

	template <typename T>
	int block::modules::RedisConnect<T>::getListLen(uint32_t key)
	{
		if (!checkConnect()) {
			return 0;
		}

		std::string _keystr = std::to_string(key);

		auto replyPtr = command(redis_context_, "LLEN %s", _keystr.c_str());
		if (!replyPtr->check("LLEN")) {
			return 0;
		}
		else {
			return replyPtr->integer();
		}

	}
}

#endif
