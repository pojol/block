#ifndef _GSF_SESSION_HEADER_
#define _GSF_SESSION_HEADER_

#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <event/event.h>

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

		static const uint32_t MSG_SIZE_LEN = 4;
		static const uint32_t MSG_ID_LEN = 4;

		class Session
			: public gsf::Door
		{
		public:
			Session(::bufferevent *bev, int fd, int door, std::function<void (int)> disconnect_callback);
			~Session();

			static void read_cb(::bufferevent *bev, void *ctx);
			static void err_cb(::bufferevent *bev, short what, void *ctx);

			int write(uint32_t msg_id, BlockPtr blockptr);
			void read(::bufferevent *bev);

			void dis_connect();

			int32_t get_id() const { return fd_; }

		protected:

		private:
			int fd_;
			int door_;

			std::function<void(int)> disconnect_callback_;

			::bufferevent *bev_;

			::evbuffer *in_buf_;
			::evbuffer *out_buf_;
		};
	}
	
}


#endif
