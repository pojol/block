#include "network_thread.h"
#include "err.h"
#include "session.h"
#include "session_mgr.h"

#include <event2/buffer.h>

#include <algorithm>

//debug
#include <iostream>
#include <assert.h>

gsf::network::NetworkThread::NetworkThread(int index)
	: th(nullptr)
	, event_base_ptr_(nullptr)
	, notify_event_(nullptr)
	, connect_queue_(nullptr)
	, in_buffer_(nullptr)
	, out_buffer_(nullptr)
	, session_mgr(nullptr)
	, index_(index)
{
	session_mgr = new SessionMgr(index * 100000);

	in_buffer_ = new IBuffer();
	out_buffer_ = new OBuffer();
}

gsf::network::NetworkThread::~NetworkThread()
{

}

void gsf::network::IBuffer::mark_produce(uint32_t session_id, evbuffer *buff)
{
	assert(session_id != 0);
	ibuffer_vec_.push_back(std::make_pair(session_id, buff));
}


void gsf::network::IBuffer::new_connect(uint32_t session_id)
{
	mtx.lock();
	connect_vec_.push_back(session_id);
	mtx.unlock();
}

gsf::network::IBuffer::IBuffer()
{
	recvbuf_ = (char*)malloc(4);	//test
}

gsf::network::IBuffer::~IBuffer()
{
	if (recvbuf_){
		free(recvbuf_);
	}
}

void gsf::network::IBuffer::produce()
{
	for (auto itr = ibuffer_vec_.begin(); itr != ibuffer_vec_.end();)
	{
		uint32_t _pack_len = evbuffer_get_length(itr->second);
		evbuffer *buff = nullptr;

		evbuffer_copyout(itr->second, recvbuf_, 4);
		uint32_t *_msg_size = reinterpret_cast<uint32_t *>(recvbuf_);
		if (_pack_len >= *_msg_size){
			buff = evbuffer_new();
		}

		while (_pack_len >= *_msg_size)
		{
			evbuffer_remove_buffer(itr->second, buff, *_msg_size);

			_pack_len = evbuffer_get_length(itr->second);
			if (_pack_len >= 4){
				evbuffer_copyout(itr->second, recvbuf_, 4);
				_msg_size = reinterpret_cast<uint32_t *>(recvbuf_);
			}
			else {
				break;
			}
		}

		mtx.lock();
		if (buff){
			consume_vec_.push_back(std::make_pair(itr->first, buff));
		}
		mtx.unlock();

		itr = ibuffer_vec_.erase(itr);
	}
}

void gsf::network::IBuffer::consume(std::vector<std::pair<uint32_t, evbuffer*>> &vec, std::vector<uint32_t> &conn)
{
	mtx.lock();
	if (!consume_vec_.empty()){

		consume_vec_.swap(vec);

		consume_vec_.clear();
	}

	if (!connect_vec_.empty()){
	
		connect_vec_.swap(conn);

		connect_vec_.clear();
	}

	mtx.unlock();
}

gsf::network::OBuffer::OBuffer()
:thread_count_(0)
{

}


gsf::network::OBuffer::~OBuffer()
{

}


void gsf::network::OBuffer::write(uint32_t session_id, const char *data, int len)
{
	//! temp
	evbuffer *buff = evbuffer_new();
	evbuffer_add(buff, data, len);

	uint32_t _index = static_cast<uint32_t>(session_id / 100000);
	thread_write_vec_[_index].push_back(std::make_pair(session_id, buff));
}

void gsf::network::OBuffer::mian_thread_init(int threads)
{
	thread_count_ = threads;
	for (int i = 0; i < threads; ++i)
	{
		ProduceVec write_vec;
		thread_write_vec_.push_back(write_vec);
		
		ProduceVec produce_vec;
		thread_produce_vec_.push_back(produce_vec);
	}
}

void gsf::network::OBuffer::produce()
{
	mtx.lock();

	for (uint32_t i = 0; i < thread_count_; ++i)
	{
		if (!thread_write_vec_[i].empty()){
			thread_write_vec_[i].swap(thread_produce_vec_[i]);
			thread_write_vec_[i].clear();
		}
	}

	mtx.unlock();
}

void gsf::network::OBuffer::consume(uint32_t thread_index, ProduceVec &vec)
{
	mtx.lock();

	if (!thread_produce_vec_[thread_index].empty()){
		thread_produce_vec_[thread_index].swap(vec);

		thread_produce_vec_[thread_index].clear();
	}

	mtx.unlock();
}


