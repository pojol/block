#include "timer.h"
#include "timer_handler.h"

#include <assert.h>

gsf::utils::Timer* gsf::utils::Timer::instance_ = NULL;


gsf::utils::Timer::~Timer()
{

}


gsf::utils::Timer::Timer()
{
	min_heap_ctor(&min_heap_);
}


gsf::utils::Timer& gsf::utils::Timer::instance()
{
	if (instance_ == NULL)
	{
		instance_ = new gsf::utils::Timer();
	}
	return *instance_;
}


gsf::utils::TimerEvent * gsf::utils::Timer::update_delay(delay_milliseconds delay, TimerHandlerPtr handler, delay_milliseconds_tag)
{
	auto _tp = std::chrono::system_clock::now() + std::chrono::milliseconds(delay.milliseconds());
	
	TimerEvent *_event = new TimerEvent();
	_event->timer_handler_ptr_ = handler;
	_event->tp_ = _tp;

	min_heap_push(&min_heap_, _event);

	return _event;
}

gsf::utils::TimerEvent * gsf::utils::Timer::update_delay(delay_day delay, TimerHandlerPtr handler, delay_day_tag)
{
	using namespace std::chrono;
	//! 

	typedef duration<int, std::ratio<60 * 60 * 24>> dur_day;
	time_point<system_clock, dur_day> _today = time_point_cast<dur_day>(system_clock::now());

	time_point<system_clock, seconds> _second = time_point_cast<seconds>(system_clock::now());
	
	uint32_t _passed_second = static_cast<uint32_t>(_second.time_since_epoch().count() - _today.time_since_epoch().count() * 24 * 60 * 60);
	uint32_t _space_second = delay.Hour() * 60 * 60 + delay.Minute() * 60;

	TimerEvent *_event = new TimerEvent();
	if (_space_second > _passed_second){
		_event->tp_ = _second + seconds(_space_second - _passed_second);
	}
	else {
		_event->tp_ = _second + seconds((24 * 60 * 60) - _passed_second - _space_second);
	}

	_event->timer_handler_ptr_ = handler;

	min_heap_push(&min_heap_, _event);
	
	return _event;
}

gsf::utils::TimerEvent * gsf::utils::Timer::update_delay(delay_week delay, TimerHandlerPtr  handler, delay_week_tag)
{
	
	return nullptr;
}

gsf::utils::TimerEvent * gsf::utils::Timer::update_delay(delay_month delay, TimerHandlerPtr handler, delay_month_tag)
{

	return nullptr;
}

int gsf::utils::Timer::rmv_timer(TimerEvent *e)
{

	return min_heap_erase(&min_heap_, e);

}

void gsf::utils::Timer::update()
{
	using namespace std::chrono;

	if (!min_heap_empty(&min_heap_))
	{
		TimerEvent *_event_ptr = min_heap_top(&min_heap_);
		auto _now = time_point_cast<milliseconds>(system_clock::now());

		while (_event_ptr->tp_ < _now)
		{
			min_heap_pop(&min_heap_);
			
			_event_ptr->timer_handler_ptr_->handleTimeout();
			
			if (!min_heap_empty(&min_heap_)){
				_event_ptr = min_heap_top(&min_heap_);
			}
			else {
				break;
			}
		}
	}

}
