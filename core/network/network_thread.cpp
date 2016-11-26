#include "network_thread.h"
#include "err.h"
#include "session.h"
#include "session_mgr.h"

#include <event2/buffer.h>

#include <algorithm>

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
	ibuffer_vec_.push_back(std::make_pair(session_id, buff));
}

void gsf::network::IBuffer::ready_consume()
{
	for (auto &it : ibuffer_vec_)
	{
		evbuffer *buff = evbuffer_new();
		evbuffer_add_buffer(buff, it.second);

		int len = evbuffer_get_length(buff);
		int len1 = evbuffer_get_length(it.second);

		consume_vec_.push_back(std::make_pair(it.first, buff));
	}

	ibuffer_vec_.clear();
}

void gsf::network::IBuffer::consume()
{

	for (auto itr = consume_vec_.begin(); itr != consume_vec_.end();)
	{
		int len = evbuffer_get_length(itr->second);
		char *buff = (char*)malloc(len);
		evbuffer_remove(itr->second, buff, len);

		//printf("session : %d len : %d recv : %s \n", itr->first, len, buff);

		//! 如果buff被拿完则从vec中删除
		if (len == 1){
			itr = consume_vec_.erase(itr);
		}
		else {
			++itr;
		}
	}

	//debug
	printf("thread id : %d \n", std::this_thread::get_id());
}
