#include "timer.hpp"

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

bool gsf::modules::TimerModule::make_timer_index(gsf::ModuleID target, int tag, uint64_t tid)
{
	auto _newtag = target * 1000 + tag;

	auto _idxItr = indexMap_.find(_newtag);
	if (_idxItr != indexMap_.end()) {
		if (_idxItr->second != 0) {
			APP.WARN_LOG("timer", "repet", " tag:{}\n", tag);
			return false;
		}

		_idxItr->second = tid;
	}
	else {
		indexMap_.insert(std::make_pair(_newtag, tid));
	}

	return true;
}

uint64_t gsf::modules::TimerModule::reset_timer_index(gsf::ModuleID target, int tag)
{
	uint64_t _tid = UINT64_MAX;

	auto _newtag = target * 1000 + tag;

	auto _idxItr = indexMap_.find(_newtag);
	if (_idxItr != indexMap_.end()) {
		_tid = _idxItr->second;
		_idxItr->second = 0;	//reset
	}
	else {
		APP.WARN_LOG("timer", "reset match index err!");
	}

	return _tid;
}

void gsf::modules::TimerModule::before_init()
{
	using namespace std::placeholders;

	listen(eid::timer::delay_milliseconds
		, std::bind(&TimerModule::eDelayMilliseconds, this, _1, _2));

	listen(eid::timer::delay_day
		, std::bind(&TimerModule::eDelayDay, this, _1, _2));

	listen(eid::timer::remove_timer
		, std::bind(&TimerModule::eRemoveTimer, this, _1, _2));
}

void gsf::modules::TimerModule::init()
{

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
			dispatch(itr->second->target_, eid::timer::timer_arrive, gsf::makeArgs(itr->second->tag_));
			
			reset_timer_index(itr->second->target_, itr->second->tag_);
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

void gsf::modules::TimerModule::shut()
{

}

void gsf::modules::TimerModule::eDelayMilliseconds(gsf::ModuleID target, gsf::ArgsPtr args)
{
	int _tag = args->pop_i32();
	uint32_t _milliseconds = args->pop_i32();

	auto _tid = make_timer_id(_milliseconds);

	auto _event = std::make_shared<TimerHandler>();
	_event->target_ = target;
	_event->timerid_ = _tid;
	_event->tag_ = _tag;

	assert(map_.find(_tid) == map_.end());
	if (make_timer_index(target, _tag, _tid)) {
		map_.insert(std::make_pair(_tid, _event));
	}
}


void gsf::modules::TimerModule::eDelayDay(gsf::ModuleID target, gsf::ArgsPtr args)
{
	using namespace std::chrono;

	int _tag = args->pop_i32();
	uint32_t _hour = args->pop_i32();
	uint32_t _minute = args->pop_i32();

	typedef duration<int, std::ratio<60 * 60 * 24>> dur_day;
	time_point<system_clock, dur_day> _today = time_point_cast<dur_day>(system_clock::now());

	time_point<system_clock, seconds> _second = time_point_cast<seconds>(system_clock::now());

	uint32_t _passed_second = static_cast<uint32_t>(_second.time_since_epoch().count() - _today.time_since_epoch().count() * 24 * 60 * 60);
	uint32_t _space_second = _hour * 60 * 60 + _minute * 60;

	auto _event = std::make_shared<TimerHandler>();
	uint64_t _delay = 0;

	if (_space_second > _passed_second) {
		_delay = (_second + seconds(_space_second - _passed_second)).time_since_epoch().count();
	}
	else {
		_delay = (_second + seconds((24 * 60 * 60) - _passed_second - _space_second)).time_since_epoch().count();
	}

	auto _tid = make_timer_id(_delay);

	assert(map_.find(_tid) == map_.end());
	_event->target_ = target;
	_event->timerid_ = _tid;
	_event->tag_ = _tag;

	if (make_timer_index(target, _tag, _tid)) {
		map_.insert(std::make_pair(_tid, _event));
	}
}


void gsf::modules::TimerModule::eRemoveTimer(gsf::ModuleID target, gsf::ArgsPtr args)
{
	uint32_t _tag = args->pop_i32();
	auto _tid = reset_timer_index(target, _tag);
	
	if (_tid != UINT64_MAX) {
		auto _titr = map_.find(_tid);
		if (_titr != map_.end()) {
			map_.erase(_titr);
		}
		else {
			APP.WARN_LOG("timer", "invalid remove!", " target:{} tag:{}", target, _tag);
		}
	}
}