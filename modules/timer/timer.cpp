#include "timer.h"

#include "timer_event_list.h"

#include <assert.h>


gsf::modules::Timer::~Timer()
{

}


gsf::modules::Timer::Timer()
{
	min_heap_ctor(&min_heap_);
}


void gsf::modules::Timer::init()
{
	using namespace std::placeholders;

	listen(event_delay_milliseconds, std::bind(&Timer::delay_milliseconds, this, _1, _2));
    listen(event_delay_day, std::bind(&Timer::delay_day, this, _1, _2));
}

void gsf::modules::Timer::execute()
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

void gsf::modules::Timer::delay_milliseconds(gsf::stream::OStream args, gsf::core::EventHandlerPtr callback)
{
	gsf::stream::IStream is(args.getBlock());
	uint32_t _sender = 0;
	uint32_t _milliseconds = 0;
	is >> _sender;
	is >> _milliseconds;

	auto _tp = std::chrono::system_clock::now() + std::chrono::milliseconds(_milliseconds);

	TimerEvent *_event = new TimerEvent();
	_event->timer_handler_ptr_ = callback;
	_event->tp_ = _tp;

	min_heap_push(&min_heap_, _event);

}

void gsf::modules::Timer::delay_day(gsf::stream::OStream args, gsf::core::EventHandlerPtr callback)
{
	using namespace std::chrono;

	gsf::stream::IStream is(args.getBlock());
	uint32_t _sender = 0, _hour = 0, _minute = 0;
	is >> _sender >> _hour >> _minute;

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



//int gsf::timer::Timer::rmv_timer(TimerEvent *e)
//{
//
//	return min_heap_erase(&min_heap_, e);
//
//}
