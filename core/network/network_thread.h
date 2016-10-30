#ifndef _GSF_NETWORK_THREAD_HEADER_
#define _GSF_NETWORK_THREAD_HEADER_

#include <event2/event.h>
#include <stdint.h>
#include <memory>

#include "session_queue.h"

namespace gsf
{
	struct NetworkThread
	{
		struct event_base *event_base_ptr_;

		struct event *notify_event_;

#ifdef WIN32
		PHANDLE notify_receive_fd_;
		PHANDLE notify_send_fd_;
#else
		int32_t notify_receive_fd_;
		int32_t notify_send_fd_;
#endif // WIN32

		SessionQueue session_queue_;

		int32_t index_;
	};
}

#endif
