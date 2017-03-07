#include "timer.h"

#include <assert.h>


gsf::modules::TimerModule::~TimerModule()
{

}


gsf::modules::TimerModule::TimerModule()
	:timer_id_(1)
{
}


void gsf::modules::TimerModule::init()
{
	using namespace std::placeholders;

	listen(make_event<TimerModule>(event_id::timer::delay_milliseconds)	
		, std::bind(&TimerModule::delay_milliseconds, this, _1, _2));
	
	listen(make_event<TimerModule>(event_id::timer::delay_day)			
		, std::bind(&TimerModule::delay_day, this, _1, _2));
	
	listen(make_event<TimerModule>(event_id::timer::remove_timer)		
		, std::bind(&TimerModule::remove_timer, this, _1, _2));
}

void gsf::modules::TimerModule::execute()
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

void gsf::modules::TimerModule::delay_milliseconds(gsf::Args args, gsf::EventHandlerPtr callback)
{
	uint32_t _sender = args.pop_uint32(0);
	uint32_t _milliseconds = args.pop_uint32(1);

	auto _tp = std::chrono::system_clock::now() + std::chrono::milliseconds(_milliseconds);

	auto _event = std::make_shared<TimerEvent>(make_timer_id());
	_event->timer_handler_ptr_ = callback;
	_event->tp_ = _tp;

	map_.insert(std::make_pair(_event->get_id(), _event));

	// result
	gsf::Args _res;
	_res << _event->get_id();
	dispatch(make_event(_sender, event_id::timer::make_timer_success), _res);
}

void gsf::modules::TimerModule::delay_day(gsf::Args args, gsf::EventHandlerPtr callback)
{
	using namespace std::chrono;

	uint32_t _sender = args.pop_uint32(0);
	uint32_t _hour = args.pop_uint32(1);
	uint32_t _minute = args.pop_uint32(2);

	typedef duration<int, std::ratio<60 * 60 * 24>> dur_day;
	time_point<system_clock, dur_day> _today = time_point_cast<dur_day>(system_clock::now());

	time_point<system_clock, seconds> _second = time_point_cast<seconds>(system_clock::now());

	uint32_t _passed_second = static_cast<uint32_t>(_second.time_since_epoch().count() - _today.time_since_epoch().count() * 24 * 60 * 60);
	uint32_t _space_second = _hour * 60 * 60 + _minute * 60;

	auto _event = std::make_shared<TimerEvent>(make_timer_id());
	if (_space_second > _passed_second){
		_event->tp_ = _second + seconds(_space_second - _passed_second);
	}
	else {
		_event->tp_ = _second + seconds((24 * 60 * 60) - _passed_second - _space_second);
	}

	_event->timer_handler_ptr_ = callback;

	map_.insert(std::make_pair(_event->get_id(), _event));

	// result
	gsf::Args _res;
	_res << _event->get_id();
	dispatch(make_event(_sender, event_id::timer::make_timer_success), _res);
}

void gsf::modules::TimerModule::remove_timer(gsf::Args args, gsf::EventHandlerPtr callback)
{
	uint32_t _sender = args.pop_uint32(0);
	uint32_t _timer_id = args.pop_uint32(1);

	auto itr = map_.find(_timer_id);
	if (itr != map_.end()) {
		map_.erase(itr);
	}
}

uint32_t gsf::modules::TimerModule::make_timer_id()
{
	if (timer_id_ == UINT32_MAX){
		timer_id_ = 1;
	}

	return timer_id_++;
}

gsf::modules::TimerEvent::TimerEvent(uint32_t id)
	:timerid_(id)
{
	
}
