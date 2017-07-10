
eid = {
	app_id = 1,
	get_module = 101,
	new_dynamic_module = 102,
	delete_dynamic_module = 103,
	
	log = {
		init = 1001,
		info = 1002,
		warning = 1003,
		error = 1004,
		log_callback = 1005,
	},
	
	network = {
		make_acceptor = 2001,
		make_connector = 2002,
		recv_remote_callback = 2003,
		send_remote_callback = 2004,
        kick_connect = 2005,
        connector_init = 2006,
		
		new_connect = 2007,
		dis_connect = 2008,
		fail_connect = 2009,
		
		err_repeated_fd = -2001,
		err_upper_limit_session = -2002,
		err_socket_new = -2003,
		err_socket_connect = -2004,
	},
	
	timer = {
		delay_milliseconds = 3001,
		delay_day = 3002,
		delay_week = 3003,
		remove_timer = 3005,
		timer_arrive = 3006,
	},
}