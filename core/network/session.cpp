#include "session.h"
#include "network_thread.h"
#include "session_mgr.h"

#include <event2/buffer.h>

#include <iostream>

gsf::network::Session::Session(int32_t id, ::bufferevent *bev, int fd)
    : id_(id)
    , bev_(bev)
    , fd_(fd)
	, session_handler_(nullptr)
	, close_handler_(nullptr)
	, need_write_(false)
	, need_read_(false)
{
}

gsf::network::Session::~Session()
{
	
}


int gsf::network::Session::init(IBuffer *ibuffer, OBuffer *obuffer)
{
	in_buf_ = evbuffer_new();
	out_buf_ = evbuffer_new();

	thread_in_buffer_ = ibuffer;
	thread_out_buffer_ = obuffer;

	return 0;
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

int gsf::network::Session::write(::evbuffer *data)
{
	evbuffer_write(data, fd_);
	return 0;
}

void gsf::network::Session::read(::bufferevent *bev)
{
	if (!need_read_){
		thread_in_buffer_->mark_produce(id_, in_buf_);
	}

	bufferevent_read_buffer(bev, in_buf_);

	need_read_ = true;
}
