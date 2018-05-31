#include "session.h"
#include "sessionMgr.h"

#include <core/application.h>

#include <iostream>

block::network::Session::Session(int fd, int eid, SessionMgr *mgr, ::bufferevent *bev)
    : fd_(fd)
	, targetM_(eid)
	, bufEvtPtr_(bev)
	, basePtr_(mgr)
{
	inBufPtr_ = evbuffer_new();
	if (0 != block::SESSION_READ_BUFFER_SIZE) {
		evbuffer_expand(inBufPtr_, block::SESSION_READ_BUFFER_SIZE);
	}

	outBufPtr_ = evbuffer_new();
	if (0 != block::SESSION_WRITE_BUFFER_SIZE) {
		evbuffer_expand(outBufPtr_, block::SESSION_WRITE_BUFFER_SIZE);
	}
}

block::network::Session::~Session()
{
	if (inBufPtr_) {
		evbuffer_free(inBufPtr_);
	}
	
	if (outBufPtr_) {
		evbuffer_free(outBufPtr_);
	}

	if (bufEvtPtr_) {
		bufferevent_free(bufEvtPtr_);
	}
}

void block::network::Session::readCB(::bufferevent *bev, void *ctx)
{
	Session *_session_ptr = static_cast<Session*>(ctx);

	_session_ptr->read(bev);

	//evbuffer_add_buffer()
}

void block::network::Session::eventCB(::bufferevent *bev, short what, void *ctx)
{
	int32_t _result = 0;

	do {
		if (what & BEV_EVENT_EOF) {
			_result = eid::error::err_event_eof;
			break;
		}

		if (what & BEV_EVENT_ERROR) {
			_result = eid::error::err_event_error;
			break;
		}

		if (what & BEV_EVENT_TIMEOUT) {
			_result = eid::error::err_event_timeout;
			break;
		}

		if (what & BEV_EVENT_CONNECTED) {
			Session * _session_ptr = static_cast<Session *>(ctx);
			_session_ptr->newConnect();
			break;
		}

	} while (0);

	if (0 != _result) {
		Session * _session_ptr = static_cast<Session *>(ctx);
		_session_ptr->disConnect(_result);
	}
}

int block::network::Session::write(uint32_t msg_id, BlockPtr blockptr)
{

	int _ret = evbuffer_add(outBufPtr_, blockptr->buf_, blockptr->size_);
	evbuffer_write(outBufPtr_, fd_);

	return 0;
}

void block::network::Session::read(::bufferevent *bev)
{
	bufferevent_read_buffer(bev, inBufPtr_);

	uint32_t _buf_len = evbuffer_get_length(inBufPtr_);

	uint32_t _msgheadlen = sizeof(MsgHeadLen);

	char * _head = (char*)malloc(_msgheadlen);
	evbuffer_copyout(inBufPtr_, _head, _msgheadlen);

	uint32_t _msg_size = *reinterpret_cast<MsgHeadLen*>(_head);

	if (_buf_len >= _msg_size) {

		while (_buf_len >= _msg_size)
		{
			auto _block = std::make_shared<Block>(_msg_size);
			evbuffer_remove(inBufPtr_, _block->buf_, _msg_size);	//! 将数据流拷贝到msg block中

			MsgID _msg_id = _block->get_msg_id();

			if (_msg_id > eid::distributed::rpc_begin && _msg_id < eid::distributed::rpc_end) {
			/*
				auto args_ptr = ArgsPool::get_ref().get();
				args_ptr->push(fd_);
				args_ptr->push(_msg_id);
				args_ptr->push_block(_block->buf_ + _block->get_head_size(), _block->get_body_size());

				basePtr_->addMessage(std::move(args_ptr));
			*/
			}
			else {
				if (!_block->check()) { //! 先这样检查下block中的内容是否合法，后面肯定不能这样明文传输
					break;
				}

				std::string _str(_block->buf_ + _block->get_head_size(), _block->get_body_size());	//tmp
				//dispatch(targetM_, eid::network::recv, block::makeArgs(fd_, _msg_id, std::move(_str)));
				basePtr_->addMessage(block::makeArgs(fd_, _msg_id, std::move(_str)));
			}

			_buf_len = evbuffer_get_length(inBufPtr_);
			if (_buf_len > _msgheadlen) {
				evbuffer_copyout(inBufPtr_, _head, _msgheadlen);
				_msg_size = *reinterpret_cast<uint32_t*>(_head);
			}
			else {
				break;
			}
		}

	}
}

void block::network::Session::disConnect(int32_t err)
{
	basePtr_->addClose(fd_);
	//dispatch(targetM_, eid::network::dis_connect, block::makeArgs(fd_, err));
}

void block::network::Session::newConnect()
{
	basePtr_->addConnect(fd_);
	//dispatch(targetM_, eid::network::new_connect, block::makeArgs(fd_));
}