#ifndef _TIMER_HANDLER_HEADER_
#define _TIMER_HANDLER_HEADER_

#include <memory>

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

#endif
