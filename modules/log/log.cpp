#include "log.h"

#include <string>
#include <sstream>

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
	listen(this, eid::log::log_callback, [&](const gsf::ArgsPtr &args, gsf::CallbackFunc callback) {

	});

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

	dispatch(eid::app_id, eid::get_app_name, nullptr, [&](const gsf::ArgsPtr &args){
		init_impl(args->pop_string());	
	});
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


void gsf::modules::LogModule::log_print(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
{
/*
	std::ostringstream oss;

	for (int i = 0; i < args.get_count(); ++i)
	{
		uint32_t _type = args.get_typeid(i);
		switch (_type)
		{
		case 0:
			if (args.pop_bool(i)) {
				oss << "true";
			}
			else {
				oss << "false";
			}
			break;
		case 1:
			oss << args.pop_uint32(i);
			break;
		case 2:
			oss << args.pop_int32(i);
			break;
		case 3:
			oss << args.pop_uint64(i);
			break;
		case 4:
			oss << args.pop_int64(i);
			break;
		case 5:
			oss << args.pop_string(i);
			break;
		}
	}

	switch (type)
	{
	case eid::log::info:
		LOG(INFO) << "[INFO] " << title << " " << oss.str();
		break;
	case eid::log::warning:
		LOG(WARNING) << "[WARNING] " << title << " " << oss.str();
		break;
	case eid::log::error:
		LOG(ERROR) << "[ERROR] " << title << " " << oss.str();
		break;
	}
*/
}

