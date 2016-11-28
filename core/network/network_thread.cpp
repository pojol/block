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


void gsf::network::IBuffer::produce()
{
	for (auto itr = ibuffer_vec_.begin(); itr != ibuffer_vec_.end();)
	{
		//! 这个buffer可以不用new 后续优化
		evbuffer *buff = evbuffer_new();
		evbuffer_add_buffer(buff, itr->second);

		//! 这里需要验证下buffer 的完整性，如果不是一个整包则不压入消费队列。

		mtx.lock();
		consume_vec_.push_back(std::make_pair(itr->first, buff));
		mtx.unlock();

		itr = ibuffer_vec_.erase(itr);
		//++itr;
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

void gsf::network::OBuffer::write(uint32_t session_id, const char *data, int len)
{
	//! temp
	evbuffer *buff = evbuffer_new();
	evbuffer_add(buff, data, len);

	uint32_t _index = static_cast<uint32_t>(session_id / 10000);
	thread_write_vec_[_index].push_back(std::make_pair(session_id, buff));
}

void gsf::network::OBuffer::mian_thread_init(int threads)
{
	for (int i = 0; i < threads; ++i)
	{
		ProduceVec vec;
		thread_write_vec_.push_back(vec);
	}
}

void gsf::network::OBuffer::produce()
{
	mtx.lock();

	if (!thread_write_vec_.empty()){
		thread_write_vec_.swap(thread_produce_vec_);
		thread_write_vec_.clear();
	}

	mtx.unlock();
}

void gsf::network::OBuffer::consume(uint32_t thread_index, ProduceVec &vec)
{
	mtx.lock();

	if (!thread_produce_vec_.empty()){
		if (!thread_produce_vec_[thread_index].empty()){
			thread_produce_vec_[thread_index].swap(vec);

			thread_produce_vec_[thread_index].clear();
		}
	}

	mtx.unlock();
}


