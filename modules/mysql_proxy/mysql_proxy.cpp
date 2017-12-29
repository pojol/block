
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
		, eid::db_proxy::mysql_query
		, std::bind(&MysqlProxyModule::query_event, this, std::placeholders::_1));
	
	listen(this
		, eid::db_proxy::mysql_execute
		, std::bind(&MysqlProxyModule::execute_event, this, std::placeholders::_1));
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

gsf::ArgsPtr gsf::modules::MysqlProxyModule::query_event(const gsf::ArgsPtr &args)
{
	std::string queryStr = args->pop_string();

	if (conn_.query(queryStr)) {
		return gsf::make_args(true);
	}

	return gsf::make_args(false);
}

gsf::ArgsPtr gsf::modules::MysqlProxyModule::execute_event(const gsf::ArgsPtr &args)
{
	auto _order = args->pop_string();

	if (conn_.execute(_order, args)) {
		return gsf::make_args(true);
	}
	else {
		return gsf::make_args(false);
	}
}

