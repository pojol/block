#ifndef _GSF_SESSION_HEADER_
#define _GSF_SESSION_HEADER_

#include <event2/bufferevent.h>
#include <stdint.h>
#include <memory>
#include <string>

namespace gsf
{
	namespace network
	{
		class OBuffer;
		class IBuffer;

		class SessionCloseHandler
		{
		public:
			virtual ~SessionCloseHandler(){};

			virtual void handle_close(uint32_t session_id
				, int err_code
				, int accepor_id
				, int connector_id
				, const std::string &ip
				, const int port) = 0;
		};

		class SessionHandler
		{
		public:
			virtual ~SessionHandler();

			virtual void handle_data(const char *data) = 0;

		};

		class Session
		{
		public:
			Session(int32_t id, ::bufferevent *bev, int fd);
			~Session();

			int init(IBuffer *ibuffer, OBuffer *obuffer);

			static void read_cb(::bufferevent *bev, void *ctx);

			static void err_cb(::bufferevent *bev, short what, void *ctx);

			int write(::evbuffer *data);

			int32_t get_id() const { return id_; }

			SessionHandler * get_session_handler() { return session_handler_; }

			void read(::bufferevent *bev);

		protected:

		private:
			uint32_t id_;

			int fd_;
			::bufferevent *bev_;

			IBuffer *thread_in_buffer_;
			OBuffer *thread_out_buffer_;

			::evbuffer *in_buf_;
			::evbuffer *out_buf_;

			bool need_write_;
			bool need_read_;

			SessionHandler *session_handler_;
			SessionCloseHandler *close_handler_;
		};
	}
	
}


#endif
