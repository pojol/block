#ifndef _GSF_SESSION_HEADER_
#define _GSF_SESSION_HEADER_

#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <stdint.h>
#include <memory>
#include <string>

namespace gsf
{
	namespace network
	{
		class OBuffer;
		class IBuffer;

		static const uint32_t SESSION_MAX_CONNECT = 100000;

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

			void read(::bufferevent *bev);
			static void read_buffer_cb(::evbuffer *buffer, const ::evbuffer_cb_info *info, void *arg);
			void change_read_state();

			void dis_connect();

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

		};
	}
	
}


#endif
