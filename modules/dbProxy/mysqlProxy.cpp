
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
		, eid::dbProxy::connect
		, std::bind(&MysqlProxyModule::eInit, this, std::placeholders::_1, std::placeholders::_2));

	listen(this
		, eid::dbProxy::query
		, std::bind(&MysqlProxyModule::eQuery, this, std::placeholders::_1, std::placeholders::_2));

	listen(this
		, eid::dbProxy::insert
		, std::bind(&MysqlProxyModule::eInsert, this, std::placeholders::_1, std::placeholders::_2));

	listen(this, eid::dbProxy::update
		, std::bind(&MysqlProxyModule::eUpdate, this, std::placeholders::_1, std::placeholders::_2));
}

void gsf::modules::MysqlProxyModule::execute()
{
	while (!queue_.empty()) {

		auto _callbackPtr = queue_.front();

		dispatch(_callbackPtr->target_, eid::dbProxy::callback, std::move(_callbackPtr->ptr_));

		delete _callbackPtr;
		_callbackPtr = nullptr;

		queue_.pop();
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
	auto _useCache = args->pop_bool();

	if (!conn_.init(_host, _port, _user, _password, _dbName)) {
		APP.ERR_LOG("MysqlProxy", "init fail!");
		return;
	}

	useCache_ = _useCache;
	if (useCache_) {
		timerM_ = APP.getModule("TimerModule");
		if (timerM_ == gsf::ModuleNil) {
			APP.ERR_LOG("DBProxy", "unRegist TimerModule!");
			return;
		}

		redisPtr_ = std::make_shared<RedisProxy>();
		redisPtr_->init();

		listen(this, eid::timer::timer_arrive, std::bind(&MysqlProxyModule::onTimer, this, std::placeholders::_1, std::placeholders::_2));

		dispatch(timerM_, eid::timer::delay_milliseconds, gsf::makeArgs(getModuleID(), executeCommandDelay_), [&](gsf::ArgsPtr args) {
			commandTimeID_ = args->pop_moduleid();
		});

		dispatch(timerM_, eid::timer::delay_milliseconds, gsf::makeArgs(getModuleID(), executeRewriteDelay_), [&](gsf::ArgsPtr args) {
			rewriteTimeID_ = args->pop_moduleid();
		});
	}
}

void gsf::modules::MysqlProxyModule::eSelect(gsf::ArgsPtr args, gsf::CallbackFunc callback /*= nullptr*/)
{
	auto _table = args->pop_string();

}
/*
_sql = string.format("insert into Entity (id,name,hp,lv,loginTime) values (%d,%s,%d,%d,%d);"
, self.property.id
, self.property.name
, self.property.hp
, self.property.lv
, os.time())
*/

#include <iostream>

void gsf::modules::MysqlProxyModule::eInsert(gsf::ArgsPtr args, gsf::CallbackFunc callback /*= nullptr*/)
{
	std::cout << args->to_string() << std::endl;

	auto _target = args->pop_moduleid();
	auto _table = args->pop_string();

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
		case gsf::at_string: return args->pop_string();
		}

		return "0";
	};

	if (useCache_) {

	}
	else {
		std::string _keys = "";
		std::string _values = "";

		auto _tag = args->get_tag();
		while (_tag > 0)
		{
			auto _key = args->pop_string();
			auto _tag = args->get_tag();
			auto _val = _getval(_tag);

			_keys += _key + ",";
			_values += _val + ",";

			_tag = args->get_tag();
		}
		if (_keys.at(_keys.size() - 1) == ',') {
			_keys.erase(_keys.size() - 1);
		}

		if (_values.at(_values.size() - 1) == ',') {
			_values.erase(_values.size() - 1);
		}

		std::string _sql = "insert into " + _table + " (" + _keys + ")" + " values (" + _values + ");";
		conn_.query(0, _sql, nullptr);
	}
}

void gsf::modules::MysqlProxyModule::eUpdate(gsf::ArgsPtr args, gsf::CallbackFunc callback /* = nullptr */)
{
	auto _table = args->pop_string();
	auto _key = args->pop_i32();

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
		case gsf::at_string: return args->pop_string();
		}

		return "0";
	};

	//! 当Redis存在于App中时，则将update交由redis缓存处理。
	if (useCache_) {

	}
	else {
		// update tbl_Account set sm_accountName="fdsafsad" where id=123;
		std::string _sql = "update " + _table + " set ";
		std::string _context = "";

		auto _tag = args->get_tag();
		while (0 != _tag)
		{
			auto _key = args->pop_string();
			auto _tag = args->get_tag();
			auto _val = _getval(_tag);

			_context += _key + "=" + _val + ',';

			_tag = args->get_tag();
		}

		if (_context.at(_context.size() - 1) == ',')
			_context.erase(_context.size() - 1);

		_sql += _context;

		_sql += " where id=" + std::to_string(_key);
		conn_.query(0, _sql, nullptr);
	}
}


void gsf::modules::MysqlProxyModule::eQuery(gsf::ArgsPtr args, gsf::CallbackFunc callback /* = nullptr */)
{
	auto _moduleid = args->pop_moduleid();
	std::string queryStr = args->pop_string();

	using namespace std::placeholders;

	conn_.query(_moduleid, queryStr, [&](gsf::ModuleID target, gsf::ArgsPtr args) {
		auto _callbackPtr = new CallbackInfo();
		_callbackPtr->ptr_ = std::move(args);
		_callbackPtr->target_ = target;
		queue_.push(_callbackPtr);
	});
}

void gsf::modules::MysqlProxyModule::onTimer(gsf::ArgsPtr args, gsf::CallbackFunc callback /*= nullptr*/)
{
	gsf::TimerID _tid = args->pop_ui64();

	if (_tid == commandTimeID_) {

		redisPtr_->execCommand();

		dispatch(timerM_, eid::timer::delay_milliseconds, gsf::makeArgs(getModuleID(), executeCommandDelay_), [&](gsf::ArgsPtr args) {
			commandTimeID_ = args->pop_moduleid();
		});
	}
	else if (_tid == rewriteTimeID_) {

		redisPtr_->execRewrite();

		dispatch(timerM_, eid::timer::delay_milliseconds, gsf::makeArgs(getModuleID(), executeRewriteDelay_), [&](gsf::ArgsPtr args) {
			rewriteTimeID_ = args->pop_moduleid();
		});
	}

}