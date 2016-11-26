#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <sys/types.h>


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

int main()
{
	using namespace gsf::utils;
	TimerTest *tt = new TimerTest();
	
	std::default_random_engine generator;
	std::uniform_int_distribution<int> dis(0, 100);

	for (int i = 0; i < 10000 * 10; ++i)
	{
		Timer::instance().add_timer(delay_second(dis(generator)), makeTimerHandler(&TimerTest::pt, tt, i));
	}

	while (1)
	{
		Timer::instance().update();
		Sleep(1);
	}

	return 0;
}
