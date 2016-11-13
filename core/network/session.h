#ifndef _GSF_SESSION_HEADER_
#define _GSF_SESSION_HEADER_

#include <event2/bufferevent.h>
#include <stdint.h>
#include <memory>
#include <string>

namespace gsf
{
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
        Session(int32_t id, ::bufferevent *bev, int fd);

		int open(SessionHandler *session_handler, SessionCloseHandler *close_handler);

        static void read_cb(::bufferevent *bev, void *ctx);

		static void err_cb(::bufferevent *bev, short what, void *ctx);

		int send(const uint8_t *data, int len);
		
        int32_t get_id() const { return id_; }

		::evbuffer * get_outbuf() { return out_buf_; }
		::evbuffer * get_inbuf() { return in_buf_; }

		SessionHandler * get_session_handler() { return session_handler_; }

    private:
        uint32_t id_;
		
		int fd_;
		::bufferevent *bev_;

		::evbuffer *out_buf_;
		::evbuffer *in_buf_;

		SessionHandler *session_handler_;
		SessionCloseHandler *close_handler_;
	};
}


#endif
