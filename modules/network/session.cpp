#include "session.h"
#include "session_mgr.h"
#include "msg_binder.h"

#include <args/args.h>

#include <iostream>

gsf::network::Session::Session(int fd, int module_id, MsgBinder *binder, std::function<void (int)> disconnect_callback)
    : fd_(fd)
	, module_id_(module_id)
	, binder_(binder)
{
	disconnect_callback_ = disconnect_callback;

	in_buf_ = evbuffer_new();
	if (0 != gsf::SESSION_READ_BUFFER_SIZE) {
		evbuffer_expand(in_buf_, gsf::SESSION_READ_BUFFER_SIZE);
	}

	out_buf_ = evbuffer_new();
	if (0 != gsf::SESSION_WRITE_BUFFER_SIZE) {
		evbuffer_expand(out_buf_, gsf::SESSION_WRITE_BUFFER_SIZE);
	}
}

gsf::network::Session::~Session()
{
	if (in_buf_) {
		evbuffer_free(in_buf_);
	}
	
	if (out_buf_) {
		evbuffer_free(out_buf_); 
	}
}

void gsf::network::Session::read_cb(::bufferevent *bev, void *ctx)
{
	Session *_session_ptr = static_cast<Session*>(ctx);

	_session_ptr->read(bev);

	//evbuffer_add_buffer()
}

void gsf::network::Session::err_cb(::bufferevent *bev, short what, void *ctx)
{
	int32_t _result = 0;

	do {
		if (what & BEV_EVENT_EOF) {
			_result = eid::network::err_event_eof;
			break;
		}

		if (what & BEV_EVENT_ERROR) {
			_result = eid::network::err_event_error;
			break;
		}

		if (what & BEV_EVENT_TIMEOUT) {
			_result = eid::network::err_event_timeout;
			break;
		}

	} while (0);

	if (0 != _result) {
		Session * _session_ptr = static_cast<Session *>(ctx);
		_session_ptr->dis_connect(_result);

		bufferevent_free(bev);
	}
}

int gsf::network::Session::write(uint32_t msg_id, BlockPtr blockptr)
{

	int _ret = evbuffer_add(out_buf_, blockptr->buf_, blockptr->size_);
	evbuffer_write(out_buf_, fd_);

	return 0;
}

void gsf::network::Session::read(::bufferevent *bev)
{
	bufferevent_read_buffer(bev, in_buf_);

	uint32_t _buf_len = evbuffer_get_length(in_buf_);

	char * _head = (char*)malloc(gsf::MSGLEN_PACKAGESIZE);
	evbuffer_copyout(in_buf_, _head, gsf::MSGLEN_PACKAGESIZE);

	uint32_t *_msg_size = reinterpret_cast<uint32_t*>(_head);

	evbuffer *_buff = nullptr;
	if (_buf_len >= *_msg_size) {
		_buff = evbuffer_new();
	}

	while (_buf_len >= *_msg_size)
	{
		evbuffer_remove_buffer(in_buf_, _buff, *_msg_size);

		evbuffer_remove(_buff, _head, gsf::MSGLEN_PACKAGESIZE);
		uint32_t _msg_len = *reinterpret_cast<uint32_t*>(_head);

		evbuffer_remove(_buff, _head, gsf::MSGLEN_MSGID);
		uint32_t _msg_id = *reinterpret_cast<MsgID*>(_head);

		//! 
		auto _blockptr = std::make_shared<Block>(_msg_len - (gsf::MSGLEN_PACKAGESIZE + gsf::MSGLEN_MSGID));
		evbuffer_remove(_buff, _blockptr->buf_, _blockptr->size_);

		auto _func = binder_->get_func(_msg_id);
		if (_func) {
			std::string _str(_blockptr->buf_, _blockptr->size_);	//tmp
			_func(gsf::Args(uint32_t(fd_), uint32_t(_msg_id), _str));
		}

		_buf_len = evbuffer_get_length(in_buf_);
		if (_buf_len > gsf::MSGLEN_PACKAGESIZE) {
			evbuffer_copyout(in_buf_, _head, gsf::MSGLEN_PACKAGESIZE);
			_msg_size = reinterpret_cast<uint32_t*>(_head);
		}
		else {
			break;
		}
	}
}

void gsf::network::Session::dis_connect(int32_t err)
{
	disconnect_callback_(fd_);

	gsf::Args args;
	args << uint32_t(fd_) << int32_t(err);
	dispatch(module_id_, eid::network::dis_connect, args);
}
