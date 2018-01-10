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

		/*!
			通过字符串获得module的id， 只能获取静态显示声明的module。
		**/
		get_module,

		/*!
			通过已经定义的module，创建多份实例。
		**/
		new_dynamic_module,
		
		/*!
			移除某个被注册的module
		**/
		delete_module,

		module_init_succ,
		module_shut_succ,
	};

	enum distributed
	{
		rpc_begin = 1001,

		/*!
			将Node绑定到Coordinator
			参数: nodeType, nodeID, {[moduleName, moduleID, moduleFeature] ... }
			类型: rpc
		**/
		coordinat_regist,
		
		/*!
			将Node从Coordinator解除绑定
		**/
		coordinat_unregit,

		/*!
			调整Node在Coordinator中的权重
			参数: nodeID, moduleName, moduleFeature, moduleWeight
			类型: rpc
		**/
		coordinat_adjust_weight,

		/*!
			通过ModuleName和ModuleFeature选取一个适合自己的Node
			参数: moduleName, moduleFeature
			类型: rpc
		**/
		coordinat_select,

		rpc_end,
	};

	enum network
	{
		/*!
			创建一个接收器
			参数: module_id, ip, port
			类型: dispatch
		**/
		make_acceptor = 2001,

		/*!
			创建一个连接器
			参数: module_id, ip, port
			类型: dispatch
		**/
		make_connector,

		/*!
			踢掉某个现有的连接
			参数: fd
			类型: dispatch
		**/
		kick_connect,

		/*!
			发送一条网络消息
			参数: fd, msgid, block
			类型: dispatch
		**/
		send,

		/*!
			接收到一条网络消息
			参数: fd, msgid, block
			类型: listen
		**/
		recv,
		
		/*!
			接收到一个新的连接（适用于 acceptor
			参数: fd
			类型: listen
		**/
		new_connect,

		/*!
			连接被断开
			参数: fd
			类型: listen
		**/
		dis_connect,

		/*!
			连接失败（适用于 connector
			参数: string
			类型: listen
		**/
		fail_connect,
	};

	enum log
	{
		//const uint32_t init = 1001;	初始化改为在自己模块中实现，regist即初始化
		print = 2101,
	};

	enum timer
	{
		/*!
			延迟若干毫秒触发
			参数: module_id, milliseconds
			类型: dispatch
		**/
		delay_milliseconds = 2201,

		/*!
			延时一天触发， hour & minute 可以指定隔天的触发点
			参数: module_id, hour, minute
			类型: dispatch
		**/
		delay_day,

		delay_week,

		delay_month,

		/*!
			从定时器中移除
			参数: timer_id
			类型: dispatch
		**/
		remove_timer,

		/*!
			触发Timer
			参数: timer_id
			类型: listen
		**/
		timer_arrive,
	};

	enum lua_proxy
	{
		/*!
			创建 Lua Script Module ,proxy会自动完成c++/lua的相关绑定 (userdata Args, interface : dispatch, listen, rpc)
			参数: proxy module_id, script_dir, script_name
			类型: dispatch
		**/
		create = 2301,

		/*!
			重新装载 Lua Script Module, 会走标准的退出和进入流程 init, shut 。即便持有状态也可以方便的热更
			参数: module_id
			类型: dispatch
		**/
		reload,

		/*!
			移除 Lua Script Module
			参数: module_id
			类型: dispatch
		**/
		destroy,
	};

	enum db_proxy
	{
		redis_connect = 2401,
		redis_command,
		redis_avatar_offline,
		redis_resume,

		/*!
			建立一个新的Mysql连接
			参数: host, user, password, dbName, port
			类型: dispatch
		**/
		mysql_connect,

		mysql_update,
		mysql_execute,

		// local
		mysql_callback,
	};

	enum node
	{
		node_create = 2500,
		node_create_succ,
		node_regist,
		node_regist_succ,
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
		err_distributed_node_repeat,
	};

	enum sample
	{
		get_proc_path = 7000,
		get_cfg,
	};
}

#endif