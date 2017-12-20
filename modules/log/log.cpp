#include "log.h"

#include <string>
#include <sstream>
#include <iostream>

#define GOOGLE_GLOG_DLL_DECL
#include <glog/logging.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // WIN32

gsf::modules::LogModule::LogModule()
	: Module("LogModule")
{

}


void gsf::modules::LogModule::before_init()
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
	if (cnt < 0 || cnt >= 512) {
		std::cout << "read path err" << std::endl;
		return;
	}
	for (int i = cnt; i >= 0; --i)
	{
		if (path_[i] == '/') {
			path_[i + 1] = '\0';
			break;
		}
	}
#endif // WIN32

	using namespace std::placeholders;

	listen(this, eid::log::print, std::bind(&LogModule::log_print, this, _1));

	auto exeName = dispatch(eid::app_id, eid::base::get_app_name, nullptr)->pop_string();
	init_impl(exeName);
}

void gsf::modules::LogModule::init()
{

}

void gsf::modules::LogModule::execute()
{

}

void gsf::modules::LogModule::shut()
{
	google::ShutdownGoogleLogging();
}

void gsf::modules::LogModule::init_impl(const std::string &exe_name)
{
	std::string _path = std::string(path_) + "/log";

	FLAGS_log_dir			= _path;	//设置输出路径
	FLAGS_alsologtostderr	= true;		//设置日志消息除了日志文件之外是否去标准输出
	FLAGS_colorlogtostderr	= true;		//设置记录到标准输出的颜色消息（如果终端支持）
	FLAGS_max_log_size		= 10;		//设置最大日志文件大小（以MB为单位）
	FLAGS_logbufsecs		= 0;		//立即写入到日志

	google::SetLogDestination(google::GLOG_INFO, (_path + "/" + exe_name + ".info_").c_str());
	google::SetLogDestination(google::GLOG_WARNING, (_path + "/" + exe_name + ".warning_").c_str());
	google::SetLogDestination(google::GLOG_ERROR, (_path + "/" + exe_name + ".error_").c_str());

	google::InitGoogleLogging(exe_name.c_str());
}


gsf::ArgsPtr gsf::modules::LogModule::log_print(const gsf::ArgsPtr &args)
{
	auto _lv = args->pop_ui16();
	auto _title = args->pop_string();

	auto _context = args->pop_string();

	switch (_lv)
	{
	case gsf::LogInfo:
		LOG(INFO) << "[INFO] " << _title << " " << _context;
		break;
	case gsf::LogWarning:
		LOG(WARNING) << "[WARNING] " << _title << " " << _context;
		break;
	case gsf::LogErr:
		LOG(ERROR) << "[ERROR] " << _title << " " << _context;
		break;
	}

	return nullptr;
}

