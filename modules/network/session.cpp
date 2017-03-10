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

	uint32_t _buf_len = evbuffer_get_length(in_buf_);

	char * _head = (char*)malloc(MSG_SIZE_LEN);
	evbuffer_copyout(in_buf_, _head, MSG_SIZE_LEN);

	uint32_t *_msg_size = reinterpret_cast<uint32_t*>(_head);

	evbuffer *_buff = nullptr;
	if (_buf_len >= *_msg_size) {
		_buff = evbuffer_new();
	}

	while (_buf_len >= *_msg_size)
	{
		evbuffer_remove_buffer(in_buf_, _buff, *_msg_size);

		//! 取出消息id
		evbuffer_remove(_buff, _head, MSG_SIZE_LEN);
		uint32_t _msg_len = *reinterpret_cast<uint32_t*>(_head);
		evbuffer_remove(_buff, _head, MSG_SIZE_LEN);
		uint32_t _msg_id = *reinterpret_cast<uint32_t*>(_head);
		//! 
		remote_callback(_msg_id, std::string("hello"));

		_buf_len = evbuffer_get_length(in_buf_);
		if (_buf_len > MSG_SIZE_LEN) {
			evbuffer_copyout(in_buf_, _head, MSG_SIZE_LEN);
			_msg_size = reinterpret_cast<uint32_t*>(_head);
		}
		else {
			break;
		}
	}
}

void gsf::network::Session::dis_connect()
{

}
