//
// Created by pojol on 2017/2/13.
//

#ifndef GSF_LOG_EVENT_LIST_H_H
#define GSF_LOG_EVENT_LIST_H_H

#include <stdint.h>

namespace event_id
{
	namespace log
	{
		const uint32_t init = 3001;
		
		const uint32_t info = 3002;
		const uint32_t warning = 3003;
		const uint32_t error = 3004;
	}
}

#endif