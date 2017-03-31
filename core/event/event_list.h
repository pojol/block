//
// Created by pojol on 2017/2/13.
//

#ifndef GSF_EVENT_LIST_H_H
#define GSF_EVENT_LIST_H_H

namespace eid
{
	const uint32_t app_id = 1;

	const uint32_t get_module = 101;

	namespace log
	{
		const uint32_t init = 1001;

		const uint32_t info = 1002;
		const uint32_t warning = 1003;
		const uint32_t error = 1004;
	}

	namespace network
	{
		const uint32_t make_acceptor = 2001;
		const uint32_t make_connector = 2002;
		const uint32_t bind_remote_callback = 2003;

		//! result code
		const uint32_t new_connect = 2004;
		const uint32_t dis_connect = 2005;
		const uint32_t fail_connect = 2006;

		const int32_t err_repeated_fd = -2001;				// repeated fd
		const int32_t err_upper_limit_session = -2002;		// upper limit session!
		const int32_t err_socket_new = -2003;				// bufferevent_socket_new err!
		const int32_t err_socket_connect = -2004;			// bufferevent_socket_connect err!
	}

	namespace timer
	{
		//! args {"uint32_t":module_id£¬"uint32_t":milliseconds}
		const uint32_t delay_milliseconds = 3001;

		//! args {"uint32_t":module_id, "uint32_t":hour, "uint32_t":minute}
		const uint32_t delay_day = 3002;

		//! args {"uint32_t":module_id, "uint32_t":day, "uint32_t":hour}
		const uint32_t delay_week = 3003;

		//! args {"uint32_t":module_id, "uint32_t":day, "uint32_t":hour}
		const uint32_t delay_month = 3004;

		//! args {"uint32_t":module_id, "uint32_t":eid}
		const uint32_t remove_timer = 3005;


		//! result code
		const uint32_t make_timer_success = 3006;
		const uint32_t make_timer_fail = 3007;
	}

	namespace lua_proxy
	{
		const uint32_t create = 4001;

		const uint32_t reload = 4002;

		const uint32_t destroy = 4003;
	}
}

#endif