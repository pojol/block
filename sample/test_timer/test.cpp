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

class AppFace
	: public gsf::utils::Singleton<AppFace>
	, public gsf::IEvent
{
public:

	void init(gsf::Application *app)
	{
		app_ = app;
	}

	template <typename M>
	uint32_t get_module_id();

private:
	gsf::Application *app_;
};

template <typename M>
uint32_t AppFace::get_module_id()
{
	return app_->find_module_id<M>();
}

#define Face AppFace::get_ref()

class TestClickModule
        : public gsf::Module
        , public gsf::IEvent
{
public:
	void init()
	{
		tick_ = 0;
		uint32_t _timer_module_id = Face.get_module_id<gsf::modules::TimerModule>();

		// test1
		listen(this, eid::timer::make_timer_success
			, [=](gsf::Args args, gsf::EventHandlerPtr callback) {
			std::cout << "success by event id : " << args.pop_uint32(0) << std::endl;
		});

		listen(this, eid::timer::make_timer_fail
			, [=](gsf::Args args, gsf::EventHandlerPtr callback) {
			std::cout << "fail by error id : " << args.pop_uint32(0) << std::endl;
		});

		gsf::Args args;
		args << get_module_id() << uint32_t(1000);

		dispatch(_timer_module_id, eid::timer::delay_milliseconds
			, args
			, make_callback(&TestClickModule::test_1, this, std::string("hello,timer!")));
		

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
	auto appptr = new gsf::Application;
	new gsf::EventModule;
	new AppFace;

	appptr->regist_module(gsf::EventModule::get_ptr());
	appptr->regist_module(new gsf::modules::TimerModule);
	appptr->regist_module(new TestClickModule);

	Face.init(appptr);
	appptr->run();

	return 0;
}
