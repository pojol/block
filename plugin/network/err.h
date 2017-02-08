#ifndef _ERR_HEADER_
#define _ERR_HEADER_

namespace gsf
{
	namespace network
	{
		enum Err
		{
			ACCEPTOR_CONSTRUCT_ERR = -1,
			ACCEPTOR_NOT_FIND = -2,
			LIBEVENT_NEW_BIND_ERR = -3,
			SESSION_NOT_FIND = -4,
			LIBEVENT_BUFFER_EVENT_CONSTRUCT_ERR = -5,
			CONNECTOR_CONSTRUCT_ERR = -6,
			ADD_EVBUFFER_ALREADLY_EXISTS = -7,
			RMV_EVBUFFER_NOT_FIND = -8,
		};
	}
}


#endif
