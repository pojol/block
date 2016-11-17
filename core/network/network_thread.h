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
	typedef std::unordered_map<int, ::evbuffer*> OBufferMap;
	typedef std::vector<std::pair<::evbuffer*, int>> ActiveBufferVec;

	class OBuffer
	{	
	public:
		int add_evbuffer(int fd);
		int rmv_evbuffer(int fd);
		
		void write_evbuffer(int fd, const char *data, int len);
		void send_evbuffer();
		
	private:
		OBufferMap buffer_map_;
		ActiveBufferVec active_buffer_vec_;
	};

	class IBuffer
	{
	public:
		int add_evbuffer(int session_id);
		int rmv_evbuffer(int session_id);

		evbuffer * find_evbuffer(int session_id);
		void push_evbuffer(ActiveBufferVec &vec);
		void pop_evbuffer();

		ActiveBufferVec active_buffer_vec_;
		std::mutex lock;
	private:
		OBufferMap buffer_map_;
	};
	
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
		
		OBuffer *out_buffer_;
		IBuffer *in_buffer_;
	};
}

#endif
