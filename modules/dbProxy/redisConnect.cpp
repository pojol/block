#include "redisConnect.h"

#ifdef WIN32
#include <winsock.h>
#endif // WIN32

#include <fmt/format.h>
#include <core/application.h>

bool gsf::modules::RedisProxy::init()
{
	using namespace std::placeholders;

	struct timeval _timeout = { 1, 500000 };

	redis_context_ = redisConnectWithTimeout(ip_.c_str(), port_, _timeout);
	if (redis_context_) {
		resume_redis_handler();

		is_open_ = true;
	}
	else {
		APP.ERR_LOG("RedisCacheProxyModule", "event_redis_connect err");
	}

	return is_open_;
}


void gsf::modules::RedisProxy::uninit()
{
	redis_command_count_ = 0;
	is_open_ = false;
	field_set_.clear();

	flush_redis_handler();

	redisFree(redis_context_);
}


void gsf::modules::RedisProxy::event_redis_avatar_offline(gsf::ArgsPtr args, gsf::CallbackFunc callback /* = nullptr */)
{
	if (!checkConnect()) {
		APP.WARN_LOG("RedisProxy", "connect fail!");
		return;
	}

	auto _field = args->pop_string();
	auto _key = args->pop_string();

	redisReply *_replay_ptr;
	_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "hdel %s %s", _field.c_str(), _key.c_str()));

	assert(_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR);
	
	if (_replay_ptr->type == REDIS_REPLY_ERROR) {
		APP.ERR_LOG("RedisCacheProxyModule", "event_redis_avatar_offline", " {}", _replay_ptr->str);
	}

	freeReplyObject(_replay_ptr);
}

void gsf::modules::RedisProxy::command(const std::string &field, const std::string &key, const std::string &block)
{
	field_set_.insert(field);	//记录下域

	if (REDIS_OK != redisAppendCommand(redis_context_, "hset %s %s %s", field.c_str(), key.c_str(), block.c_str())) {
		APP.ERR_LOG("RedisCacheProxyModule", "redisAppendCommand fail");
		return;
	}

	redis_command_count_++;
}

bool gsf::modules::RedisProxy::checkConnect()
{
	if (!is_open_) {
		APP.ERR_LOG("RedisCacheProxyModule", "service terminated! check_connect");
		return false;
	}

	redisReply *_replay_ptr;
	_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "ping"));

	if (_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR) {
		return true;
	}
	else {	// 试着重连一下
		APP.WARN_LOG("RedisCacheProxyModule", "connect fail! try reconnect!");

		struct timeval _timeout = { 1, 500000 };

		redisFree(redis_context_);

		redis_context_ = redisConnectWithTimeout(ip_.c_str(), port_, _timeout);
		if (redis_context_) {
			return true;
		}
		else {
			return false;
		}
	}
}

void gsf::modules::RedisProxy::execCommand()
{
	if (!checkConnect()) {
		return;
	}

	if (redis_command_count_ == 0) return;

	redisReply *_replay_ptr;
	for (uint32_t i = 0; i < redis_command_count_; ++i)
	{
		if (REDIS_OK != redisGetReply(redis_context_, (void**)&_replay_ptr)) {
			APP.ERR_LOG("RedisCacheProxyModule", "redisGetReply fail!");
		}

		freeReplyObject(_replay_ptr);
	}

	redis_command_count_ = 0;
}

void gsf::modules::RedisProxy::execRewrite()
{
	if (!checkConnect()) {
		return;
	}

	redisReply *_replay_ptr;
	_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "bgrewriteaof"));

	assert(_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR);

	if (_replay_ptr->type == REDIS_REPLY_ERROR) {
		APP.ERR_LOG("RedisCacheProxyModule", "rewrite_handler", " {}", _replay_ptr->str);
	}

	freeReplyObject(_replay_ptr);
}

void gsf::modules::RedisProxy::resume_redis_handler()
{
	//把redis内的数据分发出去， 由具体的avatar模块监听初始化。 这边要考虑数据极其大的情况!

	redisReply *_replay_ptr;

	for (auto &it : field_set_)
	{
		_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "hgetall %s", it.c_str()));

		for (size_t i = 0; i < _replay_ptr->elements; i += 2)
		{
			auto key_ = _replay_ptr->element[i]->str;
			auto val_ = _replay_ptr->element[i + 1]->str;

			//boardcast(eid::db_proxy::redis_resume, gsf::makeArgs(it, key_, val_));
		}

		freeReplyObject(_replay_ptr);

	}
}

void gsf::modules::RedisProxy::flush_redis_handler()
{
	redisReply *_replay_ptr;

	_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "flushall"));

	assert(_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR);

	if (_replay_ptr->type == REDIS_REPLY_ERROR) {
		APP.ERR_LOG("RedisCacheProxyModule", "flush_redis_handler", " {}", _replay_ptr->str);
	}

	freeReplyObject(_replay_ptr);
}

