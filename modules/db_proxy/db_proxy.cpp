#include "db_proxy.h"


void gsf::modules::DBProxyModule::before_init()
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

	listen(this, eid::db_proxy::redis_command
		, std::bind(&DBProxyModule::event_redis_command, this, _1, _2));

	listen(this, eid::db_proxy::redis_connect
		, std::bind(&DBProxyModule::event_redis_connect, this, _1, _2));

	listen(this, eid::module_init_succ, std::bind(&DBProxyModule::start_update_redis_timer, this, _1, _2));
}


void gsf::modules::DBProxyModule::init()
{

}

void gsf::modules::DBProxyModule::event_redis_connect(const gsf::Args &args, gsf::CallbackFunc callback)
{
	std::string _ip = args.pop_string(0);
	int _port = args.pop_int32(1);

	if (redis_conn_.connect(_ip.c_str(), _port, nullptr, 1)) {

	}
	else {

	}
}


void gsf::modules::DBProxyModule::event_redis_command(const gsf::Args &args, gsf::CallbackFunc callback)
{

	redis_cmd_.cmd("rpush", "avatar.id", "123");

}

void gsf::modules::DBProxyModule::start_update_redis_timer(const gsf::Args &args, gsf::CallbackFunc callback)
{
	ModuleID _module_id = args.pop_int32(0);

	listen(this, eid::timer::timer_arrive, [&](const gsf::Args &args, gsf::CallbackFunc cb) {
		gsf::TimerID _tid = args.pop_uint64(0);

		if (_tid == sec_timer_id_) {
			sec_handler();
		
			dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::Args(get_module_id(), redis_delay_time_), [&](const gsf::Args &args) {
				sec_timer_id_ = args.pop_int32(0);
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
			sec_timer_id_ = args.pop_int32(0);
		});

		dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::Args(get_module_id(), redis_rewrite_time_), [&](const gsf::Args &args) {
			rewrite_timer_id_ = args.pop_int32(0);
		});
	}
}

void gsf::modules::DBProxyModule::sec_handler()
{

	redis_conn_.command(redis_cmd_);
	if (redis_conn_.reply(redis_result_)) {

	}

}

void gsf::modules::DBProxyModule::rewrite_handler()
{
	aredis::redis_command _cmd;

	// for list 
	// cmd llen
	// cmd.lpop to len = 1

	_cmd.cmd("bgrewriteaof");

	if (redis_conn_.reply(redis_result_)) {

	}
}
