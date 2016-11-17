#include "session.h"
#include "network_thread.h"

#include <event2/buffer.h>


#include <iostream>

gsf::Session::Session(int32_t id, ::bufferevent *bev, int fd, OBuffer *out_buffer, IBuffer *in_buffer)
    : id_(id)
    , bev_(bev)
    , fd_(fd)
	, session_handler_(nullptr)
	, close_handler_(nullptr)
{
	out_buffer_ = out_buffer;
	out_buffer->add_evbuffer(fd);

	in_buffer_ = in_buffer;
	in_buffer_->add_evbuffer(id);
}

gsf::Session::~Session()
{
	
}

int gsf::Session::open(SessionHandler *session_handler, SessionCloseHandler *close_handler)
{
	session_handler = session_handler;
	close_handler_ = close_handler;

	return 0;
}

void gsf::Session::read_cb(::bufferevent *bev, void *ctx)
{
	Session *_session_ptr = static_cast<Session*>(ctx);
	IBuffer *_in = _session_ptr->get_inbuf();

	bufferevent_read_buffer(bev, _in->find_evbuffer(_session_ptr->get_id()));
}

void gsf::Session::err_cb(::bufferevent *bev, short what, void *ctx)
{
	if (what & BEV_EVENT_EOF)  
	{  
		/* connection has been closed, do any clean up here */  
		printf("connection closed\n");  
	}  
	else if (what & BEV_EVENT_ERROR)  
	{  
		/* check errno to see what error occurred */  
		printf("some other error\n");  
	}  
	else if (what & BEV_EVENT_TIMEOUT)  
	{  
		/* must be a timeout event handle, handle it */  
		printf("Timed out\n");  
	}  
	bufferevent_free(bev);
}

void gsf::Session::write(const char *data, int len)
{
	out_buffer_->write_evbuffer(fd_, data, len);
}
