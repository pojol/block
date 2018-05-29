#pragma once

#include "../depend/types.h"

#define GOOGLE_GLOG_DLL_DECL
#include <glog/logging.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // WIN32

#include <string>
#include <sstream>
#include <iostream>
#include <list>

#include "logger_impl.hpp"

namespace block
{
	namespace utils
	{
		class Logger
		{
		public:
			Logger() {}
			~Logger() {}

			void init(const std::string &appName)
			{
				impl_.init(appName);
			}

			// -- 调试日志 --
			template <typename ...P>
			void DEBUG(const std::string &content, P&& ...values)
			{
				impl_.DEBUG_LOG(content, values...);
			}
			void DEBUG(const std::string &content)
			{
				impl_.DEBUG_LOG(content);
			}

			// -- 诊断日志 --
			template <typename... P>
			void INFO(const std::string &content, P &&... values)
			{
				impl_.INFO_LOG(content, values...);
			}
			void INFO(const std::string &content)
			{
				impl_.INFO_LOG(content);
			}

			template <typename... P>
			void ERR(const std::string &content, P &&... values)
			{
				impl_.ERR_LOG(content, values...);
			}
			void ERR(const std::string &content)
			{
				impl_.ERR_LOG(content);
			}

			template <typename... P>
			void WARN(const std::string &content, P &&... values)
			{
				impl_.WARN_LOG(content, values...);
			}
			void WARN(const std::string &content)
			{
				impl_.WARN_LOG(content);
			}

			// -- 统计日志 --
			// behavior, player, time, args
			template <typename ...P>
			void RECORD(const std::string &behavior, uint32_t player, uint32_t time, const std::string &content, P&& ...values)
			{
				impl_.RECORD_LOG(behavior, player, time, content, values);
			}

		private:
			LoggerImpl impl_;
		};
	}

}

