#pragma once

#include "timer_impl.hpp"

namespace block
{
	namespace utils
	{
		struct delay_milliseconds;
		struct delay_day;

		class Timer
		{
		public:

			/*!
				在进程主循环执行
			**/
			void exec()
			{
				impl_.exec();
			}

			/*!
				添加一个定时器
				delay_milliseconds	延时若干毫秒
				delay_day			延时若干天
			**/
			template <typename T>
			uint64_t delay(T delay, std::function<void()> callback)
			{
				return impl_.delay(delay, callback);
			}

			/*!
				从定时管理器中移除某个定时器
			**/
			void remove(uint64_t timerID)
			{
				impl_.remove(timerID);
			}

		private:
			TimerImpl impl_;
		};
	}
}