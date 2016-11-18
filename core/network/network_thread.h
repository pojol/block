#ifndef _GSF_NETWORK_THREAD_HEADER_
#define _GSF_NETWORK_THREAD_HEADER_


#include <event2/event.h>

#include <vector>
#include <unordered_map>

#include <stdint.h>
#include <memory>
#include <thread>

#include "network_connect.h"


namespace gsf
{

	struct NetworkThread
	{
		NetworkThread();
		~NetworkThread();

		std::thread *th;
		
		event_base *event_base_ptr_;

		event *notify_event_;

		evutil_socket_t notify_receive_fd_;
		evutil_socket_t notify_send_fd_;

		CQ *connect_queue_;
	};
}

#endif
