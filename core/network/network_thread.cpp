#include "network_thread.h"
#include "err.h"

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
