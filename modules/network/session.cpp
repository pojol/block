#include "session.h"
#include "session_mgr.h"

#include <iostream>

gsf::network::Session::Session(::bufferevent *bev, int fd)
    : bev_(bev)
    , fd_(fd)
{
	in_buf_ = evbuffer_new();
	out_buf_ = evbuffer_new();
}

gsf::network::Session::~Session()
{
	
}

void gsf::network::Session::read_cb(::bufferevent *bev, void *ctx)
{
	Session *_session_ptr = static_cast<Session*>(ctx);

	_session_ptr->read(bev);

	//evbuffer_add_buffer()
}

void gsf::network::Session::err_cb(::bufferevent *bev, short what, void *ctx)
{
	if (what & BEV_EVENT_EOF)
	{
		/* connection has been closed, do any clean up here */
		//printf("connection closed\n");

		Session * _session_ptr = static_cast<Session *>(ctx);
		_session_ptr->dis_connect();
	}
	else if (what & BEV_EVENT_ERROR)
	{
		/* check errno to see what error occurred */
		Session * _session_ptr = static_cast<Session *>(ctx);
		_session_ptr->dis_connect();
	}
	else if (what & BEV_EVENT_TIMEOUT)
	{
		/* must be a timeout event handle, handle it */
		printf("Timed out\n");
	}
	bufferevent_free(bev);
}

int gsf::network::Session::write(::evbuffer *data)
{
	evbuffer_write(data, fd_);
	return 0;
}

void gsf::network::Session::read(::bufferevent *bev)
{
	bufferevent_read_buffer(bev, in_buf_);

	//! 将完整的一个消息包取出，派发到对应的mgr中
	remote_callback(1001, std::string("hello"));
}

void gsf::network::Session::dis_connect()
{

}
