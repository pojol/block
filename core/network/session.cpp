#include "session.h"
#include "network_thread.h"

#include <event2/buffer.h>


#include <iostream>

gsf::Session::Session(int32_t id, ::bufferevent *bev, int fd, OBuffer *out_buffer)
    : id_(id)
    , bev_(bev)
    , fd_(fd)
	, in_buf_(nullptr)
	, session_handler_(nullptr)
	, close_handler_(nullptr)
{
	out_buffer_ = out_buffer;
	out_buffer->add_evbuffer(fd);
}

gsf::Session::~Session()
{
	evbuffer_free(in_buf_);
}

int gsf::Session::open(SessionHandler *session_handler, SessionCloseHandler *close_handler)
{
	session_handler = session_handler;
	close_handler_ = close_handler;

	in_buf_ = evbuffer_new();

	return 0;
}

void gsf::Session::read_cb(::bufferevent *bev, void *ctx)
{
	char buf[1024];
	int len;

	Session *_session_ptr = static_cast<Session*>(ctx);
	evbuffer *_in = _session_ptr->get_inbuf();
	if (_in){
		bufferevent_read_buffer(bev, _in);

		while ((len = evbuffer_remove(_in, buf, sizeof(buf))) > 0)
		{
			//test 
			uint8_t *read = new uint8_t[len];
			memcpy(read, buf, len);

			if (_session_ptr->get_session_handler()){
				_session_ptr->get_session_handler()->handle_data(read);
			}
			else {
				printf("not bind session_handler! %d\n", _session_ptr->get_id());
			}

			delete[]read;
		}
	}
	else {
		printf("read err, not open session!\n");
	}
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
