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
#include <timer.h>
#include <timer_handler.h>

#include <windows.h>


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
		using namespace gsf::utils;
		timer_event_ = Timer::instance().add_timer(delay_second(3)
			, makeTimerHandler(&RecommendTest::pt, this, std::string("hello!")));
	}

	~RecommendTest()
	{
		if (timer_event_){
			if (gsf::utils::Timer::instance().rmv_timer(timer_event_) == 0){ //! succ
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
	gsf::utils::TimerEvent *timer_event_;
};

int main()
{
	using namespace gsf::utils;
	TimerTest *tt = new TimerTest();
	
	std::default_random_engine generator;
	std::uniform_int_distribution<int> dis(0, 10);

	for (int i = 0; i < 10; ++i)
	{
		//dis(generator)
		Timer::instance().add_timer(delay_second(i)
			, makeTimerHandler(&TimerTest::pt, tt, i));
	}

	RecommendTest test;

	while (1)
	{
		Timer::instance().update();
		Sleep(1);
	}

	return 0;
}
