#ifndef _GSF_SESSION_HEADER_
#define _GSF_SESSION_HEADER_

#include <event2/bufferevent.h>
#include <stdint.h>
#include <memory>
#include <string>

namespace gsf
{
	class OBuffer;

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

		virtual void handle_data(const uint8_t *data) = 0;
		
	};

	class Session
	{
    public:
        Session(int32_t id, ::bufferevent *bev, int fd, OBuffer *out_buffer);
		~Session();

		int open(SessionHandler *session_handler, SessionCloseHandler *close_handler);

		//! 这个地方需要把消息写入到同一个线程的FIFO队列里面，由逻辑循环读取
        static void read_cb(::bufferevent *bev, void *ctx);

		static void err_cb(::bufferevent *bev, short what, void *ctx);

		void write(const char *data, int len);

        int32_t get_id() const { return id_; }

		::evbuffer * get_inbuf() { return in_buf_; }

		SessionHandler * get_session_handler() { return session_handler_; }

	protected:

    private:
        uint32_t id_;
		
		int fd_;
		::bufferevent *bev_;

		OBuffer *out_buffer_;
		::evbuffer *in_buf_;

		SessionHandler *session_handler_;
		SessionCloseHandler *close_handler_;
	};
}


#endif
