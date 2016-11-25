#include "timer.h"
#include "timer_handler.h"

#include <assert.h>

gsf::utils::Timer* gsf::utils::Timer::instance_ = NULL;


gsf::utils::Timer::~Timer()
{

}


gsf::utils::Timer::Timer()
{

}


gsf::utils::Timer& gsf::utils::Timer::instance()
{
	if (instance_ == NULL)
	{
		instance_ = new gsf::utils::Timer();
	}
	return *instance_;
}


uint32_t gsf::utils::Timer::update_delay(delay_second delay, TimerHandler *handler, delay_second_tag)
{
	auto _timeid = make_timeid();

	auto _tp = std::chrono::system_clock::now() + std::chrono::seconds(delay.Second());
	TimerItem _item;
	_item.timer_handler_ptr_ = handler;
	_item.timer_id_ = _timeid;
	_item.tp_ = _tp;
	
	minheap_.add(_item);
	mark_map_.insert(std::make_pair(_timeid, true));

	return _timeid;
}

uint32_t gsf::utils::Timer::update_delay(delay_day delay, TimerHandler *handler, delay_day_tag)
{
	auto _timeid = make_timeid();

	using namespace std::chrono;
	//! 这里不需要用到具体的日期，所以不转换到北京时间去处理。

	typedef duration<int, std::ratio<60 * 60 * 24>> dur_day;
	time_point<system_clock, dur_day> _today = time_point_cast<dur_day>(system_clock::now());

	time_point<system_clock, seconds> _second = time_point_cast<seconds>(system_clock::now());
	
	uint32_t _passed_second = static_cast<uint32_t>(_second.time_since_epoch().count() - _today.time_since_epoch().count() * 24 * 60 * 60);
	uint32_t _space_second = delay.Hour() * 60 * 60 + delay.Minute() * 60;

	TimerItem _item;
	if (_space_second > _passed_second){
		_item.tp_ = _second + seconds(_space_second - _passed_second);
	}
	else {
		_item.tp_ = _second + seconds((24 * 60 * 60) - _passed_second - _space_second);
	}

	_item.timer_handler_ptr_ = handler;
	_item.timer_id_ = _timeid;

	minheap_.add(_item);
	mark_map_.insert(std::make_pair(_timeid, true));

	return _timeid;
}

uint32_t gsf::utils::Timer::update_delay(delay_week delay, TimerHandler *handler, delay_week_tag)
{
	auto _timeid = make_timeid();

	return _timeid;
}

uint32_t gsf::utils::Timer::update_delay(delay_month delay, TimerHandler *handler, delay_month_tag)
{
	auto _timeid = make_timeid();

	return _timeid;
}

int gsf::utils::Timer::rmv_timer(uint32_t timer_id)
{
	auto itr = mark_map_.find(timer_id);
	if (itr != mark_map_.end()){
		itr->second = false;
		return 0;
	}
	else {
		return -1;
	}
}

void gsf::utils::Timer::update()
{
	using namespace std::chrono;

	if (!minheap_.empty())
	{
		TimerItem _item = minheap_.get_min();
		time_point<system_clock, seconds> _second = time_point_cast<seconds>(system_clock::now());

		while (_item.tp_ < _second)
		{
			minheap_.rem_min();
			
			auto itr = mark_map_.find(_item.timer_id_);
			assert(itr != mark_map_.end());

			if (itr->second){
				_item.timer_handler_ptr_->handleTimeout();
			}
			mark_map_.erase(itr);

			_item = minheap_.get_min();
		}
	}

}

uint32_t gsf::utils::Timer::make_timeid()
{
	time_index_++;

	if (time_index_ == UINT32_MAX){
		time_index_ = 0;
	}

	return time_index_;
}

uint32_t gsf::utils::Timer::time_index_ = 0;
