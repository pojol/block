#include "session.h"
#include "network_thread.h"
#include "session_mgr.h"

#include <iostream>

gsf::network::Session::Session(int32_t id, ::bufferevent *bev, int fd)
    : id_(id)
    , bev_(bev)
    , fd_(fd)
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
	evbuffer_add_cb(in_buf_, gsf::network::Session::read_buffer_cb, this);

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

void gsf::network::Session::change_read_state()
{
	need_read_ = false;
}

void gsf::network::Session::read_buffer_cb(::evbuffer *buffer, const ::evbuffer_cb_info *info, void *arg)
{
	if (info->n_deleted > 0){
		auto _session_ptr = static_cast<gsf::network::Session *>(arg);
		_session_ptr->change_read_state();
	}
}
