#include "timer.h"

#include <assert.h>

gsf::modules::TimerModule::~TimerModule()
{

}

gsf::modules::TimerModule::TimerModule()
	: Module("TimerModule")
{
}

void gsf::modules::TimerModule::before_init()
{
	dispatch(eid::app_id, eid::get_module, gsf::Args("LogModule"), [&](const gsf::Args &args) {
		log_m_ = args.pop_uint32(0);
	});
}

void gsf::modules::TimerModule::init()
{
	using namespace std::placeholders;

	listen(this, eid::timer::delay_milliseconds	
		, std::bind(&TimerModule::delay_milliseconds, this, _1, _2));
	
	listen(this, eid::timer::delay_day
		, std::bind(&TimerModule::delay_day, this, _1, _2));
	
	listen(this, eid::timer::remove_timer
		, std::bind(&TimerModule::remove_timer, this, _1, _2));
}

void gsf::modules::TimerModule::execute()
{
	using namespace std::chrono;
	
	if (!map_.empty()) {

		auto itr = map_.begin();
		uint64_t _tick = get_system_tick();

		while (itr->first < _tick)
		{
			for (auto it : itr->second)
			{
				dispatch(it->target_, eid::timer::timer_arrive, gsf::Args(it->timerid_));
			}
			map_.erase(itr);

			if (!map_.empty()) {
				itr = map_.begin();
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


void gsf::modules::TimerModule::delay_milliseconds(const gsf::Args &args, gsf::CallbackFunc callback)
{
	uint32_t _sender = args.pop_uint32(0);
	uint32_t _milliseconds = args.pop_uint32(1);

	// 不采用snowflake算法等方式生成唯一ID， 后续会通过module id构造
	auto _tid = get_system_tick() + _milliseconds;

	auto _event = std::make_shared<TimerEvent>();
	_event->target_ = _sender;
	_event->timerid_ = _tid;

	
	auto itr = map_.find(_tid);
	if (itr != map_.end()) {
		itr->second.push_back(_event);
	}
	else {
		std::list<TimerEventPtr> _list;
		_list.push_back(_event);
		map_.insert(std::make_pair(_tid, _list));
	}

	callback(gsf::Args(_tid));
}


void gsf::modules::TimerModule::delay_day(const gsf::Args &args, gsf::CallbackFunc callback)
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

	auto _event = std::make_shared<TimerEvent>();
	uint64_t _tid = 0;

	if (_space_second > _passed_second){
		_tid = (_second + seconds(_space_second - _passed_second)).time_since_epoch().count();
	}
	else {
		_tid = (_second + seconds((24 * 60 * 60) - _passed_second - _space_second)).time_since_epoch().count();
	}

	_event->target_ = _sender;
	_event->timerid_ = _tid;

	auto itr = map_.find(_tid);
	if (itr != map_.end()) {
		itr->second.push_back(_event);
	}
	else {
		std::list<TimerEventPtr> _list;
		_list.push_back(_event);
		map_.insert(std::make_pair(_tid, _list));
	}

	callback(gsf::Args(_tid));
}


void gsf::modules::TimerModule::remove_timer(const gsf::Args &args, gsf::CallbackFunc callback)
{
	uint32_t _sender = args.pop_uint32(0);
	uint64_t _timer_id = args.pop_uint64(1);

	auto itr = map_.find(_timer_id);
	if (itr != map_.end()) {
		map_.erase(itr);

		callback(gsf::Args(uint32_t(1)));
	}
}
