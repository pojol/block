//
// Created by pojol on 2017/2/13.
//

#pragma once

// 鍚庨潰鐢ㄥ伐鍏疯嚜鍔ㄧ敓鎴?
//! 鐢ㄤ簬鎻忚堪妗嗘灦涓敤鍒扮殑浜嬩欢, 鐢ㄦ埛鍙互浠ユ涓烘ā鏉垮湪鑷繁宸ョ▼涓缓绔嬩竴涓猠vent_list 鐢ㄤ簬宸ョ▼涓殑浜嬩欢鎻忚堪

namespace block
{
	enum event {
		/*!
		comment: Module鍒濆鍖栨垚鍔?
		args: nil
		type: boardcast
		res : int32_t module_id
		**/
		module_init_succ = 101,

		/*!
		comment: Module閫€鍑烘垚鍔?
		args: nil
		type: boardcast
		res : int32_t module_id
		**/
		module_shut_succ,

		//! 鍑℃槸gsf闆嗙兢鍐呬娇鐢╮pc浜や簰鐨勬秷鎭兘搴旇瀹氫箟鍦╠istributed鍖哄潡
		rpc_begin = 1001,

		/*!
		comment: 灏哊ode缁戝畾鍒癈oordinator
		args: string node_type, int32_t node_id, string root_ip, int32_t root_port, [{string module_name, int32_t module_id, int32_t module_fature} ... ]
		type: rpc
		res : stream args, int32_t progress, bool succ
		**/
		coordinat_regist,

		/***/
		coordinat_unregit,

		/*!
		comment: 璋冩暣Node鍦–oordinator涓殑鏉冮噸
		args: int32_t node_id, string module_name, int32_t module_fature, int32_t +- weight
		type: rpc
		res : stream args, int32_t progress, bool succ
		**/
		coordinat_adjust_weight,

		/*!
		comment: 閫氳繃ModuleName鍜孧oduleFeature閫夊彇涓€涓泦缇や腑閫傚悎鑷繁鐨凬ode
		args: string module_name, int32_t module_fature
		type: rpc
		res : stream args, int32_t progress, bool succ
		**/
		coordinat_select,

		rpc_end = 1999,

		/*!
		comment: 鍒涘缓涓€涓帴鏀跺櫒
		args: int32_t module_id, string ip, int32_t port
		type: dispatch
		res : bool succ or fail, string reason
		**/
		tcp_make_acceptor = 2001,

		/*!
		comment: 鍒涘缓涓€涓繛鎺ュ櫒
		args: module_id, ip, port
		type: dispatch
		res : bool succ or fail, string reason
		**/
		tcp_make_connector,

		/*!
		comment: 韪㈡帀鏌愪釜鐜版湁鐨勮繛鎺?
		args: int32_t fd
		type: dispatch
		res : bool succ or fail, string reason
		**/
		tcp_kick_connect,
			
		/*!
		comment: 鍙戦€佷竴鏉＄綉缁滄秷鎭?
		args: int32_t fd, int32_t msgid, stream block
		type: dispatch
		res : nil
		**/
		tcp_send,

		/*!
		comment: 鎺ユ敹鍒颁竴鏉＄綉缁滄秷鎭?
		args: int32_t fd, int32_t msgid, stream block
		type: listen
		res : nil
		**/
		tcp_recv,

		/*!
		comment: 鎺ユ敹鍒颁竴涓柊鐨勮繛鎺ワ紙閫傜敤浜?acceptor
		args: int32_t fd
		type: listen
		res : nil
		**/
		tcp_new_connect,

		/*!
		comment: 杩炴帴琚柇寮€, 鍙湁杩炴帴閿欒鎵嶄細鏀跺埌杩欐潯浜嬩欢锛堟湇鍔″櫒涓诲姩kick涓嶄細
		args: int32_t fd
		type: listen
		res : int32_t fd, string reason
		**/
		tcp_dis_connect,

		/*!
		comment: 閲嶆柊瑁呰浇 Lua Script Module, 浼氳蛋鏍囧噯鐨勯€€鍑哄拰杩涘叆娴佺▼ init, shut 銆傚嵆渚挎寔鏈夌姸鎬佷篃鍙互鏂逛究鐨勭儹鏇?
		args: int32_t module_id
		type: dispatch
		res : bool succ
		**/
		script_reload = 2301,

		/*!
		comment: 寤虹珛涓€涓柊鐨凪ysql杩炴帴
		args: string host, string user, string password, string dbName, int32_t port, bool useCache
		type: dispatch
		res : bool succ
		**/
		db_connect = 2401,

		/*!
		comment: 鎵ц涓€鏉ql璇彞
		args: int32_t moduleID, string sql
		type: dispatch
		res : nil
		**/
		db_execSql,

		/*!
		comment: 鍒涘缓涓€涓疄渚?
		args: string tableName, table entityInfo
		type: dispatch
		res : entityInfo
		**/
		db_insert,

		/*!
		comment: 鑾峰彇涓€涓疄渚?
		args: int32_t moduleID, string field, int32_t entityID ( entityID == 0 then load all
		type: dispatch
		res : entityInfo
		**/
		db_load,

		/*!
		comment: 鏇存柊涓€涓疄渚?
		args: string field, int32_t entityID, table entityDirty
		type: dispatch
		res : nil
		**/
		db_update,

		db_callback,

		node_create = 2501,
		node_regist
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
}
