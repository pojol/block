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

			bool make_timer_index(gsf::ModuleID target, int tag, uint64_t tid);
			uint64_t reset_timer_index(gsf::ModuleID target, int tag);

		private:

			typedef std::map<uint64_t, TimerHandlerPtr> TimerMap;
			TimerMap map_;

			typedef std::unordered_map<uint32_t, uint64_t> TimerIndexMap;
			TimerIndexMap indexMap_;
		};

		
	}
}