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
#include <timer/timer.h>
#include <timer/timer_handler.h>

#if defined(WIN32)
	#include <windows.h>
#else
	#include <unistd.h>
#endif


static int count_ = 0;
class TimerTest
{
public:
	void pt(int i) { printf("%d\n", i); ++count_; }
};

class RecommendTest
{
public:
	RecommendTest()
	{
		using namespace gsf::timer;
		timer_event_ = Timer::instance().add_timer(delay_milliseconds(3000)
			, makeTimerHandler(&RecommendTest::pt, this, std::string("hello!")));
	}

	~RecommendTest()
	{
		using namespace gsf::timer;
		if (timer_event_){
			if (Timer::instance().rmv_timer(timer_event_) == 0){ //! succ
				delete timer_event_;
				timer_event_ = nullptr;
			}
		}
	}

	void pt(std::string str)
	{
		std::cout << str.c_str() << std::endl;

		delete timer_event_;
		timer_event_ = nullptr;
	}

private:
	gsf::timer::TimerEvent *timer_event_;
};

#include <functional>
const uint32_t event_delay_milliseconds = 1;

int main()
{
	using namespace gsf::timer;

	TimerTest *tt = new TimerTest();
	
	//plugin::event::dispatch(event_delay_milliseconds, std::bind(&TimerTest::pt, tt, std::placeholders::_1), 10);

	std::default_random_engine generator;
	std::uniform_int_distribution<int> dis(0, 10);

	for (int i = 0; i < 10; ++i)
	{
		//dis(generator)
		Timer::instance().add_timer(delay_milliseconds(i)
			, makeTimerHandler(&TimerTest::pt, tt, i));
	}

	RecommendTest test;

	while (1)
	{
		Timer::instance().update();
#if defined(WIN32)
		Sleep(1);
#else
		usleep(1000);
#endif
	}

	return 0;
}
