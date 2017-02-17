#include "timer.h"

#include "timer_event_list.h"

#include <assert.h>


gsf::modules::Timer::~Timer()
{

}


gsf::modules::Timer::Timer()
{

}


void gsf::modules::Timer::init()
{
	using namespace std::placeholders;

	listen(event_delay_milliseconds, std::bind(&Timer::delay_milliseconds, this, _1, _2));
    listen(event_delay_day, std::bind(&Timer::delay_day, this, _1, _2));
	listen(event_remove_timer, std::bind(&Timer::remove_timer, this, _1, _2));
}

void gsf::modules::Timer::execute()
{
	using namespace std::chrono;
	
	if (!map_.empty()) {
		auto itr = map_.begin();
		auto _now = time_point_cast<milliseconds>(system_clock::now());

		while (itr->second->tp_ < _now) 
		{
			auto handler = itr->second->timer_handler_ptr_;
			map_.erase(itr);
			handler->execute();
			
			if (!map_.empty()) {
				itr = map_.begin();
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

	auto _event = std::make_shared<TimerEvent>();
	_event->timer_handler_ptr_ = callback;
	_event->tp_ = _tp;

	map_.insert(std::make_pair(_event->get_id(), _event));

	// result
	gsf::stream::OStream os;
	os << 1 << _event->get_id();
	dispatch(_sender, os, nullptr);
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

	auto _event = std::make_shared<TimerEvent>();
	if (_space_second > _passed_second){
		_event->tp_ = _second + seconds(_space_second - _passed_second);
	}
	else {
		_event->tp_ = _second + seconds((24 * 60 * 60) - _passed_second - _space_second);
	}

	_event->timer_handler_ptr_ = callback;

	map_.insert(std::make_pair(_event->get_id(), _event));

	// result
	gsf::stream::OStream os;
	os << 1 << _event->get_id();
	dispatch(_sender, os, nullptr);
}

void gsf::modules::Timer::remove_timer(gsf::stream::OStream args, gsf::core::EventHandlerPtr callback)
{
	gsf::stream::IStream is(args.getBlock());
	uint32_t _timer_id;
	is >> _timer_id;

	auto itr = map_.find(_timer_id);
	if (itr != map_.end()) {
		map_.erase(itr);
	}
}

// make time id
gsf::modules::TimerEvent::TimerEvent()
{

}
