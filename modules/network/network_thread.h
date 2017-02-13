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
			IBuffer();
			~IBuffer();

			void mark_produce(uint32_t session_id, evbuffer *buff);

			void new_connect(uint32_t session_id);
			void dis_connect(uint32_t session_id);
			void fail_connect(const std::string &ip, uint32_t port);

			//! 生产，把消息填充到ringbuff
			void produce();

			//! 由主线程取出ringbuff
			void consume(std::vector<std::pair<uint32_t, evbuffer*>> &vec
				, std::vector<uint32_t> &conn
				, std::vector<uint32_t> &disconn
				, std::vector<std::pair<std::string, uint32_t>> &failconn);

		private:
			//! produce list
			std::vector<std::pair<uint32_t, evbuffer*>> ibuffer_vec_;
			std::vector<std::pair<uint32_t, evbuffer*>> consume_vec_;
			
			//! temp
			std::vector<uint32_t> connect_vec_;
			std::vector<uint32_t> disconn_vec_;
			std::vector<std::pair<std::string, uint32_t>> failconn_vec_;

			char * recvbuf_;

			std::mutex mtx;
		};

		class OBuffer
		{
			typedef std::vector<std::pair<uint32_t, evbuffer*>> ProduceVec;
		public:
			OBuffer();
			~OBuffer();

			void mian_thread_init(int threads);

			void write(uint32_t session_id, const char *data, int len);

			void produce();

			void consume(uint32_t thread_index, ProduceVec &vec);

		protected:
			void queue_push(uint32_t session_id, const char *data, int len);

		private:

			std::unordered_map<uint32_t, evbuffer*> active_index_map_;
			std::vector<ProduceVec> active_write_list_;
			std::unordered_map<uint32_t, evbuffer*> send_index_map_;
			std::vector<ProduceVec> send_write_list_;

			//std::vector<ProduceVec> thread_write_vec_;
			//std::vector<ProduceVec> thread_produce_vec_;
			
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
