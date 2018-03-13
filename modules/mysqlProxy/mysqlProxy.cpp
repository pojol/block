
#include "mysqlProxy.h"

#include <core/application.h>

gsf::modules::MysqlProxyModule::MysqlProxyModule()
	: Module("MysqlProxyModule")
{

}

gsf::modules::MysqlProxyModule::~MysqlProxyModule()
{

}


void gsf::modules::MysqlProxyModule::before_init()
{
	logM_ = APP.getModule("LogModule");
}

void gsf::modules::MysqlProxyModule::init()
{
	listen(this
		, eid::db_proxy::mysql_connect
		, std::bind(&MysqlProxyModule::eInit, this, std::placeholders::_1, std::placeholders::_2));

	listen(this
		, eid::distributed::mysql_query
		, std::bind(&MysqlProxyModule::eQuery, this, std::placeholders::_1, std::placeholders::_2));

	listen(this, eid::distributed::mysql_update
		, std::bind(&MysqlProxyModule::eUpdate, this, std::placeholders::_1, std::placeholders::_2));
}

void gsf::modules::MysqlProxyModule::execute()
{
	while (!queue_.empty()) {

		auto _callbackPtr = queue_.front();

		dispatch(_callbackPtr->target_, eid::db_proxy::mysql_callback, std::move(_callbackPtr->ptr_));

		delete _callbackPtr;
		_callbackPtr = nullptr;
	}
}

void gsf::modules::MysqlProxyModule::shut()
{

}

void gsf::modules::MysqlProxyModule::after_shut()
{

}

void gsf::modules::MysqlProxyModule::eInit(gsf::ArgsPtr args, gsf::CallbackFunc callback /* = nullptr */)
{
	auto _host = args->pop_string();	//host
	auto _user = args->pop_string(); //user
	auto _password = args->pop_string(); //password
	auto _dbName = args->pop_string(); //database
	auto _port = args->pop_i32();	//port

	if (!conn_.init(_host, _port, _user, _password, _dbName)) {
		APP.ERR_LOG("MysqlProxy", "init fail!");
	}
}

void gsf::modules::MysqlProxyModule::eQuery(gsf::ArgsPtr args, gsf::CallbackFunc callback /* = nullptr */)
{
	auto _moduleid = args->pop_moduleid();
	auto _remote = args->pop_moduleid();
	std::string queryStr = args->pop_string();

	using namespace std::placeholders;
	conn_.query(_moduleid, _remote, queryStr, [&](gsf::ModuleID target, gsf::ArgsPtr args) {
		auto _callbackPtr = new CallbackInfo();
		_callbackPtr->ptr_ = std::move(args);
		_callbackPtr->target_ = target;
		queue_.push(_callbackPtr);
	});
}

void gsf::modules::MysqlProxyModule::eUpdate(gsf::ArgsPtr args, gsf::CallbackFunc callback /* = nullptr */)
{
	auto _table = args->pop_string();
	auto _key = args->pop_i32();

	auto _getkey = [&]()->std::string {
		return args->pop_string();
	};

	auto _getval = [&](int32_t tag)->std::string {
		switch (tag) {
		case gsf::at_int8: return std::to_string(args->pop_i8());
		case gsf::at_uint8: return std::to_string(args->pop_ui8());
		case gsf::at_int16: return std::to_string(args->pop_i16());
		case gsf::at_uint16: return std::to_string(args->pop_ui16());
		case gsf::at_int32: return std::to_string(args->pop_i32());
		case gsf::at_uint32: return std::to_string(args->pop_ui32());
		case gsf::at_int64: return std::to_string(args->pop_i64());
		case gsf::at_uint64: return std::to_string(args->pop_ui64());
		}

		return "0";
	};
	// update tbl_Account set sm_accountName="fdsafsad" where id=123;
	std::string _sql = "update " + _table + " set ";
	std::string _context = "";

	auto _tag = args->get_tag();
	while (_tag != 0)
	{
		auto _key = _getkey();
		auto _val = _getval(args->get_tag());

		_context += _key + "=" + _val + ',';

		_tag = args->get_tag();
	}

	if (_context.at(_context.size() - 1) == ',')
		_context.erase(_context.size() - 1);

	_sql += _context;

	_sql += " where id=" + std::to_string(_key);
	conn_.query(0, 0, _sql, nullptr);
}

