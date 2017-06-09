#ifndef _TIMER_HEADER_
#define _TIMER_HEADER_

#include <memory>
#include <map>

#include <chrono>
#include <ctime>
#include <tuple>

#include <core/module.h>
#include <core/event.h>

namespace gsf
{
	namespace modules
	{
		struct TimerEvent
		{
			TimerEvent()
				: target_(0)
				, timerid_(0)
			{}

			uint32_t target_;
			uint64_t timerid_;
		};

		typedef std::shared_ptr<TimerEvent> TimerEventPtr;

		class TimerModule
                : public gsf::Module
                , public gsf::IEvent
		{
		public:
			
			~TimerModule();
			TimerModule();

		protected:

			void before_init() override;

            void init() override;

            void execute() override;

		private:

			void delay_milliseconds(const gsf::Args &args, gsf::CallbackFunc callback);
			void delay_day(const gsf::Args &args, gsf::CallbackFunc callback);
			//void delay_week(std::tuple<gsf::utils::Any> args, gsf::EventHandlerPtr callback);
			//void delay_month(std::tuple<gsf::utils::Any> args, gsf::EventHandlerPtr callback);

            void remove_timer(const gsf::Args &args, gsf::CallbackFunc callback);
		private:
	
			uint64_t get_system_tick();
			uint64_t make_timer_id(uint64_t delay);

			uint64_t start_time_ = get_system_tick();

			uint64_t local_idx_ = 0;
			uint64_t sequence_bit_ = 15;
			uint64_t sequence_mask_ = (uint64_t)pow(2, sequence_bit_) - 1;

		private:

			uint32_t log_m_;

			typedef std::map<uint64_t, TimerEventPtr> TimerMap;
			TimerMap map_;
		};


	}
}

#endif
