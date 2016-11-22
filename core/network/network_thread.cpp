#include "network_thread.h"
#include "err.h"
#include "session.h"
#include "session_mgr.h"

#include <event2/buffer.h>

#include <algorithm>

gsf::network::NetworkThread::NetworkThread()
	: th(nullptr)
	, event_base_ptr_(nullptr)
	, notify_event_(nullptr)
	, connect_queue_(nullptr)
{

}

gsf::network::NetworkThread::~NetworkThread()
{

}
