#include "timer.h"

#include "timer_event_list.h"

#include <assert.h>


gsf::timer::Timer::~Timer()
{

}


gsf::timer::Timer::Timer()
{
	min_heap_ctor(&min_heap_);
}


void gsf::timer::Timer::init()
{
	using namespace std::placeholders;

	listen(event_delay_milliseconds, std::bind(&Timer::delay_milliseconds, this, _1, _2));
    listen(event_delay_day, std::bind(&Timer::delay_day, this, _1, _2));
}

void gsf::timer::Timer::execute()
{
	using namespace std::chrono;

	if (!min_heap_empty(&min_heap_))
	{
		TimerEvent *_event_ptr = min_heap_top(&min_heap_);
		auto _now = time_point_cast<milliseconds>(system_clock::now());

		while (_event_ptr->tp_ < _now)
		{
			min_heap_pop(&min_heap_);

			_event_ptr->timer_handler_ptr_->execute();

			if (!min_heap_empty(&min_heap_)){
				_event_ptr = min_heap_top(&min_heap_);
			}
			else {
				break;
			}
		}
	}
}

void gsf::timer::Timer::delay_milliseconds(std::tuple<gsf::utils::Any> args, gsf::core::EventHandlerPtr callback)
{
	uint32_t _sender = std::get<0>(args).AnyCast<uint32_t>();
	uint32_t _milliseconds;// = std::get<1>(args).AnyCast<uint32_t>();

	auto _tp = std::chrono::system_clock::now() + std::chrono::milliseconds(_milliseconds);

	TimerEvent *_event = new TimerEvent();
	_event->timer_handler_ptr_ = callback;
	_event->tp_ = _tp;

	min_heap_push(&min_heap_, _event);

}

void gsf::timer::Timer::delay_day(std::tuple<gsf::utils::Any> args, gsf::core::EventHandlerPtr callback)
{
	using namespace std::chrono;
	//!
    uint32_t _sender = std::get<0>(args).AnyCast<uint32_t>();
	uint32_t _hour;// = std::get<1>(args).AnyCast<uint32_t>();
    uint32_t _minute;// = std::get<2>(args).AnyCast<uint32_t>();

	typedef duration<int, std::ratio<60 * 60 * 24>> dur_day;
	time_point<system_clock, dur_day> _today = time_point_cast<dur_day>(system_clock::now());

	time_point<system_clock, seconds> _second = time_point_cast<seconds>(system_clock::now());
	
	uint32_t _passed_second = static_cast<uint32_t>(_second.time_since_epoch().count() - _today.time_since_epoch().count() * 24 * 60 * 60);
	uint32_t _space_second = _hour * 60 * 60 + _minute * 60;

	TimerEvent *_event = new TimerEvent();
	if (_space_second > _passed_second){
		_event->tp_ = _second + seconds(_space_second - _passed_second);
	}
	else {
		_event->tp_ = _second + seconds((24 * 60 * 60) - _passed_second - _space_second);
	}

	_event->timer_handler_ptr_ = callback;

	min_heap_push(&min_heap_, _event);
}

void gsf::timer::Timer::delay_week(std::tuple<gsf::utils::Any> args, gsf::core::EventHandlerPtr callback)
{

}

void gsf::timer::Timer::delay_month(std::tuple<gsf::utils::Any> args, gsf::core::EventHandlerPtr callback)
{

}

//int gsf::timer::Timer::rmv_timer(TimerEvent *e)
//{
//
//	return min_heap_erase(&min_heap_, e);
//
//}
