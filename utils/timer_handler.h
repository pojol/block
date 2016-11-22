#ifndef _TIMER_HANDLER_HEADER_
#define _TIMER_HANDLER_HEADER_


namespace gsf
{
	namespace utils
	{
		//!定时器处理器接口定义
		class TimerHandler
		{
		public:
			//!构造函数
			TimerHandler();

			//!析构函数
			virtual ~TimerHandler();

			//!定时器处理函数
			virtual void handleTimeout() = 0;
		};

		inline TimerHandler::TimerHandler()
		{

		}
		inline TimerHandler::~TimerHandler()
		{

		}

#include "make_timer_handler.h"
	}
}

#endif
