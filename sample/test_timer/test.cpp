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

#include <module/application.h>
#include <event/event.h>

#include <timer/timer.h>

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
		dispatch(eid::app_id, eid::get_module, gsf::Args(std::string("TimerModule")), [&](gsf::Args args) {
			timer_module_ = args.pop_uint32(0);
		});
	}

	void init()
	{
		// test1
		listen(this, eid::timer::make_timer_success
			, [=](gsf::Args args, gsf::CallbackFunc callback) {
			std::cout << "success by event id : " << args.pop_uint32(0) << std::endl;
		});

		listen(this, eid::timer::make_timer_fail
			, [=](gsf::Args args, gsf::CallbackFunc callback) {
			std::cout << "fail by error id : " << args.pop_uint32(0) << std::endl;
		});

		tick_ = 0;

		// test2
		/*
		listen_callback(eid::timer::make_timer_success, [&](gsf::Args os) {
		tick_++;
		uint32_t _timer_id = os.pop_uint32(10);

		if (tick_ == 4) {
		gsf::Args args;
		args << get_door_id() << _timer_id;
		dispatch(eid::timer::remove_timer, args);
		}
		});

		for (int i = 0; i < 10; ++i)
		{
		gsf::Args args;
		args << get_door_id() << uint32_t(i * 1000);

		dispatch(eid::timer::delay_milliseconds
		, args
		, make_callback(&TestClickModule::test_2, this, i));
		}
		*/
		/* test3
		for (int i = 0; i < 10 * 10000; ++i)
		{
		gsf::Args args;
		args << get_door_id() << uint32_t(i * 10);

		dispatch(eid::timer::delay_milliseconds
		, args
		, make_callback(&TestClickModule::test_2, this, i));
		}
		*/

		dispatch(timer_module_, eid::timer::delay_milliseconds, gsf::Args(get_module_id(), uint32_t(1000)), [=](gsf::Args args) {
			std::cout << "timer!" << std::endl;
		});
	}

	void test_1(gsf::Args args)
	{
		std::cout << "timer" << std::endl;
	}

	void test_2(int i)
	{
		std::cout << i << std::endl;
	}

private:
	uint32_t tick_;
	uint32_t timer_module_;
};


int main()
{
	gsf::Application app;
	gsf::AppConfig cfg;
	app.init_cfg(cfg);

	app.regist_module(gsf::EventModule::get_ptr());
	app.regist_module(new gsf::modules::TimerModule());
	app.regist_module(new TestClickModule());

	app.run();

	return 0;
}
