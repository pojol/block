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
	};
}