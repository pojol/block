#include "redis_cache_proxy.h"

#ifdef WIN32
#include <winsock.h>
#endif // WIN32

#include <fmt/format.h>
#include <core/application.h>

void gsf::modules::RedisCacheProxyModule::before_init()
{
	using namespace std::placeholders;

	timer_m_ = APP.get_module("TimerModule");
	assert(timer_m_ != gsf::ModuleNil);
	
	log_m_ = APP.get_module("LogModule");
	assert(log_m_ != gsf::ModuleNil);

	listen(this, eid::db_proxy::redis_connect
		, std::bind(&RedisCacheProxyModule::event_redis_connect, this, _1));

	listen(this, eid::module_init_succ, std::bind(&RedisCacheProxyModule::start_update_redis_timer, this, _1));
}


void gsf::modules::RedisCacheProxyModule::init()
{

}

void gsf::modules::RedisCacheProxyModule::shut()
{
	redis_command_count_ = 0;
	is_open_ = false;
	field_set_.clear();

	flush_redis_handler();

	redisFree(redis_context_);
}

gsf::ArgsPtr gsf::modules::RedisCacheProxyModule::event_redis_connect(const gsf::ArgsPtr &args)
{
	using namespace std::placeholders;

	std::string _ip = args->pop_string();
	int _port = args->pop_i32();
	struct timeval _timeout = { 1, 500000 };

	ip_ = _ip;
	port_ = _port;

	redis_context_ = redisConnectWithTimeout(ip_.c_str(), port_, _timeout);
	if (redis_context_) {
		resume_redis_handler();

		is_open_ = true;

		listen(this, eid::db_proxy::redis_command, std::bind(&RedisCacheProxyModule::event_redis_command, this, _1));

		listen(this, eid::db_proxy::redis_avatar_offline
			, std::bind(&RedisCacheProxyModule::event_redis_avatar_offline, this, _1));

		boardcast(eid::module_init_succ, gsf::make_args(get_module_id()));
	}
	else {
		dispatch(log_m_, eid::log::print, gsf::log_error("RedisCacheProxyModule", " event_redis_connect err"));
	}

	return nullptr;
}

gsf::ArgsPtr gsf::modules::RedisCacheProxyModule::event_redis_avatar_offline(const gsf::ArgsPtr &args)
{
	if (!check_connect()) {
		return gsf::make_args(false);
	}

	auto _field = args->pop_string();
	auto _key = args->pop_string();

	redisReply *_replay_ptr;
	_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "hdel %s %s", _field.c_str(), _key.c_str()));

	assert(_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR);
	
	if (_replay_ptr->type == REDIS_REPLY_ERROR) {
		dispatch(log_m_, eid::log::print, gsf::log_error("RedisCacheProxyModule", fmt::format("event_redis_avatar_offline {}", _replay_ptr->str)));
	}

	freeReplyObject(_replay_ptr);

	return gsf::make_args(true);
}

gsf::ArgsPtr gsf::modules::RedisCacheProxyModule::event_redis_command(const gsf::ArgsPtr &args)
{
	auto _field = args->pop_string();
	auto _key = args->pop_string();
	auto _block = args->pop_string();	//need slice

	field_set_.insert(_field);	//记录下域

	if (REDIS_OK != redisAppendCommand(redis_context_, "hset %s %s %s", _field.c_str(), _key.c_str(), _block.c_str())) {
		dispatch(log_m_, eid::log::print, gsf::log_error("RedisCacheProxyModule", "redisAppendCommand fail!"));
		return gsf::make_args(false);
	}

	redis_command_count_++;

	return gsf::make_args(true);
}

gsf::ArgsPtr gsf::modules::RedisCacheProxyModule::start_update_redis_timer(const gsf::ArgsPtr &args)
{
	ModuleID _module_id = args->pop_i32();

	listen(this, eid::timer::timer_arrive, [&](const gsf::ArgsPtr &args) {
		gsf::TimerID _tid = args->pop_ui64();

		if (_tid == cmd_timer_id_) {
			cmd_handler();
		
			cmd_timer_id_ = dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::make_args(get_module_id(), redis_delay_time_))->pop_moduleid();
		}

		if (_tid == rewrite_timer_id_) {
			rewrite_handler();

			rewrite_timer_id_ = dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::make_args(get_module_id(), redis_rewrite_time_))->pop_moduleid();
		}

		return nullptr;
	});

	if (_module_id == timer_m_) {

		cmd_timer_id_ = dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::make_args(get_module_id(), redis_delay_time_))->pop_moduleid();

		rewrite_timer_id_ = dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::make_args(get_module_id(), redis_rewrite_time_))->pop_moduleid();
	}

	return nullptr;
}

bool gsf::modules::RedisCacheProxyModule::check_connect()
{
	if (!is_open_) {
		dispatch(log_m_, eid::log::print, gsf::log_error("RedisCacheProxyModule", "service terminated! check_connect"));
		return false;
	}

	redisReply *_replay_ptr;
	_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "ping"));

	if (_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR) {
		return true;
	}
	else {	// 试着重连一下
		dispatch(log_m_, eid::log::print, gsf::log_warring("RedisCacheProxyModule", "connect fail! try reconnect!"));

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

void gsf::modules::RedisCacheProxyModule::cmd_handler()
{
	if (!check_connect()) {
		return;
	}

	if (redis_command_count_ == 0) return;

	redisReply *_replay_ptr;
	for (int i = 0; i < redis_command_count_; ++i)
	{
		if (REDIS_OK != redisGetReply(redis_context_, (void**)&_replay_ptr)) {
			dispatch(log_m_, eid::log::print, gsf::log_error("RedisCacheProxyModule", "redisGetReply fail!"));
		}

		freeReplyObject(_replay_ptr);
	}

	redis_command_count_ = 0;
}

void gsf::modules::RedisCacheProxyModule::rewrite_handler()
{
	if (!check_connect()) {
		return;
	}

	redisReply *_replay_ptr;
	_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "bgrewriteaof"));

	assert(_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR);

	if (_replay_ptr->type == REDIS_REPLY_ERROR) {
		dispatch(log_m_, eid::log::print, gsf::log_error("RedisCacheProxyModule", fmt::format("rewrite_handler {}", _replay_ptr->str)));
	}

	freeReplyObject(_replay_ptr);
}

void gsf::modules::RedisCacheProxyModule::resume_redis_handler()
{
	//把redis内的数据分发出去， 由具体的avatar模块监听初始化。 这边要考虑数据极其大的情况!

	redisReply *_replay_ptr;

	for (auto &it : field_set_)
	{
		_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "hgetall %s", it.c_str()));

		for (int i = 0; i < _replay_ptr->elements; i += 2)
		{
			auto key_ = _replay_ptr->element[i]->str;
			auto val_ = _replay_ptr->element[i + 1]->str;

			boardcast(eid::db_proxy::redis_resume, gsf::make_args(it, key_, val_));
		}

		freeReplyObject(_replay_ptr);

	}
}

void gsf::modules::RedisCacheProxyModule::flush_redis_handler()
{
	redisReply *_replay_ptr;

	_replay_ptr = static_cast<redisReply*>(redisCommand(redis_context_, "flushall"));

	assert(_replay_ptr && _replay_ptr->type != REDIS_REPLY_ERROR);

	if (_replay_ptr->type == REDIS_REPLY_ERROR) {
		dispatch(log_m_, eid::log::print, gsf::log_error("RedisCacheProxyModule", fmt::format("flush_redis_handler {}", _replay_ptr->str)));
	}

	freeReplyObject(_replay_ptr);
}
