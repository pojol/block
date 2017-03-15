//
// Created by pojol on 2017/2/19.
//

#ifndef GSF_NETWORK_EVENT_LIST_H
#define GSF_NETWORK_EVENT_LIST_H

#include <stdint.h>

namespace event_id
{
	namespace network
	{
		const uint32_t make_acceptor = 2001;

		const uint32_t make_connector = 2002;

		const uint32_t bind_remote_callback = 2003;

		//! result code
		const uint32_t new_connect = 1;
		const uint32_t dis_connect = 2;
		const uint32_t fail_connect = 3;
	}
}


#endif //GSF_NETWORK_EVENT_LIST_H