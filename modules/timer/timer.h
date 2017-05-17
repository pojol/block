#ifndef _TIMER_HEADER_
#define _TIMER_HEADER_

#include <memory>
#include <map>

#include <chrono>
#include <ctime>
#include <tuple>

#include <module/module.h>
#include <event/event.h>

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

			void before_init();

            void init();

            void execute();

		private:

			uint64_t get_system_tick();

			void delay_milliseconds(gsf::Args args, gsf::CallbackFunc callback);
			void delay_day(gsf::Args args, gsf::CallbackFunc callback);
			//void delay_week(std::tuple<gsf::utils::Any> args, gsf::EventHandlerPtr callback);
			//void delay_month(std::tuple<gsf::utils::Any> args, gsf::EventHandlerPtr callback);

            void remove_timer(gsf::Args args, gsf::CallbackFunc callback);
		private:

			uint32_t log_m_;

			typedef std::map<uint64_t, std::list<TimerEventPtr>> TimerMap;

			TimerMap map_;
		};


	}
}

#endif
