#pragma once

#include "../core/types.h"

#define GOOGLE_GLOG_DLL_DECL
#include <glog/logging.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // WIN32

#include <fmt/format.h>
#include <string>
#include <sstream>
#include <iostream>
#include <list>

namespace block
{
	namespace utils
	{
		class LoggerImpl
		{
		public:
			LoggerImpl() {}
			~LoggerImpl() {}

			void init(const std::string &appName)
			{
#ifdef WIN32
				GetModuleFileName(NULL, path_, 512);
				//取出文件路径
				for (int i = strlen(path_); i >= 0; i--)
				{
					if (path_[i] == '\\')
					{
						path_[i] = '\0';
						break;
					}
				}
#else
				int cnt = readlink("/proc/self/exe", path_, 512);
				if (cnt < 0 || cnt >= 512)
				{
					std::cout << "read path err" << std::endl;
					return;
				}
				for (int i = cnt; i >= 0; --i)
				{
					if (path_[i] == '/')
					{
						path_[i + 1] = '\0';
						break;
					}
				}
#endif // WIN32

				auto _path = std::string(path_) + "/log";

				FLAGS_log_dir = _path;		   //设置输出路径
				FLAGS_alsologtostderr = true;  //设置日志消息除了日志文件之外是否去标准输出
				FLAGS_colorlogtostderr = true; //设置记录到标准输出的颜色消息（如果终端支持）
				FLAGS_max_log_size = 10;	   //设置最大日志文件大小（以MB为单位）
				FLAGS_logbufsecs = 0;		   //立即写入到日志

				google::SetLogDestination(google::GLOG_INFO, (_path + "/" + appName + ".info_").c_str());
				//! 将日志输出到同一个文件
				//google::SetLogDestination(google::GLOG_WARNING, "");
				//google::SetLogDestination(google::GLOG_ERROR, "");
				
				//! 区分info, warning, error 日志文件
				google::SetLogDestination(google::GLOG_WARNING, (_path + "/" + appName + ".warning_").c_str());
				google::SetLogDestination(google::GLOG_ERROR, (_path + "/" + appName + ".error_").c_str());

				google::InitGoogleLogging(appName.c_str());
			}

			void print(int lv, const std::string &content)
			{
				switch (lv)
				{
				case block::LogDebug:
					DLOG(INFO) << content;
					break;
				case block::LogInfo:
					LOG(INFO) << content;
					break;
				case block::LogWarning:
					LOG(WARNING) << content;
					break;
				case block::LogErr:
					LOG(ERROR) << content;
					break;
				}
			}

			// -- 调试日志 --
			template <typename ...P>
			void DEBUG_LOG(const std::string &content, P&& ...values);
			void DEBUG_LOG(const std::string &content)
			{
				print(block::LogDebug, content);
			}

			// -- 诊断日志 --
			template <typename... P>
			void INFO_LOG(const std::string &content, P &&... values);
			void INFO_LOG(const std::string &content)
			{
				print(block::LogInfo, content);
			}

			template <typename... P>
			void ERR_LOG(const std::string &content, P &&... values);
			void ERR_LOG(const std::string &content)
			{
				print(block::LogErr, content);
			}

			template <typename... P>
			void WARN_LOG(const std::string &content, P &&... values);
			void WARN_LOG(const std::string &content)
			{
				print(block::LogWarning, content);
			}

			// -- 统计日志 --
			// behavior, player, time, args
			template <typename ...P>
			void RECORD_LOG(const std::string &behavior, uint32_t player, uint32_t time, const std::string &content, P&& ...values);

		private:
			char path_[512];
		};

		template <typename ...P>
		void block::utils::LoggerImpl::RECORD_LOG(const std::string &behavior, uint32_t player, uint32_t time, const std::string &content, P&& ...values)
		{
		}

		template <typename ...P>
		void block::utils::LoggerImpl::WARN_LOG(const std::string &content, P&& ...values)
		{
			print(block::LogWarning, fmt::format(content, std::forward<P>(values)...));
		}


		template <typename ...P>
		void block::utils::LoggerImpl::ERR_LOG(const std::string &content, P&& ...values)
		{
			print(block::LogErr, fmt::format(content, std::forward<P>(values)...));
		}


		template <typename ...P>
		void block::utils::LoggerImpl::INFO_LOG(const std::string &content, P&& ...values)
		{
			print(block::LogInfo, fmt::format(content, std::forward<P>(values)...));
		}

		template <typename ...P>
		void block::utils::LoggerImpl::DEBUG_LOG(const std::string &content, P&& ...values)
		{
			print(block::LogDebug, fmt::format(content, std::forward<P>(values)...));
		}
	}

}

