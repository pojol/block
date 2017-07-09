//
// Created by pojol on 2017/2/13.
//

#ifndef GSF_EVENT_LIST_H_H
#define GSF_EVENT_LIST_H_H

#include "types.h"

// 后面用工具自动生成
//! 用于描述框架中用到的事件, 用户可以以此为模板在自己工程中建立一个event_list 用于工程中的事件描述

namespace eid
{
	enum base
	{
		app_id = 1,					//! 每个进程中application module 的id ，因为application的作用范围只在自身进程所以 id 可以是固定的。
		get_app_name,
		get_module,					//! 通过字符串获得module的id， 只能获取静态显示声明的module。
		new_dynamic_module,			//! ͨ通过已经定义的module，创建多份实例。
		delete_dynamic_module,
		module_init_succ,
		regist_module,
	};


	enum error
	{
		err_repeated_fd = -10000,
		err_upper_limit_session,
		err_socket_new,
		err_socket_connect,
		err_event_eof,
		err_event_error,
		err_event_timeout,
		err_inet_pton,
	};

	enum network
	{
		make_acceptor = 1000,
		make_connector,
		kick_connect,
		connector_init,
		send,
		recv,
		//! result code
		new_connect,
		dis_connect,
		fail_connect,
	};

	enum distributed
	{
		sync_dispatch = 2001,

		create_node,

		distributed_state_change,
		node_state_change,
	};

	enum log
	{
		//const uint32_t init = 1001;	初始化改为在自己模块中实现，regist即初始化
		print = 3000,
	};

	enum timer
	{
		//! args {"uint32_t":module_id，"uint32_t":milliseconds}
		delay_milliseconds = 4000,

		//! args {"uint32_t":module_id, "uint32_t":hour, "uint32_t":minute}
		delay_day,

		//! args {"uint32_t":module_id, "uint32_t":day, "uint32_t":hour}
		delay_week,

		//! args {"uint32_t":module_id, "uint32_t":day, "uint32_t":hour}
		delay_month,

		//! args {"uint32_t":module_id, "uint32_t":eid}
		remove_timer,

		timer_arrive,
	};

	enum lua_proxy
	{
		create = 5000,

		reload,

		destroy,
	};

	enum db_proxy
	{
		redis_connect = 6000,
		redis_command_callback,
		redis_avatar_offline,
		redis_resume,
	};

	enum sample
	{
		get_proc_path = 7000,
	};
}

#endif