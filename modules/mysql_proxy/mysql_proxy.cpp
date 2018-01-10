
#include "mysql_proxy.h"

gsf::modules::MysqlProxyModule::MysqlProxyModule()
	: Module("MysqlProxyModule")
{

}

gsf::modules::MysqlProxyModule::~MysqlProxyModule()
{

}


void gsf::modules::MysqlProxyModule::before_init()
{
	log_m_ = dispatch(eid::app_id, eid::get_module, gsf::make_args("LogModule"))->pop_moduleid();
}

void gsf::modules::MysqlProxyModule::init()
{
	listen(this
		, eid::db_proxy::mysql_connect
		, std::bind(&MysqlProxyModule::init_event, this, std::placeholders::_1));

	listen(this
		, eid::db_proxy::mysql_execute
		, std::bind(&MysqlProxyModule::execute_event, this, std::placeholders::_1));
	
	listen(this
		, eid::db_proxy::mysql_update
		, std::bind(&MysqlProxyModule::update_event, this, std::placeholders::_1));
}

void gsf::modules::MysqlProxyModule::execute()
{

}

void gsf::modules::MysqlProxyModule::shut()
{

}

void gsf::modules::MysqlProxyModule::after_shut()
{

}

gsf::ArgsPtr gsf::modules::MysqlProxyModule::init_event(const gsf::ArgsPtr &args)
{
	auto _host = args->pop_string();	//host
	auto _user = args->pop_string(); //user
	auto _password = args->pop_string(); //password
	auto _dbName = args->pop_string(); //database
	auto _port = args->pop_i32();	//port

	if (conn_.init(_host, _port, _user, _password, _dbName)) {
		return gsf::make_args(true);
	}

	return gsf::make_args(false);
}

gsf::ArgsPtr gsf::modules::MysqlProxyModule::execute_event(const gsf::ArgsPtr &args)
{
	auto _moduleid = args->pop_moduleid();
	auto _remote = args->pop_moduleid();
	std::string queryStr = args->pop_string();

	using namespace std::placeholders;
	conn_.execute(_moduleid, _remote, queryStr, std::bind(&MysqlProxyModule::event_callback, this, _1, _2));

	return nullptr;
}

gsf::ArgsPtr gsf::modules::MysqlProxyModule::update_event(const gsf::ArgsPtr &args)
{
	auto _order = args->pop_string();

	conn_.update(_order, args);

	return nullptr;
}

void gsf::modules::MysqlProxyModule::event_callback(gsf::ModuleID target, const gsf::ArgsPtr &args)
{
	dispatch(target, eid::db_proxy::mysql_callback, args);
}

