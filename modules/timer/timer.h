#ifndef _TIMER_HEADER_
#define _TIMER_HEADER_

#include <stdint.h>
#include <memory>
#include <map>

#include <chrono>
#include <ctime>
#include <tuple>

#include "min_heap.h"

#include <module/module.h>
#include <stream/ostream.h>
#include <stream/istream.h>
#include <event/event.h>


namespace gsf
{
	namespace modules
	{
		struct TimerEvent
		{

            gsf::core::EventHandlerPtr timer_handler_ptr_;
			std::chrono::system_clock::time_point tp_;
			int32_t min_heap_idx;
		};

		class Timer
                : public gsf::core::Module
                , public gsf::core::Door
		{
		public:
			
			~Timer();


		protected:

            void init();

            void execute();

		private:
			Timer();

			void delay_milliseconds(std::tuple<gsf::utils::Any> args, gsf::core::EventHandlerPtr callback);
			void delay_day(std::tuple<gsf::utils::Any> args, gsf::core::EventHandlerPtr callback);
			void delay_week(std::tuple<gsf::utils::Any> args, gsf::core::EventHandlerPtr callback);
			void delay_month(std::tuple<gsf::utils::Any> args, gsf::core::EventHandlerPtr callback);

            void remove_timer();

		private:

			min_heap<TimerEvent> min_heap_;
		};


	}
}

#endif
