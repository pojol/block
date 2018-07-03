//
// Created by pojol on 2017/2/13.
//

#pragma once

// 后面用工具自动生成
//! 用于描述框架中用到的事件, 用户可以以此为模板在自己工程中建立一个event_list 用于工程中的事件描述

namespace block
{
	namespace event {

		enum block_event {
			/*!
			comment: Module初始化成功
			args: nil
			type: boardcast
			res : int32_t module_id
			**/
			module_init_succ = 101,

			/*!
			comment: Module退出成功
			args: nil
			type: boardcast
			res : int32_t module_id
			**/
			module_shut_succ,

			/*!
			comment: 创建一个接收器
			args: int32_t module_id, string ip, int32_t port
			type: dispatch
			res : bool succ or fail, string reason
			**/
			tcp_make_acceptor = 2001,

			/*!
			comment: 创建一个连接器
			args: module_id, ip, port
			type: dispatch
			res : bool succ or fail, string reason
			**/
			tcp_make_connector,

			/*!
			comment: 踢掉某个现有的连接
			args: int32_t fd
			type: dispatch
			res : bool succ or fail, string reason
			**/
			tcp_kick_connect,

			/*!
			comment: 发送一条网络消息
			args: int32_t fd, int32_t msgid, stream block
			type: dispatch
			res : nil
			**/
			tcp_send,

			/*!
			comment: 接收到一条网络消息
			args: int32_t fd, int32_t msgid, stream block
			type: listen
			res : nil
			**/
			tcp_recv,

			/*!
			comment: 接收到一个新的连接（适用于 acceptor
			args: int32_t fd
			type: listen
			res : nil
			**/
			tcp_new_connect,

			/*!
			comment: 连接被断开, 只有连接错误才会收到这条事件（服务器主动kick不会
			args: int32_t fd
			type: listen
			res : int32_t fd, string reason
			**/
			tcp_dis_connect,

			/*!
			comment: 重新装载 Lua Script Module, 会走标准的退出和进入流程 init, shut 。即便持有状态也可以方便的热更
			args: int32_t module_id
			type: dispatch
			res : bool succ
			**/
			script_reload = 2301,

			/*!
			comment: 建立一个新的Mysql连接
			args: string host, string user, string password, string dbName, int32_t port, bool useCache
			type: dispatch
			res : bool succ
			**/
			db_connect = 2401,

			/*!
			comment: 执行一条sql语句
			args: int32_t moduleID, string sql
			type: dispatch
			res : nil
			**/
			db_execSql,

			/*!
			comment: 创建一个实例
			args: string tableName, table entityInfo
			type: dispatch
			res : entityInfo
			**/
			db_insert,

			/*!
			comment: 获取一个实例
			args: int32_t moduleID, string field, int32_t entityID ( entityID == 0 then load all
			type: dispatch
			res : entityInfo
			**/
			db_load,

			/*!
			comment: 更新一个实例
			args: string field, int32_t entityID, table entityDirty
			type: dispatch
			res : nil
			**/
			db_update,

			db_callback,

			node_init = 2501,
			node_addModule,
			node_rmvModule,
		};
		
	}

	namespace error
	{
		enum block_error
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
	}	
}