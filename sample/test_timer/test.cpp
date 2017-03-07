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
#include <timer/timer_event_list.h>

#if defined(WIN32)
	#include <windows.h>
#else
	#include <unistd.h>
#endif

static int test_tick = -1;

class TestTimerApp : public gsf::Application
{
public:

	void tick()
	{
		int t = (test_tick + 1) % 50;

		if (t == 0){
			printf("delay %d\n", delay_);
		}

		test_tick = t;
	}

};

class TestClickModule
        : public gsf::Module
        , public gsf::Door
{
public:
	void init()
	{
		tick_ = 0;

		// test1
		listen(make_event<gsf::modules::TimerModule>(event_id::timer::make_timer_success)
			, [=](gsf::Args args, gsf::EventHandlerPtr callback) {
			std::cout << "success by event id : " << args.pop_uint32(0) << std::endl;
		});

		listen(make_event<gsf::modules::TimerModule>(event_id::timer::make_timer_fail)
			, [=](gsf::Args args, gsf::EventHandlerPtr callback) {
			std::cout << "fail by error id : " << args.pop_uint32(0) << std::endl;
		});

		gsf::Args args;
		args << get_door_id() << uint32_t(1000);

		dispatch(make_event<gsf::modules::TimerModule>(event_id::timer::delay_milliseconds)
			, args
			, make_callback(&TestClickModule::test_1, this, std::string("hello,timer!")));
		

		// test2
		/*		
		listen_callback(event_id::timer::make_timer_success, [&](gsf::Args os) {
			tick_++;
			uint32_t _timer_id = os.pop_uint32(10);

			if (tick_ == 4) {
				gsf::Args args;
				args << get_door_id() << _timer_id;
				dispatch(event_id::timer::remove_timer, args);
			}
		});

		for (int i = 0; i < 10; ++i)
		{
			gsf::Args args;
			args << get_door_id() << uint32_t(i * 1000);

			dispatch(event_id::timer::delay_milliseconds
				, args
				, make_callback(&TestClickModule::test_2, this, i));
		}
		*/
		/* test3
		for (int i = 0; i < 10 * 10000; ++i)
		{
			gsf::Args args;
			args << get_door_id() << uint32_t(i * 10);

			dispatch(event_id::timer::delay_milliseconds
				, args
				, make_callback(&TestClickModule::test_2, this, i));
		}
		*/
	}

	void test_1(std::string str)
	{
		std::cout << str.c_str() << std::endl;
	}

	void test_2(int i)
	{
		std::cout << i << std::endl;
	}

private:
	uint32_t tick_;
};


int main()
{
	TestTimerApp app;
	new gsf::EventModule;

	app.regist_module(gsf::EventModule::get_ptr());
	app.regist_module(new gsf::modules::TimerModule);
    app.regist_module(new TestClickModule);

	app.run();

	return 0;
}
