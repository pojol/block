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
			comment: 通过字符串获得module的id， 只能获取静态显示声明的module。
			args: string module_name
			type: dispatch
			res : int32_t module_id 
		**/
		get_module,

		/*!
			comment: 运行过程中创建Module，需要通过 REGISTER_CLASS 提前进行类型定义。
			args: string module_type
			type: dispatch 
			res : int32_t module_id
		**/
		new_dynamic_module,
		
		/*!
			comment: 移除注册在App中的某个Module
			args: int32_t module_id
			type: dispatch
			res : nil
		**/
		delete_module,

		module_init_succ,
		module_shut_succ,

		/*!
			comment: 在集群中创建一个唯一ID，需要用户通过config.machine 为App做好区分。
			args: nil
			type: dispatch
			res : int64_t uuid
		**/
		uuid,
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
		
		mysql_execute,

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