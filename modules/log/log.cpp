#include "log.h"

#include <string>

#define GOOGLE_GLOG_DLL_DECL
#include <glog/logging.h>


void gsf::modules::LogModule::init()
{
	listen(this, event_id::log::init
		, std::bind(&LogModule::init_impl, this
		, std::placeholders::_1
		, std::placeholders::_2));

	listen(this, event_id::log::info
		, std::bind(&LogModule::log_info, this
		, std::placeholders::_1
		, std::placeholders::_2));

	listen(this, event_id::log::warning
		, std::bind(&LogModule::log_warning, this
		, std::placeholders::_1
		, std::placeholders::_2));

	listen(this, event_id::log::error
		, std::bind(&LogModule::log_error, this
		, std::placeholders::_1
		, std::placeholders::_2));
}

void gsf::modules::LogModule::execute()
{
	while (!log_.empty())
	{
		auto itr = log_.begin();

		switch (itr->first)
		{
		case event_id::log::info:
			LOG(INFO) << itr->second;
			break;
		}
	
		log_.pop_front();
	}
}

void gsf::modules::LogModule::shut()
{
	google::ShutdownGoogleLogging();
}

void gsf::modules::LogModule::init_impl(gsf::Args args, gsf::EventHandlerPtr callback)
{
	std::string _appid = args.pop_string(0);
	std::string _path = args.pop_string(1);

	FLAGS_log_dir			= _path;			//设置输出路径
	FLAGS_alsologtostderr	= true;  //设置日志消息除了日志文件之外是否去标准输出
	FLAGS_colorlogtostderr	= true;  //设置记录到标准输出的颜色消息（如果终端支持）
	FLAGS_max_log_size		= 10;  //设置最大日志文件大小（以MB为单位）

	google::InitGoogleLogging("log");
}

void gsf::modules::LogModule::log_info(gsf::Args args, gsf::EventHandlerPtr callback)
{
	log_.push_back(std::make_pair(event_id::log::info, args.pop_string(0)));
}

void gsf::modules::LogModule::log_warning(gsf::Args args, gsf::EventHandlerPtr callback)
{

}

void gsf::modules::LogModule::log_error(gsf::Args args, gsf::EventHandlerPtr callback)
{

}

