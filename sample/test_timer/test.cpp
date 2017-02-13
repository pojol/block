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

#include <module/application.h>
#include <event/event.h>
#include <stream/istream.h>
#include <stream/ostream.h>

#if defined(WIN32)
	#include <windows.h>
#else
	#include <unistd.h>
#include "../../modules/timer/timer_event_list.h"

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

        listen(this, std::bind([=](uint32_t event, gsf::stream::OStream os){
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

        gsf::stream::OStream os;
        os << 10;
        dispatch(event_delay_milliseconds
                , os, make_callback(&TestClickModule::click, this, std::string("hello,timer!")));
	}

	void click(std::string &str)
	{
		std::cout << str << std::endl;
	}
};


int main()
{
	TestTimerApp app;

	app.regist_module(gsf::core::EventModule::get_ptr());
    app.regist_module(new TestClickModule);

	app.run();

	return 0;
}
