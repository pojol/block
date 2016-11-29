#ifndef _GSF_NETWORK_THREAD_HEADER_
#define _GSF_NETWORK_THREAD_HEADER_


#include <event2/event.h>

#include <vector>
#include <unordered_map>

#include <stdint.h>
#include <memory>
#include <thread>

#include "session_mgr.h"
#include "network_connect.h"


namespace gsf
{
	namespace network
	{
		//! 两个buffer先上锁验证逻辑，后续优化为无锁队列（lockfree | ringbuffer ...)

		class IBuffer
		{
		public:
			void mark_produce(uint32_t session_id, evbuffer *buff);

			void new_connect(uint32_t session_id);

			//! 生产，把消息填充到ringbuff
			void produce();

			//! 由主线程取出ringbuff
			void consume(std::vector<std::pair<uint32_t, evbuffer*>> &vec, std::vector<uint32_t> &conn);

		private:
			//! produce list
			std::vector<std::pair<uint32_t, evbuffer*>> ibuffer_vec_;
			std::vector<std::pair<uint32_t, evbuffer*>> consume_vec_;
			
			//! temp
			std::vector<uint32_t> connect_vec_;

			std::mutex mtx;
		};

		class OBuffer
		{
			typedef std::vector<std::pair<uint32_t, evbuffer*>> ProduceVec;
		public:
			void mian_thread_init(int threads);

			void write(uint32_t session_id, const char *data, int len);

			void produce();

			void consume(uint32_t thread_index, ProduceVec &vec);

		private:

			std::vector<ProduceVec> thread_write_vec_;
			std::vector<ProduceVec> thread_produce_vec_;
			
			uint32_t thread_count_;
			std::mutex mtx;
		};

		struct NetworkThread
		{
			NetworkThread(int index);
			~NetworkThread();

			std::thread *th;

			event_base *event_base_ptr_;

			event *notify_event_;

			evutil_socket_t notify_receive_fd_;
			evutil_socket_t notify_send_fd_;

			CQ *connect_queue_;

			IBuffer *in_buffer_;
			OBuffer *out_buffer_;

			SessionMgr *session_mgr;

			int index_;
		};
	}
	
}

#endif
