#pragma once

#include <memory>
#include <map>
#include <math.h>

#include <chrono>
#include <ctime>
#include <tuple>
#include <assert.h>

#include <fmt/format.h>

#include <core/module.h>
#include <core/application.h>

namespace gsf
{
	namespace modules
	{
		/*!
			timer handler
		**/
		struct TimerHandler
		{
			TimerHandler() {}

			std::function<void()> target_ = nullptr;
			uint64_t timerid_ = UINT64_MAX;
			uint32_t repet_ = UINT32_MAX;
		};

		typedef std::shared_ptr<TimerHandler> TimerHandlerPtr;

		template <typename T>
		struct timer_traits
		{
			typedef typename T::type type;
		};

		struct delay_milliseconds_tag {};
		struct delay_milliseconds
		{
			typedef delay_milliseconds_tag type;

			delay_milliseconds(uint32_t milliseconds)
				: milliseconds_(milliseconds)
			{}

			uint32_t milliseconds() const { return milliseconds_; }
		private:
			uint32_t milliseconds_;
		};

		struct delay_day_tag {};
		struct delay_day
		{
			typedef delay_day_tag type;

			delay_day(uint32_t hour, uint32_t minute)
				: hour_(hour)
				, minute_(minute)
			{}

			uint32_t Hour() const { return hour_; }
			uint32_t Minute() const { return minute_; }

		private:
			uint32_t hour_;
			uint32_t minute_;
		};


		class Timer
			: public gsf::utils::Singleton<Timer>
		{
		public:

			void exec();

			/*!
				延时若干毫秒
			**/
			uint64_t delay(delay_milliseconds milliseconds, std::function<void()> callback, delay_milliseconds_tag);
			
			/*!
				延时若干天
			**/
			uint64_t delay(delay_day day, std::function<void()> callback, delay_day_tag);

			/*!
				从定时管理器中移除某个定时器

			**/
			void remove(uint64_t timerID);
		
		private:

			uint64_t get_system_tick();
			uint64_t make_timer_id(uint64_t delay);

			uint64_t start_time_ = get_system_tick();

			uint64_t local_idx_ = 0;
			uint64_t sequence_bit_ = 15;
			uint64_t sequence_mask_ = (uint64_t)pow(2, sequence_bit_) - 1;
		private:

			typedef std::map<uint64_t, TimerHandlerPtr> TimerMap;
			TimerMap map_;
		};

		uint64_t gsf::modules::Timer::get_system_tick()
		{
			return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}

		uint64_t gsf::modules::Timer::make_timer_id(uint64_t delay)
		{
			uint64_t _tick = 0;

			_tick = get_system_tick() - start_time_ + delay;

			_tick <<= sequence_bit_;
			_tick |= (++local_idx_ & sequence_mask_);

			return _tick;
		}

		void gsf::modules::Timer::exec()
		{
			using namespace std::chrono;

			if (!map_.empty()) {

				auto itr = map_.begin();
				uint64_t _now = get_system_tick() - start_time_;
				uint64_t _time_id = itr->first;

				while ((_time_id >> sequence_bit_) < _now)
				{
					itr->second->target_();

					// repet!

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

		uint64_t gsf::modules::Timer::delay(delay_milliseconds milliseconds, std::function<void()> callback, delay_milliseconds_tag)
		{
			auto _tid = make_timer_id(milliseconds.milliseconds);

			auto _event = std::make_shared<TimerHandler>();
			_event->target_ = callback;
			_event->timerid_ = _tid;

			assert(map_.find(_tid) == map_.end());
			map_.insert(std::make_pair(_tid, _event));

			return _tid;
		}

		uint64_t gsf::modules::Timer::delay(delay_day day, std::function<void()> callback, delay_day_tag)
		{
			using namespace std::chrono;

			uint32_t _hour = day.Hour();
			uint32_t _minute = day.Minute();

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
			_event->target_ = callback;
			_event->timerid_ = _tid;

			map_.insert(std::make_pair(_tid, _event));

			return _tid;
		}

		void gsf::modules::Timer::remove(uint64_t timerID)
		{
			auto _titr = map_.find(timerID);
			if (_titr != map_.end()) {
				map_.erase(_titr);
			}
			else {
				APP.WARN_LOG("timer", "invalid remove!", " timerID:{}", timerID);
			}
		}
	}
}