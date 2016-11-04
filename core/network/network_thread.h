#ifndef _GSF_NETWORK_THREAD_HEADER_
#define _GSF_NETWORK_THREAD_HEADER_


#include <event2/event.h>


#include <stdint.h>
#include <memory>

#include "network_connect.h"


namespace gsf
{
	struct NetworkThread
	{
		struct event_base *event_base_ptr_;

        struct event *notify_event_;

		evutil_socket_t notify_receive_fd_;
		evutil_socket_t notify_send_fd_;

		CQ *connect_queue_;

		int32_t index_;
	};
}

#endif
