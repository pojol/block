#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <sys/types.h>

#include <iostream>
#include <random>
#include <tuple>

#include <core/application.h>
#include <core/event.h>

#include <timer/timer.h>
#include <log/log.h>

#if defined(WIN32)
	#include <windows.h>
#else
	#include <unistd.h>
#endif



class TestClickModule
        : public gsf::Module
        , public gsf::IEvent
{
public:
	TestClickModule()
		: Module("TestClickModule")
	{}

	void before_init()
	{
		dispatch(eid::app_id, eid::get_module, gsf::Args("TimerModule"), [&](const gsf::Args &args) {
			timer_m_ = args.pop_uint32(0);
		});

		dispatch(eid::app_id, eid::get_module, gsf::Args("LogModule"), [&](const gsf::Args &args) {
			log_m_ = args.pop_uint32(0);
		});

		dispatch(log_m_, eid::log::log_callback, gsf::Args(), [&](const gsf::Args &args) {
			log_f_ = args.pop_log_callback(0);
		});
	}

	void init()
	{
		auto _create = 100;
		_surplus = _create;

		for (int i = 0; i < _create; ++i)
		{
			dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::Args(get_module_id(), uint32_t(i * 100)), [&](const gsf::Args &args) {
				uint64_t _time_id = args.pop_uint64(0);
				//log_f_(eid::log::info, "TestClickModule", gsf::make_args("regist time ", args->pop_uint64(0)));

				if (idx_ > 3 && idx_ < 7) {
					dispatch(timer_m_, eid::timer::remove_timer, gsf::Args(get_module_id(), _time_id), [&](const gsf::Args &args) {
						_surplus--;
					});
				}

				idx_++;
			});
		}

		listen(this, eid::timer::timer_arrive, [&](const gsf::Args &args, gsf::CallbackFunc callback) {
			_surplus--;
			log_f_(eid::log::info, "TestClickModule", gsf::Args("arrive time ", args.pop_uint64(0), " surplus ", _surplus));
		});
	}

private:
	uint32_t timer_m_;

	uint32_t log_m_;
	gsf::LogFunc log_f_;

	uint32_t idx_ = 0;
	uint32_t _surplus = 0;
};


int main()
{
	gsf::Application app;
	gsf::AppConfig cfg;
	cfg.is_watch_pref = true;
	app.init_cfg(cfg);

	app.regist_module(new gsf::modules::LogModule);
	app.regist_module(new gsf::modules::TimerModule);
	app.regist_module(new TestClickModule);

	app.run();

	return 0;
}
