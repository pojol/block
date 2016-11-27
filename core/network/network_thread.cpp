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

void gsf::network::IBuffer::consume(std::vector<std::pair<uint32_t, evbuffer*>> &vec)
{
	mtx.lock();
	if (!consume_vec_.empty()){

		consume_vec_.swap(vec);

		consume_vec_.clear();
	}
	mtx.unlock();
}
