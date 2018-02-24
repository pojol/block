#include "timer.h"

#include <core/application.h>

#include <assert.h>
#include <fmt/format.h>

gsf::modules::TimerModule::~TimerModule()
{

}

gsf::modules::TimerModule::TimerModule()
	: Module("TimerModule")
{
}

void gsf::modules::TimerModule::before_init()
{
	log_m_ = APP.get_module("LogModule");
	assert(log_m_ != gsf::ModuleNil);
}

void gsf::modules::TimerModule::init()
{
	using namespace std::placeholders;

	listen(this, eid::timer::delay_milliseconds	
		, std::bind(&TimerModule::delay_milliseconds, this, _1));
	
	listen(this, eid::timer::delay_day
		, std::bind(&TimerModule::delay_day, this, _1));
	
	listen(this, eid::timer::remove_timer
		, std::bind(&TimerModule::remove_timer, this, _1));

	boardcast(eid::module_init_succ, gsf::make_args(get_module_id()));
}

void gsf::modules::TimerModule::execute()
{
	using namespace std::chrono;
	
	if (!map_.empty()) {

		auto itr = map_.begin();
		uint64_t _now = get_system_tick() - start_time_;
		uint64_t _time_id = itr->first;

		while ((_time_id >> sequence_bit_) < _now)
		{
			dispatch(itr->second->target_, eid::timer::timer_arrive, gsf::make_args(itr->second->timerid_));
			map_.erase(itr);

			if (!map_.empty()) {
				itr = map_.begin();
				_time_id = itr->first;
			}
			else {
				break;
			}
		}
	}
}


uint64_t gsf::modules::TimerModule::get_system_tick()
{
	return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

uint64_t gsf::modules::TimerModule::make_timer_id(uint64_t delay)
{
	uint64_t _tick = 0;

	_tick = get_system_tick() - start_time_ + delay;

	_tick <<= sequence_bit_;
	_tick |= (++local_idx_ & sequence_mask_);

	return _tick;
}

gsf::ArgsPtr gsf::modules::TimerModule::delay_milliseconds(const gsf::ArgsPtr &args)
{
	uint32_t _sender = args->pop_i32();
	uint32_t _milliseconds = args->pop_i32();

	auto _tid = make_timer_id(_milliseconds);

	auto _event = std::make_shared<TimerEvent>();
	_event->target_ = _sender;
	_event->timerid_ = _tid;
	
	assert(map_.find(_tid) == map_.end());
	map_.insert(std::make_pair(_tid, _event));

	return gsf::make_args(_tid);
}


gsf::ArgsPtr gsf::modules::TimerModule::delay_day(const gsf::ArgsPtr &args)
{
	using namespace std::chrono;

	uint32_t _sender = args->pop_i32();
	uint32_t _hour = args->pop_i32();
	uint32_t _minute = args->pop_i32();

	typedef duration<int, std::ratio<60 * 60 * 24>> dur_day;
	time_point<system_clock, dur_day> _today = time_point_cast<dur_day>(system_clock::now());

	time_point<system_clock, seconds> _second = time_point_cast<seconds>(system_clock::now());

	uint32_t _passed_second = static_cast<uint32_t>(_second.time_since_epoch().count() - _today.time_since_epoch().count() * 24 * 60 * 60);
	uint32_t _space_second = _hour * 60 * 60 + _minute * 60;

	auto _event = std::make_shared<TimerEvent>();
	uint64_t _delay = 0;

	if (_space_second > _passed_second){
		_delay = (_second + seconds(_space_second - _passed_second)).time_since_epoch().count();
	}
	else {
		_delay = (_second + seconds((24 * 60 * 60) - _passed_second - _space_second)).time_since_epoch().count();
	}

	auto _tid = make_timer_id(_delay);

	assert(map_.find(_tid) == map_.end());
	_event->target_ = _sender;
	_event->timerid_ = _tid;

	map_.insert(std::make_pair(_tid, _event));

	return gsf::make_args(_tid);
}


gsf::ArgsPtr gsf::modules::TimerModule::remove_timer(const gsf::ArgsPtr &args)
{
	uint32_t _sender = args->pop_i32();
	uint64_t _timer_id = args->pop_ui64();

	auto itr = map_.find(_timer_id);
	if (itr != map_.end()) {
		map_.erase(itr);

		return gsf::make_args(true);
	}
	else {
		APP.WARN_LOG("TimerModule", "can't find remove timer", "{}\n", _timer_id);
	}

	return gsf::make_args(false);
}
