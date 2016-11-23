#ifndef _TIMER_HEADER_
#define _TIMER_HEADER_

#include <stdint.h>
#include <memory>
#include <map>

#include "min_heap.h"


namespace gsf
{
	namespace utils
	{
		class TimerHandler;

		template <typename T>
		struct timer_traits
		{
			typedef typename T::type type;
		};

		struct delay_second_tag {};
		struct delay_second
		{
			typedef delay_second_tag type;
			
			delay_second(uint32_t second)
				: second_(second)
			{}
			
			uint32_t Second() const { return second_; }
		private:
			uint32_t second_;
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

		struct delay_week_tag {};
		struct delay_week
		{
			typedef delay_week_tag type;
		
			delay_week(uint32_t day, uint32_t hour)
				: day_(day)
				, hour_(hour)
			{}
			
			uint32_t Day() const { return day_; }
			uint32_t Hour() const { return hour_; }
			
		private:
			uint32_t day_;
			uint32_t hour_;
		};

		struct delay_month_tag {};
		struct delay_month
		{
			typedef delay_month_tag type;
			
			delay_month(uint32_t day, uint32_t hour)
				: day_(day)
				, hour_(hour)
			{}
			
			uint32_t Day() const { return day_; }
			uint32_t Hour() const { return hour_; }
			
		private:
			uint32_t day_;
			uint32_t hour_;
		};

		class Timer
		{
			typedef std::shared_ptr<TimerHandler> TimerHandlerPtr;
		public:
			
			template <typename T>
			uint32_t add_timer(T delay, TimerHandlerPtr timer_handler_ptr);
			
			void rmv_timer(uint32_t timer_id);
			
			void update();

		private:
			struct TimerItem
			{
				std::shared_ptr<TimerHandler> timer_handler_ptr_;
				int64_t time_;
				int32_t timer_id_;
				
				bool operator > (const TimerItem &item)
				{
					return time_ > item.time_;
				}
				
				bool operator < (const TimerItem &item)
				{
					return time_ < item.time_;
				}
			};

			uint32_t update_delay(delay_second delay, TimerHandlerPtr handler, delay_second_tag);
			uint32_t update_delay(delay_day delay, TimerHandlerPtr handler, delay_day_tag);
			uint32_t update_delay(delay_week delay, TimerHandlerPtr handler, delay_week_tag);
			uint32_t update_delay(delay_month delay, TimerHandlerPtr handler, delay_month_tag);

			template <typename T>
			uint32_t Timer::add_timer(T delay, TimerHandlerPtr timer_handler_ptr)
			{
				update_delay(delay, timer_handler_ptr, typename timer_traits<T>::type());
			}

		private:

			MinHeap<TimerItem> minheap_;

			std::map<uint32_t, bool> mark_map_;
		};

	}
}

#endif
