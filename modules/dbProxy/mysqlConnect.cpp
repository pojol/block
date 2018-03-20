#include "mysqlConnect.h"

#include <iostream>
#include <core/application.h>

uint8_t ToValueType(enum_field_types mysqlType)
{
	switch (mysqlType)
	{
	case FIELD_TYPE_TIMESTAMP:
	case FIELD_TYPE_DATE:
	case FIELD_TYPE_TIME:
	case FIELD_TYPE_DATETIME:
	case FIELD_TYPE_YEAR:
	case FIELD_TYPE_STRING:
	case FIELD_TYPE_VAR_STRING:
	case FIELD_TYPE_BLOB:
	case FIELD_TYPE_SET:
	case FIELD_TYPE_NULL:
		return gsf::at_string;
	case FIELD_TYPE_TINY:
	case FIELD_TYPE_SHORT:
	case FIELD_TYPE_ENUM:
	case FIELD_TYPE_LONG:
		return gsf::at_int32;
	case FIELD_TYPE_INT24:
	case FIELD_TYPE_LONGLONG:
		return gsf::at_int64;
	case FIELD_TYPE_DECIMAL:
	case FIELD_TYPE_FLOAT:
		return gsf::at_float;
	case FIELD_TYPE_DOUBLE:
		return gsf::at_double;
	default:
		return gsf::at_eof;
	}
}

std::pair<enum_field_types, char> ToMySqlType(uint8_t cppType)
{
	std::pair<enum_field_types, char> ret(MYSQL_TYPE_NULL, 0);

	switch (cppType)
	{
	case gsf::at_uint8:
		ret.first = MYSQL_TYPE_TINY;
		ret.second = 1;
		break;
	case gsf::at_int8:
		ret.first = MYSQL_TYPE_TINY;
		ret.second = 0;
		break;
	case gsf::at_uint16:
		ret.first = MYSQL_TYPE_SHORT;
		ret.second = 1;
		break;
	case gsf::at_int16:
		ret.first = MYSQL_TYPE_SHORT;
		ret.second = 0;
		break;
	case gsf::at_uint32:
		ret.first = MYSQL_TYPE_LONG;
		ret.second = 1;
		break;
	case gsf::at_int32:
		ret.first = MYSQL_TYPE_LONG;
		ret.second = 0;
		break;
	case gsf::at_uint64:
		ret.first = MYSQL_TYPE_LONGLONG;
		ret.second = 1;
		break;
	case gsf::at_int64:
		ret.first = MYSQL_TYPE_LONGLONG;
		ret.second = 0;
		break;
	case gsf::at_float:
		ret.first = MYSQL_TYPE_FLOAT;
		ret.second = 0;
		break;
	case gsf::at_double:
		ret.first = MYSQL_TYPE_DOUBLE;
		ret.second = 0;
		break;
	case gsf::at_string:
		ret.first = MYSQL_TYPE_STRING;
		ret.second = 0;
		break;
	}
	return ret;
}

gsf::modules::MysqlConnect::MysqlConnect()
{

}

gsf::modules::MysqlConnect::~MysqlConnect()
{

}

bool gsf::modules::MysqlConnect::init(const std::string &host, int port, const std::string &user, const std::string &pwd, const std::string &name)
{
	auto init = mysql_init(nullptr);
	if (nullptr == init) {
		APP.ERR_LOG("MysqlConnect", "init fail!");
		return false;
	}
	
	basePtr_ = mysql_real_connect(init, host.c_str(), user.c_str(), pwd.c_str(), name.c_str(), port, nullptr, 0);
	if (nullptr == basePtr_) {
		APP.ERR_LOG("MysqlConnect", "connect fail!");
		//mysql_close(base);
		return false;
	}

	return true;
}

void gsf::modules::MysqlConnect::execute(const std::string &order, const gsf::ArgsPtr &args)
{
	SqlStmtPtr stmt;
	perpare(order, stmt);

	if (stmt->params != args->get_params_count())
	{
		std::cout << " enough params " << std::endl;
	}

	std::vector<MYSQL_BIND> mysqlBinds;
	auto _tag = args->get_tag();
	while (0 != _tag)
	{
		auto mt = ToMySqlType(_tag);
		mysqlBinds.push_back(MYSQL_BIND());
		auto& Param = mysqlBinds.back();
		memset(&Param, 0, sizeof(MYSQL_BIND));
		Param.buffer_type = mt.first;
		Param.is_null = 0;
		Param.is_unsigned = mt.second;
		Param.length = 0;

		if (_tag == gsf::at_int8 || _tag == gsf::at_uint8 || _tag == gsf::at_int16 || _tag == gsf::at_uint16 ||
			_tag == gsf::at_int16 || _tag == gsf::at_int32 || _tag == gsf::at_uint32 || _tag == gsf::at_int64 ||
			_tag == gsf::at_uint64 || _tag == gsf::at_float || _tag == gsf::at_double || _tag == gsf::at_bool)
		{
			Param.buffer = args->peek(_tag);
			Param.buffer_length = (unsigned long)0;
		}
		else if (_tag == gsf::at_string) {
			auto _pair = args->peek_str();
			Param.buffer = _pair.first;
			Param.buffer_length = _pair.second;
		}
		else {

		}

		_tag = args->get_tag();
	}

	// bind input arguments
	if (mysql_stmt_bind_param(stmt->stmt, mysqlBinds.data()))
	{
		std::cout << mysql_stmt_error(stmt->stmt) << std::endl;
	}

	if (mysql_stmt_execute(stmt->stmt))
	{
		std::cout << mysql_stmt_error(stmt->stmt) << std::endl;
	}
}

void gsf::modules::MysqlConnect::query(gsf::ModuleID target, const std::string &sql, std::function<void (gsf::ModuleID, gsf::ArgsPtr)> callback)
{
	MYSQL_RES *result = nullptr;
	MYSQL_FIELD *fields = nullptr;

	int _progress = 1;

	auto errf = [&](const std::string &err) {
		if (callback) {
			callback(target, gsf::makeArgs(false, int32_t(-1), err));
		}
	};

	if (mysql_query(basePtr_, sql.c_str())) {
		errf(mysql_error(basePtr_));
		return;
	}

	result = mysql_store_result(basePtr_);
	if (nullptr == result) {
		std::string sqlop = sql.substr(0, 6);
		if (strcmp(sqlop.c_str(), "INSERT") == 0 || strcmp(sqlop.c_str(), "insert") == 0) {
			
			if (mysql_query(basePtr_, "select last_insert_id()")) {
				errf(mysql_error(basePtr_));
				return;
			}

			result = mysql_store_result(basePtr_);
		}
		else {
			if (callback) {
				callback(target, gsf::makeArgs(true, int32_t(-1), "success!"));
			}
			return;
		}
	}

	uint64_t rowCount = mysql_affected_rows(basePtr_);
	if (0 == rowCount) {
		mysql_free_result(result);
	}

	uint32_t fieldCount = mysql_field_count(basePtr_);
	fields = mysql_fetch_fields(result);

	std::vector<std::pair<std::string, uint8_t>> col_tags;

	for (uint32_t i = 0; i < fieldCount; ++i)
	{
		col_tags.push_back(std::make_pair(fields[i].name, ToValueType(fields[i].type)));
	}

	MYSQL_ROW row;
	row = mysql_fetch_row(result);
	size_t col = 0;

	while (nullptr != row)
	{
		auto argsPtr = gsf::ArgsPool::get_ref().get();
		argsPtr->push(true);
		argsPtr->push(_progress);

		for (size_t col = 0; col < fieldCount; col++)
		{
			auto _val = row[col];

			switch (col_tags[col].second)
			{
			case gsf::at_int32:
				argsPtr->push_i32(row[col] != nullptr ? std::stoul(row[col]) : 0);
				break;
			case gsf::at_int64:
				argsPtr->push_i64(row[col] != nullptr ? std::stoull(row[col]) : 0);
				break;
			case gsf::at_float:
				argsPtr->push_float(row[col] != nullptr ? std::stof(row[col]) : 0);
				break;
			case gsf::at_double:
				argsPtr->push_double(row[col] != nullptr ? std::stod(row[col]) : 0);
				break;
			case gsf::at_string:
				argsPtr->push_string((row[col] != nullptr) ? row[col] : "");
				break;
			}
		}
		callback(target, std::move(argsPtr));

		_progress++;

		row = mysql_fetch_row(result);
	}

	// eof
	callback(target, gsf::makeArgs(true, -1));

	if (nullptr != result)
	{
		mysql_free_result(result);
	}
}

void gsf::modules::MysqlConnect::perpare(const std::string &sql, SqlStmtPtr &stmtPtr)
{
	/*
	auto itr = prepared_stmt_map.find(sql);
	if (itr != prepared_stmt_map.end()) {
		stmtPtr = itr->second;
		return;
	}

	do {
		if (nullptr == base) {
			std::cout << "mysql unuseable!" << std::endl;
			break;
		}

		stmtPtr = std::make_shared<SqlStmt>();
		stmtPtr->sql = sql;

		stmtPtr->stmt = mysql_stmt_init(base);
		if (nullptr == stmtPtr->stmt) {
			std::cout << "stmt init fail" << std::endl;
			break;
		}

		if (mysql_stmt_prepare(stmtPtr->stmt, sql.c_str(), (unsigned long)sql.size())) {
			std::cout << mysql_stmt_error(stmtPtr->stmt) << std::endl;
			break;
		}

		stmtPtr->params = mysql_stmt_param_count(stmtPtr->stmt);
		stmtPtr->result = mysql_stmt_result_metadata(stmtPtr->stmt);

		std::string sqlop = sql.substr(0, 6);
		if (nullptr == stmtPtr->result && (strcmp(sqlop.c_str(), "SELECT") == 0 || strcmp(sqlop.c_str(), "select") == 0)) {
			std::cout << mysql_stmt_error(stmtPtr->stmt) << std::endl;
			break;
		}

		if (stmtPtr->result) {
			stmtPtr->is_query = true;
			stmtPtr->columns = mysql_num_fields(stmtPtr->result);
		}
		prepared_stmt_map.emplace(sql, stmtPtr);
		return;

	} while (0);

	stmtPtr.reset();
	*/
}


void gsf::modules::MysqlConnect::startThread()
{
	if (nullptr != basePtr_) {
		mysql_thread_init();
	}	
}


void gsf::modules::MysqlConnect::endThread()
{
	if (nullptr != basePtr_) {
		mysql_thread_end();
	}
}