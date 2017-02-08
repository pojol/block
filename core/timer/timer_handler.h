#ifndef _TIMER_HANDLER_HEADER_
#define _TIMER_HANDLER_HEADER_

#include <memory>

namespace gsf
{
	namespace utils
	{
		class TimerHandler
		{
		public:
			TimerHandler();

			virtual ~TimerHandler();

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
