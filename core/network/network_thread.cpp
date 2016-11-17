#include "network_thread.h"
#include "err.h"
#include "session.h"
#include "session_mgr.h"

#include <event2/buffer.h>

#include <algorithm>

gsf::NetworkThread::NetworkThread()
	: th(nullptr)
	, event_base_ptr_(nullptr)
	, notify_event_(nullptr)
	, connect_queue_(nullptr)
	, out_buffer_(nullptr)
{

}

gsf::NetworkThread::~NetworkThread()
{

}

int gsf::OBuffer::add_evbuffer(int fd)
{
	auto _itr = buffer_map_.find(fd);
	if (_itr == buffer_map_.end()){
		buffer_map_.insert(std::make_pair(fd, evbuffer_new()));
	}
	else {
		return ADD_EVBUFFER_ALREADLY_EXISTS;
	}

	return 0;
}

int gsf::OBuffer::rmv_evbuffer(int fd)
{
	auto _itr = buffer_map_.find(fd);
	if (_itr != buffer_map_.end()){
		evbuffer_free(_itr->second);
		buffer_map_.erase(_itr);

		auto active_itr = std::find_if(active_buffer_vec_.begin(), active_buffer_vec_.end(), [&](ActiveBufferVec::value_type it){
			return (it.second == fd);
		});

		if (active_itr != active_buffer_vec_.end()){
			active_buffer_vec_.erase(active_itr);
		}
	}
	else {
		return RMV_EVBUFFER_NOT_FIND;
	}

	return 0;
}

void gsf::OBuffer::write_evbuffer(int fd, const char *data, int len)
{
	auto _itr = buffer_map_.find(fd);
	if (_itr != buffer_map_.end()){
		evbuffer_add(_itr->second, data, len);

		active_buffer_vec_.push_back(std::make_pair(_itr->second, fd));
	}
}

void gsf::OBuffer::send_evbuffer()
{
	if (!active_buffer_vec_.empty()){
		for (auto &buf : active_buffer_vec_)
		{
			evbuffer_write(buf.first, buf.second);
		}
		active_buffer_vec_.clear();
	}
}

int gsf::IBuffer::add_evbuffer(int session_id)
{
	auto _itr = buffer_map_.find(session_id);
	if (_itr == buffer_map_.end()){
		buffer_map_.insert(std::make_pair(session_id, evbuffer_new()));
	}
	else {
		return ADD_EVBUFFER_ALREADLY_EXISTS;
	}

	return 0;
}

int gsf::IBuffer::rmv_evbuffer(int session_id)
{
	auto _itr = buffer_map_.find(session_id);
	if (_itr != buffer_map_.end()){
		evbuffer_free(_itr->second);
		buffer_map_.erase(_itr);

		auto active_itr = std::find_if(active_buffer_vec_.begin(), active_buffer_vec_.end(), [&](ActiveBufferVec::value_type it){
			return (it.second == session_id);
		});

		if (active_itr != active_buffer_vec_.end()){
			active_buffer_vec_.erase(active_itr);
		}
	}
	else {
		return RMV_EVBUFFER_NOT_FIND;
	}

	return 0;
}

evbuffer * gsf::IBuffer::find_evbuffer(int session_id)
{
	auto _itr = buffer_map_.find(session_id);
	if (_itr != buffer_map_.end()){

		active_buffer_vec_.push_back(std::make_pair(_itr->second, session_id));

		return _itr->second;
	}
	else {
		return nullptr;
	}
}

void gsf::IBuffer::push_evbuffer(ActiveBufferVec &vec)
{
	lock.lock();

	if (!vec.empty()){

		for (auto &buf : vec)
		{
			auto _itr = buffer_map_.find(buf.second);
			if (_itr != buffer_map_.end()){
				evbuffer_add_buffer(_itr->second, buf.first);
				active_buffer_vec_.push_back(std::make_pair(_itr->second, buf.second));
			}
		}

		vec.clear();
	}

	lock.unlock();
}

void gsf::IBuffer::pop_evbuffer()
{
	for (auto &buf : active_buffer_vec_)
	{
		SessionPtr _session_ptr = SessionMgr::instance().find(buf.second);
		if (_session_ptr){
			char data[1024];
			int len;

			while ((len = evbuffer_remove(buf.first, data, sizeof(data))) > 0)
			{
				//test
				if (_session_ptr->get_session_handler()){
					_session_ptr->get_session_handler()->handle_data(data);
				}
				else {
					printf("not bind session_handler! %d\n", _session_ptr->get_id());
				}
			}
		}
	}
}