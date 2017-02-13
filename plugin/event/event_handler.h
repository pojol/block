#ifndef _EVENT_HANDLER_HEADER_
#define _EVENT_HANDLER_HEADER_

#include <memory>

namespace gsf
{
	namespace core
	{
		class EventHandler
		{
		public:
			EventHandler();

			virtual ~EventHandler();

			virtual void execute() = 0;
		};

		inline EventHandler::EventHandler()
		{

		}
		inline EventHandler::~EventHandler()
		{

		}

#include "make_event_handler.h"
	}
}

#endif
