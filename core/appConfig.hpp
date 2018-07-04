#pragma once

#include <stdint.h>
#include <string>

namespace block
{
	struct AppConfig
	{
		//! 每帧间隔的毫秒时间
		uint32_t tick_count = 20;

		//! args池的初始化大小
		uint32_t pool_args_count = 1024 * 10;

		//! 是否开启性能监控日志
		bool is_watch_pref = false;

		//! app的名称
		std::string name = "default";

		//! app的id，作用于分布式架构，必须保证唯一。
		int32_t appid = 0;

		//! rpc 调用的超时时间 ms
		int32_t rpc_timeout = 10000;

		//! rpc 管道的最大消息长度（大于此长度时消息会被丢弃
		int32_t rpc_maxlen = 1024;

		//! rpc channel ip (redis ip 仅作用于rpc服务 | 如果和db cache使用同一个redis 需要注意避免重复
		std::string rpcIp = "";

		//! rpc channel port (redis port
		int32_t rpcPort = 0;
	};
}