#ifndef _GSF_SESSION_HEADER_
#define _GSF_SESSION_HEADER_

#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <core/event.h>

#include <map>
#include <memory>
#include <string>

namespace gsf
{
	namespace network
	{
		class OBuffer;
		class IBuffer;
		class MsgBinder;

		class Session
			: public gsf::IEvent
		{
		public:
			Session(int fd, int eid, MsgBinder *binder, std::function<void (int)> disconnect_callback);
			~Session();

			static void read_cb(::bufferevent *bev, void *ctx);
			static void err_cb(::bufferevent *bev, short what, void *ctx);

			int write(uint32_t msg_id, BlockPtr blockptr);
			void read(::bufferevent *bev);

			void dis_connect(int32_t err);
			void new_connect();

			int32_t get_id() const { return fd_; }
			uint32_t get_module_id() const { return module_id_; }

			void set_log_module(uint32_t log_module) { log_module_ = log_module; }

		protected:

		private:
			int fd_;
			int module_id_;

			uint32_t log_module_;
			MsgBinder *binder_;

			std::function<void(int)> disconnect_callback_;

			::evbuffer *in_buf_;
			::evbuffer *out_buf_;
		};
	}
	
}


#endif
