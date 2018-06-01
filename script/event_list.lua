
eid = {
	
	module = {
		init = 101,
		shut = 102,
	},

	distributed = {
		rpc_begin = 1001,
		
		coordinat_regist = 1002,
		coordinat_unregit = 1003,
		coordinat_adjust_weight = 1004,
		coordinat_select = 1005,

		rpc_end = 2000,
	},
	
	network = {
		tcp_make_acceptor = 2001,
		ws_make_acceptor = 2002,
		tcp_make_connector = 2003,
		ws_make_connector = 2004,
		kick_connect = 2005,
		send = 2006,
		recv = 2007,
		new_connect = 2008,
		dis_connect = 2009,
		fail_connect = 2010
	},

	dbProxy = {
		connect = 2401,
		execSql = 2402,
		insert = 2403,
		load = 2404,		
		update = 2405,
		callback = 2406
	},

	node = {
		node_create = 2500,
		node_create_succ = 2501,
		node_regist = 2502,
		node_regist_succ = 2503,
	},
}