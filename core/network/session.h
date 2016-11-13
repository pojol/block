#ifndef _GSF_SESSION_HEADER_
#define _GSF_SESSION_HEADER_

#include <event2/bufferevent.h>
#include <stdint.h>

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

        static void read_cb(::bufferevent *bev, void *ctx);

		static void err_cb(::bufferevent *bev, short what, void *ctx);

		int send(const char *data, int len);
		
        int32_t getid() const { return id_; }

    private:
        uint32_t id_;
		
	int fd_;
		
	::bufferevent *bev_;
	};
}


#endif
