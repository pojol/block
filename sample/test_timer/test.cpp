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

class TestTimerApp : public gsf::core::Application
{
public:



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

		/*
        listen(this, std::bind([=](gsf::stream::OStream os, EventHandlerPtr callback){
            uint32_t arg1 = 0, arg2 = 0;
            gsf::stream::IStream is(os.getBlock());
            is >> arg1;
            is >> arg2;

            if (arg1 == 1){
                std::cout << "success by event id : " << arg2 << std::endl;
            }
            else {
                std::cout << "fail by errcode : " << arg2 << std::endl;
            }
        }));
		*/
		listen(this, std::bind(&TestClickModule::testf, this, std::placeholders::_1, std::placeholders::_2));

		OStream os;
		os << 1 << 10;

        dispatch(event_delay_milliseconds
                , os
                , make_callback(&TestClickModule::click, this, std::string("hello,timer!")));
	}

	void testf(gsf::stream::OStream os, gsf::core::EventHandlerPtr callback)
	{

	}

	void click(std::string str)
	{
		std::cout << str << std::endl;
	}
};


int main()
{
	TestTimerApp app;

	app.regist_module(new gsf::core::EventModule);
	app.regist_module(new gsf::modules::Timer);
    app.regist_module(new TestClickModule);

	app.run();

	return 0;
}
