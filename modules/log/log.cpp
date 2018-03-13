#include "log.h"

#include <string>
#include <sstream>
#include <iostream>

#define GOOGLE_GLOG_DLL_DECL
#include <glog/logging.h>

#include <core/application.h>

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

	listen(this, eid::log::print, std::bind(&LogModule::ePrint, this, _1, _2));

	listen(this, eid::log::nodebug, std::bind(&LogModule::eChangeFlag, this, _1, _2));

	auto exeName = APP.getAppName();
	initImpl(exeName);
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

void gsf::modules::LogModule::initImpl(const std::string &exe_name)
{
	std::string _path = std::string(path_) + "/log";

	FLAGS_log_dir			= _path;	//设置输出路径
	FLAGS_alsologtostderr	= true;		//设置日志消息除了日志文件之外是否去标准输出
	FLAGS_colorlogtostderr	= true;		//设置记录到标准输出的颜色消息（如果终端支持）
	FLAGS_max_log_size		= 10;		//设置最大日志文件大小（以MB为单位）
	FLAGS_logbufsecs		= 0;		//立即写入到日志
	//FLAGS_stderrthreshold = google::GLOG_INFO;

	google::SetLogDestination(google::GLOG_INFO, (_path + "/" + exe_name + ".info_").c_str());
	google::SetLogDestination(google::GLOG_WARNING, (_path + "/" + exe_name + ".warning_").c_str());
	google::SetLogDestination(google::GLOG_ERROR, (_path + "/" + exe_name + ".error_").c_str());

	google::InitGoogleLogging(exe_name.c_str());
}


void gsf::modules::LogModule::ePrint(gsf::ArgsPtr args, gsf::CallbackFunc callback /* = nullptr */)
{
	//std::cout << args->to_string() << std::endl;
	auto _lv = args->pop_ui16();
	auto _context = args->pop_string();

	switch (_lv)
	{
	case gsf::LogDebug:
		if (!ndebug_) {
			DLOG(INFO) << _context;
		}
		break;
	case gsf::LogInfo:
		LOG(INFO) << _context;
		break;
	case gsf::LogWarning:
		LOG(WARNING) << _context;
		break;
	case gsf::LogErr:
		LOG(ERROR) << _context;
		break;
	}
}

void gsf::modules::LogModule::eChangeFlag(gsf::ArgsPtr args, gsf::CallbackFunc callback /* =nullptr */)
{
	auto _flag = args->pop_i32();
	if (0 == _flag) {
		ndebug_ = false;
	}
	else if (1 == _flag) {
		ndebug_ = true;
	}
}

