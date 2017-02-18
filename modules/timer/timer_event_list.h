//
// Created by pojol on 2017/2/13.
//

#ifndef GSF_TIMER_EVENT_LIST_H_H
#define GSF_TIMER_EVENT_LIST_H_H

#include <stdint.h>

namespace event_id
{
	namespace timer
	{
		//! args uint32_t milliseconds
		const uint32_t delay_milliseconds = 1001;

		//! args uint32_t hour , uint32_t minute
		const uint32_t delay_day = 1002;

		//! args uint32_t day , uint32_t hour
		const uint32_t delay_week = 1003;

		//! args uint32_t day , uint32_t hour
		const uint32_t delay_month = 1004;

		//!
		const uint32_t remove_timer = 1005;

		//! result code
		const uint32_t make_timer_success = 1;
		const uint32_t make_timer_fail = 0;
	}
}

#endif //GSF_TIMER_EVENT_LIST_H_H
