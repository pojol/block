#ifndef _TIMER_HEADER_
#define _TIMER_HEADER_

#include <stdint.h>

namespace gsf
{
	namespace utils
	{
		template <typename T>
		struct timer_traits
		{
			typedef typename T::type type;
		}

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
		public:
			
			template <typename T>
			uint32_t add_timer(T delay, std::shared_ptr<TimerHandler> timer_handler_ptr);
			
			void rmv_timer(uint32_t timer_id);
			
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
			}
		};
	}
}

#endif
