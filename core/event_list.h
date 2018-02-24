//
// Created by pojol on 2017/2/13.
//

#pragma once

#include "types.h"

// 后面用工具自动生成
//! 用于描述框架中用到的事件, 用户可以以此为模板在自己工程中建立一个event_list 用于工程中的事件描述

namespace eid
{
	enum base
	{
		/*!
			comment: Module初始化成功
			args: nil
			type: boardcast
			res : int32_t module_id
		**/
		module_init_succ,

		/*!
			comment: Module退出成功
			args: nil
			type: boardcast
			res : int32_t module_id
		**/
		module_shut_succ,
	};

	//! 凡是gsf集群内使用rpc交互的消息都应该定义在distributed区块
	enum distributed
	{
		rpc_begin = 1001,

		/*!
			comment: 将Node绑定到Coordinator
			args: string node_type, int32_t node_id, string root_ip, int32_t root_port, [{string module_name, int32_t module_id, int32_t module_fature} ... ]
			type: rpc
			res : stream args, int32_t progress, bool succ
		**/
		coordinat_regist,
		
		/***/
		coordinat_unregit,

		/*!
			comment: 调整Node在Coordinator中的权重
			args: int32_t node_id, string module_name, int32_t module_fature, int32_t +- weight 
			type: rpc
			res : stream args, int32_t progress, bool succ
		**/
		coordinat_adjust_weight,

		/*!
			comment: 通过ModuleName和ModuleFeature选取一个集群中适合自己的Node
			args: string module_name, int32_t module_fature
			type: rpc
			res : stream args, int32_t progress, bool succ
		**/
		coordinat_select,

		/**!
			comment: 查询Mysql数据库， args中存放单条查询信息， 如果返回的是数组 progress 则代表当前进度 -1 代表eof
			args: moduleid, sql
			type: rpc
			res : stream args, int32_t progress, bool succ
		*/
		mysql_query,

		mysql_update,

		rpc_end,
	};

	enum network
	{
		/*!
			comment: 创建一个接收器
			args: int32_t module_id, string ip, int32_t port
			type: dispatch
			res : bool succ or fail, string reason
		**/
		make_acceptor = 2001,

		/*!
			comment: 创建一个连接器
			args: module_id, ip, port
			type: dispatch
			res : bool succ or fail, string reason
		**/
		make_connector,

		/*!
			comment: 踢掉某个现有的连接
			args: int32_t fd
			type: dispatch
			res : bool succ or fail, string reason
		**/
		kick_connect,

		/*!
			comment: 发送一条网络消息
			args: int32_t fd, int32_t msgid, stream block
			type: dispatch
			res : nil
		**/
		send,

		/*!
			comment: 接收到一条网络消息
			args: int32_t fd, int32_t msgid, stream block
			type: listen
			res : nil
		**/
		recv,
		
		/*!
			comment: 接收到一个新的连接（适用于 acceptor
			args: int32_t fd
			type: listen
			res : nil
		**/
		new_connect,

		/*!
			comment: 连接被断开
			args: int32_t fd
			type: listen
			res : int32_t fd, string reason
		**/
		dis_connect,
	};

	enum log
	{
		/*!
			comment: 输出日志
			args: int32_t loglv, string log
			type: dispatch
			res : nil 
		**/
		print = 2101,

		/*!
			comment: 关闭/打开调试日志
			args: int32_t flag (1 close, 0 open)
			type: dispatch
			res : nil
		*/
		nodebug,
	};

	enum timer
	{
		/*!
			comment: 延迟若干毫秒触发
			args: int32_t module_id, int32_t milliseconds
			type: dispatch
			res : int64_t timer_id
		**/
		delay_milliseconds = 2201,

		/*!
			comment: 隔天触发
			args: int32_t module_id, int32_t hour, int32_t minute
			type: dispatch
			res : int64_t timer_id
		**/
		delay_day,

		delay_week,

		delay_month,

		/*!
			comment: 从定时器中移除某个timer
			args: int64_t timer_id
			type: dispatch
			res : bool succ
		**/
		remove_timer,

		/*!
			comment: 触发Timer
			args: nil
			type: listen
			res : int64_t timer_id
		**/
		timer_arrive,
	};

	enum lua_proxy
	{
		/*!
			comment: 创建 Lua Script Module ,proxy会自动完成c++/lua的相关绑定 
			args: int32_t module_id, string script_dir, string script_name
			type: dispatch
			res : bool succ
		**/
		create = 2301,

		/*!
			comment: 重新装载 Lua Script Module, 会走标准的退出和进入流程 init, shut 。即便持有状态也可以方便的热更
			args: int32_t module_id
			type: dispatch
			res : bool succ
		**/
		reload,

		/*!
			comment: 移除 Lua Script Module
			args: int32_t module_id
			type: dispatch
			res : bool succ
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
			comment: 建立一个新的Mysql连接
			args: string host, string user, string password, string dbName, int32_t port
			type: dispatch
			res : bool succ
		**/
		mysql_connect,

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
