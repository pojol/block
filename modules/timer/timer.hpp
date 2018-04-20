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
			TimerHandler()
				: target_(0)
				, timerid_(0)
				, tag_(0)
			{}

			uint32_t target_ = UINT32_MAX;
			uint64_t timerid_ = UINT64_MAX;
			int32_t tag_ = INT32_MAX;
		};

		typedef std::shared_ptr<TimerHandler> TimerHandlerPtr;

		class TimerModule
			: public gsf::Module
		{
		public:

			virtual ~TimerModule()
			{
			}

			TimerModule()
				: Module("TimerModule")
			{
			}

		protected:

			void before_init() override;

			void init() override;

			void execute() override;

			void shut() override;

		protected:

			/*!
				延时若干毫秒
			**/
			void eDelayMilliseconds(gsf::ModuleID target, gsf::ArgsPtr args);
			
			/*!
				延时若干天
			**/
			void eDelayDay(gsf::ModuleID target, gsf::ArgsPtr args);

			/*!
				从定时管理器中移除某个定时器
			**/
			void eRemoveTimer(gsf::ModuleID target, gsf::ArgsPtr args);
		
		private:

			uint64_t get_system_tick();
			uint64_t make_timer_id(uint64_t delay);

			uint64_t start_time_ = get_system_tick();

			uint64_t local_idx_ = 0;
			uint64_t sequence_bit_ = 15;
			uint64_t sequence_mask_ = (uint64_t)pow(2, sequence_bit_) - 1;

		private:

			typedef std::map<uint64_t, TimerEventPtr> TimerMap;
			TimerMap map_;
		};

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

		void gsf::modules::TimerModule::before_init()
		{
			using namespace std::placeholders;

			mailboxPtr_->listen(eid::timer::delay_milliseconds
				, std::bind(&TimerModule::eDelayMilliseconds, this, _1, _2));

			mailboxPtr_->listen(eid::timer::delay_day
				, std::bind(&TimerModule::eDelayDay, this, _1, _2));

			mailboxPtr_->listen(eid::timer::remove_timer
				, std::bind(&TimerModule::eRemoveTimer, this, _1, _2));
		}

		void gsf::modules::TimerModule::init()
		{
			mailboxPtr_->pull();
		}

		void gsf::modules::TimerModule::execute()
		{
			using namespace std::chrono;

			mailboxPtr_->pull();

			if (!map_.empty()) {

				auto itr = map_.begin();
				uint64_t _now = get_system_tick() - start_time_;
				uint64_t _time_id = itr->first;

				while ((_time_id >> sequence_bit_) < _now)
				{
					mailboxPtr_->dispatch(itr->second->target_, eid::timer::timer_arrive, gsf::makeArgs(itr->second->tag_));
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
			mailboxPtr_->pull();
		}

		void gsf::modules::TimerModule::eDelayMilliseconds(gsf::ModuleID target, gsf::ArgsPtr args)
		{
			int _tag = args->pop_i32();
			uint32_t _milliseconds = args->pop_i32();

			auto _tid = make_timer_id(_milliseconds);

			auto _event = std::make_shared<TimerEvent>();
			_event->target_ = target;
			_event->timerid_ = _tid;
			_event->tag_ = _tag;

			assert(map_.find(_tid) == map_.end());
			map_.insert(std::make_pair(_tid, _event));
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

			auto _event = std::make_shared<TimerEvent>();
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

			map_.insert(std::make_pair(_tid, _event));
		}


		void gsf::modules::TimerModule::eRemoveTimer(gsf::ModuleID target, gsf::ArgsPtr args)
		{
			uint32_t _tag = args->pop_i32();

			//! ..
			for (TimerMap::iterator itr = map_.begin(); itr != map_.end(); ++itr)
			{
				if (itr->second->target_ == target && itr->second->tag_ == _tag) {
					map_.erase(itr);
					return;
				}
			}

			APP.WARN_LOG("TimerModule", "removeTimer can't find tag", "target:{} tag:{}\n", target, _tag);
			/*
			auto itr = map_.find(_timer_id);
			if (itr != map_.end()) {
			map_.erase(itr);
			}
			else {
			APP.WARN_LOG("TimerModule", "event_remove_timer can't find remove timer", "{}\n", _timer_id);
			}
			*/
		}
	}
}