#include "redis_cache_proxy.h"


void gsf::modules::RedisCacheProxyModule::before_init()
{
	using namespace std::placeholders;

	dispatch(eid::app_id, eid::get_module, gsf::Args("TimerModule"), [&](const gsf::Args &args) {
		timer_m_ = args.pop_int32(0);
	});

	dispatch(eid::app_id, eid::get_module, gsf::Args("LogModule"), [&](const gsf::Args &args) {
		log_m_ = args.pop_int32(0);
	});

	dispatch(log_m_, eid::log::log_callback, gsf::Args(), [&](const gsf::Args &args) {
		log_f_ = args.pop_log_callback(0);
	});

	listen(this, eid::db_proxy::redis_connect
		, std::bind(&RedisCacheProxyModule::event_redis_connect, this, _1, _2));

	listen(this, eid::module_init_succ, std::bind(&RedisCacheProxyModule::start_update_redis_timer, this, _1, _2));
}


void gsf::modules::RedisCacheProxyModule::init()
{

}

void gsf::modules::RedisCacheProxyModule::shut()
{
	flush_redis_handler();
}

void gsf::modules::RedisCacheProxyModule::event_redis_connect(const gsf::Args &args, gsf::CallbackFunc callback)
{
	using namespace std::placeholders;

	std::string _ip = args.pop_string(0);
	int _port = args.pop_int32(1);

	if (redis_conn_.connect(_ip.c_str(), _port, nullptr, 1)) {
		resume_redis_handler();

		is_open_ = true;

		listen(this, eid::db_proxy::redis_command_callback, [&](const Args& args, gsf::CallbackFunc callback) {
			auto _args = gsf::Args();
			_args.push_redis_cmd_callback(std::bind(&RedisCacheProxyModule::event_redis_command, this, _1, _2, _3, _4));

			callback(_args);
		});

		boardcast(eid::module_init_succ, gsf::Args(get_module_id()));
	}
	else {

	}
}


void gsf::modules::RedisCacheProxyModule::event_redis_command(const std::string &cmd, const std::string &key, char *block, int len)
{
	aredis::redis_command _cmd;
	_cmd.cmd(cmd, key, block);
	redis_cmd_.add(_cmd);
}

void gsf::modules::RedisCacheProxyModule::start_update_redis_timer(const gsf::Args &args, gsf::CallbackFunc callback)
{
	ModuleID _module_id = args.pop_int32(0);

	listen(this, eid::timer::timer_arrive, [&](const gsf::Args &args, gsf::CallbackFunc cb) {
		gsf::TimerID _tid = args.pop_uint64(0);

		if (_tid == cmd_timer_id_) {
			cmd_handler();
		
			dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::Args(get_module_id(), redis_delay_time_), [&](const gsf::Args &args) {
				cmd_timer_id_ = args.pop_int32(0);
			});
		}

		if (_tid == rewrite_timer_id_) {
			rewrite_handler();

			dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::Args(get_module_id(), redis_rewrite_time_), [&](const gsf::Args &args) {
				rewrite_timer_id_ = args.pop_int32(0);
			});
		}
	});

	if (_module_id == timer_m_) {

		dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::Args(get_module_id(), redis_delay_time_), [&](const gsf::Args &args) {
			cmd_timer_id_ = args.pop_int32(0);
		});

		dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::Args(get_module_id(), redis_rewrite_time_), [&](const gsf::Args &args) {
			rewrite_timer_id_ = args.pop_int32(0);
		});
	}
}

void gsf::modules::RedisCacheProxyModule::cmd_handler()
{
	if (!is_open_) return;
	if (redis_cmd_.count == 0) return;

	redis_conn_.command(redis_cmd_);
	if (redis_conn_.reply(redis_result_)) {

	}

	redis_cmd_.clear();
}

void gsf::modules::RedisCacheProxyModule::rewrite_handler()
{
	if (!is_open_) return;

	aredis::redis_command _cmd;

	// for list 
	// cmd llen
	// cmd.lpop to len = 1

	_cmd.cmd("bgrewriteaof");

	if (redis_conn_.reply(redis_result_)) {

	}
}

void gsf::modules::RedisCacheProxyModule::resume_redis_handler()
{
	//把redis内的数据分发出去， 由具体的avatar模块监听初始化
}

void gsf::modules::RedisCacheProxyModule::flush_redis_handler()
{
	aredis::redis_command _cmd;

	_cmd.cmd("flushall");

	if (redis_conn_.reply(redis_result_)) {

	}
}
