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
	session_mgr = new SessionMgr(index * SESSION_MAX_CONNECT);

	in_buffer_ = new IBuffer();
	out_buffer_ = new OBuffer();
}

gsf::network::NetworkThread::~NetworkThread()
{
	if (th){
		th->join();
		delete th;
		th = nullptr;
	}

	if (connect_queue_){
		delete connect_queue_;
		connect_queue_ = nullptr;
	}

	if (in_buffer_){
		delete in_buffer_;
		in_buffer_ = nullptr;
	}

	if (out_buffer_){
		delete out_buffer_;
		out_buffer_ = nullptr;
	}

	if (session_mgr){
		delete session_mgr;
		session_mgr = nullptr;
	}

	event_del(notify_event_);

	event_base_free(event_base_ptr_);
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

void gsf::network::IBuffer::dis_connect(uint32_t session_id)
{
	mtx.lock();
	disconn_vec_.push_back(session_id);
	mtx.unlock();
}

void gsf::network::IBuffer::fail_connect(const std::string &ip, uint32_t port)
{
	mtx.lock();
	failconn_vec_.push_back(std::make_pair(ip, port));
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
	for (auto itr = ibuffer_vec_.begin(); itr != ibuffer_vec_.end(); ++itr)
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
	}

	ibuffer_vec_.clear();
}

void gsf::network::IBuffer::consume(std::vector<std::pair<uint32_t, evbuffer*>> &vec 
	, std::vector<uint32_t> &conn 
	, std::vector<uint32_t> &disconn
	, std::vector<std::pair<std::string, uint32_t>> &failconn)
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

	if (!disconn_vec_.empty()){
		disconn_vec_.swap(disconn);
		disconn_vec_.clear();
	}

	if (!failconn_vec_.empty()){
		failconn_vec_.swap(failconn);
		failconn_vec_.clear();
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
	queue_push(session_id, data, len);

	//! temp
	/*
	evbuffer *buff = evbuffer_new();
	evbuffer_add(buff, data, len);

	uint32_t _index = static_cast<uint32_t>(session_id / SESSION_MAX_CONNECT);
	thread_write_vec_[_index].push_back(std::make_pair(session_id, buff));
	*/
}

void gsf::network::OBuffer::queue_push(uint32_t session_id, const char *data, int len)
{
	evbuffer *_buf;

	auto _push_back = [&](evbuffer * buf){
		uint32_t _threads = static_cast<uint32_t>(session_id / SESSION_MAX_CONNECT);
		active_write_list_[_threads].push_back(std::make_pair(session_id, buf));
	};

	auto itr = active_index_map_.find(session_id);
	if (itr == active_index_map_.end()){
		_buf = evbuffer_new();
		evbuffer_add(_buf, data, len);

		active_index_map_.insert(std::make_pair(session_id, _buf));
		_push_back(_buf);
	}
	else{
		_buf = itr->second;
		uint32_t _olen = evbuffer_get_length(_buf);
		evbuffer_add(_buf, data, len);


		if (_olen == 0){
			_push_back(_buf);
		}
	}
}

void gsf::network::OBuffer::mian_thread_init(int threads)
{
	thread_count_ = threads;
	for (int i = 0; i < threads; ++i)
	{
		/*
		ProduceVec write_vec;
		thread_write_vec_.push_back(write_vec);
		
		ProduceVec produce_vec;
		thread_produce_vec_.push_back(produce_vec);
		*/
		ProduceVec active_vec;
		active_write_list_.push_back(active_vec);
		ProduceVec send_vec;
		send_write_list_.push_back(send_vec);
	}
}

void gsf::network::OBuffer::produce()
{
	mtx.lock();

	for (uint32_t i = 0; i < thread_count_; ++i)
	{
		if (!active_write_list_[i].empty()){

			for (auto itr = active_write_list_[i].begin(); itr != active_write_list_[i].end(); ++itr)
			{
				auto _sitr = send_index_map_.find(itr->first);
				if (_sitr == send_index_map_.end()){
					evbuffer * _buff = evbuffer_new();
					evbuffer_add_buffer(_buff, itr->second);

					send_index_map_.insert(std::make_pair(itr->first, _buff));
					send_write_list_[i].push_back(std::make_pair(itr->first, _buff));
				}
				else {
					uint32_t _len = evbuffer_get_length(_sitr->second);
					evbuffer_add_buffer(_sitr->second, itr->second);	//move buffer
					if (_len == 0){
						send_write_list_[i].push_back(std::make_pair(itr->first, _sitr->second));
					}
				}
			}

			active_write_list_[i].clear();
		}

		/*
		if (!thread_write_vec_[i].empty()){
			thread_write_vec_[i].swap(thread_produce_vec_[i]);
			thread_write_vec_[i].clear();
		}
		*/
	}

	mtx.unlock();
}

void gsf::network::OBuffer::consume(uint32_t thread_index, ProduceVec &vec)
{
	mtx.lock();

	if (!send_write_list_[thread_index].empty()){

		send_write_list_[thread_index].swap(vec);
		send_write_list_[thread_index].clear();

	}

	/*
	if (!thread_produce_vec_[thread_index].empty()){
		thread_produce_vec_[thread_index].swap(vec);

		thread_produce_vec_[thread_index].clear();
	}
	*/

	mtx.unlock();
}


