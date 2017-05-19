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
		dispatch(eid::app_id, eid::get_module, gsf::Args("TimerModule"), [&](gsf::Args args) {
			timer_m_ = args.pop_uint32(0);
		});

		dispatch(eid::app_id, eid::get_module, gsf::Args("LogModule"), [&](gsf::Args args) {
			log_m_ = args.pop_uint32(0);
		});
	}

	void init()
	{
		dispatch(log_m_, eid::log::log_callback, gsf::Args(), [&](gsf::Args args) {
			log_f_ = args.pop_log_callback(0);
		});

		for (int i = 0; i < 100000; ++i)
		{
			dispatch(timer_m_, eid::timer::delay_milliseconds, gsf::Args(get_module_id(), uint32_t(i * 1000)), [&](gsf::Args args) {
				uint64_t _time_id = args.pop_uint64(0);
				log_f_(gsf::TLog::LogInfo, "TestClickModule", gsf::Args("regist time ", args.pop_uint64(0)));

				if (idx_ > 3 && idx_ < 7) {
					dispatch(timer_m_, eid::timer::remove_timer, gsf::Args(get_module_id(), _time_id), [&](gsf::Args args) {
					});
				}

				idx_++;
			});
		}

		listen(this, eid::timer::timer_arrive, [&](gsf::Args args, gsf::CallbackFunc callback) {
			log_f_(gsf::TLog::LogInfo, "TestClickModule", gsf::Args("arrive time ", args.pop_uint64(0)));
		});
	}

private:
	uint32_t timer_m_;

	uint32_t log_m_;
	gsf::LogFunc log_f_;

	uint32_t idx_ = 0;
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
