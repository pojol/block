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
	class ThreadMessageBuffer
	{
		typedef std::unordered_map<int, ::evbuffer*> MessageBufferMap;
		typedef std::vector<std::pair<::evbuffer*, int>> ActiveBufferVec;
	public:
		int add_evbuffer(int fd);
		int rmv_evbuffer(int fd);
		
		void write_evbuffer(int fd, const uint8_t *data, int len);
		void send_evbuffer();
		
	private:
		MessageBufferMap message_buffer_map_;
		ActiveBufferVec active_buffer_vec_;
	}
	
	struct NetworkThread
	{
		std::thread *th;
		
		event_base *event_base_ptr_;

		event *notify_event_;

		evutil_socket_t notify_receive_fd_;
		evutil_socket_t notify_send_fd_;

		CQ *connect_queue_;
		
		ThreadMessageBuffer *buffer_;
	};
}

#endif
