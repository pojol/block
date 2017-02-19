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
#include <stream/istream.h>
#include <stream/ostream.h>

#include <timer/timer.h>
#include <timer/timer_event_list.h>

#if defined(WIN32)
	#include <windows.h>
#else
	#include <unistd.h>
#endif

static int test_tick = -1;

class TestTimerApp : public gsf::core::Application
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
        : public gsf::core::Module
        , public gsf::core::Door
{
public:
	void init()
	{
		using namespace gsf::core;
		using namespace gsf::stream;

		/* test1
		listen(this, [=](gsf::stream::OStream os, EventHandlerPtr callback){
		uint32_t arg1 = 0, arg2 = 0;
		gsf::stream::IStream is(os.getBlock());
		is >> arg1;
		is >> arg2;

		if (arg1 == event_id::timer::make_timer_success){
		std::cout << "success by event id : " << arg2 << std::endl;
		}
		else {
		std::cout << "fail by errcode : " << arg2 << std::endl;
		}
		});

		OStream args;
		args << get_door_id() << 3000;

		dispatch(event_id::timer::delay_milliseconds , args
		, make_callback(&TestClickModule::test_1, this, std::string("hello,timer!")));
		*/

		// test2
		listen(this, [=](gsf::stream::OStream os, EventHandlerPtr callback){
			uint32_t arg1 = 0, arg2 = 0;
			gsf::stream::IStream is(os.getBlock());
			is >> arg1 >> arg2;

			if (arg1 == event_id::timer::make_timer_success && arg2 == 4){

				OStream rArags;
				rArags << get_door_id() << arg2;
				dispatch(event_id::timer::remove_timer, rArags, nullptr);
			}
		});

		for (int i = 0; i < 10; ++i)
		{
			OStream args;
			args << get_door_id() << i * 1000;

			dispatch(event_id::timer::delay_milliseconds, args
				, make_callback(&TestClickModule::test_2, this, i));
		}
		

		/* test3
		for (int i = 0; i < 10 * 10000; ++i)
		{
			OStream args;
			args << get_door_id() << i * 10;

			dispatch(event_id::timer::delay_milliseconds, args, make_callback(&TestClickModule::test_2, this, i));
		}
		*/
	}

	void test_1(std::string str)
	{
		std::cout << str << std::endl;
	}

	void test_2(int i)
	{
		std::cout << i << std::endl;
	}
};


int main()
{
	TestTimerApp app;
	new gsf::core::EventModule;

	app.regist_module(gsf::core::EventModule::get_ptr());
	app.regist_module(new gsf::modules::TimerModule);
    app.regist_module(new TestClickModule);

	app.run();

	return 0;
}
