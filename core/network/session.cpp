#include "session.h"


gsf::Session::Session(int32_t id, ::bufferevent *bev, int fd)
    : id_(id)
    , bev_(bev)
    , fd_(fd)
{}

void gsf::Session::read_cb(::bufferevent *bev, void *ctx)
{
	char buf[1024];
	int len;

	struct evbuffer *input = bufferevent_get_input(bev);
	while ((len = evbuffer_remove(input, buf, sizeof(buf))) > 0)
	{
		char *read = new char[len];
		memcpy(read, buf, len);
		std::cout << read << std::endl;
		delete []read;
	}

	Session *session = static_cast<Session*>(ctx);
	session->send();
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

void gsf::Session::send(const char *data, int len)
{
	struct evbuffer *output = evbuffer_new();

	evbuffer_add(output, "hello", 5);

	evbuffer_write(output, fd_);

	delete output;
}
