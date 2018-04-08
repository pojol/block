#include "mysqlConnect.h"

#include <iostream>


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
		return gsf::at_block;
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

	APP.INFO_LOG("MysqlConnect", "init success!");

	return true;
}


void gsf::modules::MysqlConnect::insert(const std::string &query, const char *buf, unsigned long len)
{
	SqlStmtPtr stmt;
	perpare(query, stmt);

	MYSQL_BIND params[2];
	memset(params, 0, sizeof(params));

	int id = 0;

	params[0].buffer_type = MYSQL_TYPE_LONG;
	params[0].buffer = &id;

	char *blobBuf = nullptr;
	try {
		blobBuf = new char(len);
	}
	catch (...) {
		mysql_stmt_close(stmt->stmt);
		APP.ERR_LOG("MysqlConnect", "out of memory");
		return;
	}
	std::shared_ptr<char>(blobBuf, [](char *p)->void { delete[] p; });

	params[1].buffer_type = MYSQL_TYPE_BLOB;
	params[1].buffer_length = 10240;
	params[1].length = &len;
	params[1].is_null = 0;
	params[1].buffer = blobBuf;
	memcpy(blobBuf, buf, len);

	// bind input arguments
	if (mysql_stmt_bind_param(stmt->stmt, params))
	{
		std::cout << mysql_stmt_error(stmt->stmt) << std::endl;
	}

	if (mysql_stmt_execute(stmt->stmt))
	{
		std::cout << mysql_stmt_error(stmt->stmt) << std::endl;
	}
}

void gsf::modules::MysqlConnect::update(const std::string &query, const char *buf, unsigned long len)
{
	SqlStmtPtr stmt;
	perpare(query, stmt);

	MYSQL_BIND params[1];
	memset(params, 0, sizeof(params));

	char *blobBuf = nullptr;
	try {
		blobBuf = new char(len);
	}
	catch (...) {
		mysql_stmt_close(stmt->stmt);
		APP.ERR_LOG("MysqlConnect", "out of memory");
		return;
	}
	std::shared_ptr<char>(blobBuf, [](char *p)->void { delete[] p; });

	params[0].buffer_type = MYSQL_TYPE_BLOB;
	params[0].buffer_length = 10240;
	params[0].length = &len;
	params[0].is_null = 0;
	params[0].buffer = blobBuf;
	memcpy(blobBuf, buf, len);

	// bind input arguments
	if (mysql_stmt_bind_param(stmt->stmt, params))
	{
		std::cout << mysql_stmt_error(stmt->stmt) << std::endl;
	}

	if (mysql_stmt_execute(stmt->stmt))
	{
		std::cout << mysql_stmt_error(stmt->stmt) << std::endl;
	}
}

void gsf::modules::MysqlConnect::execSql(gsf::ModuleID target, int oper, const std::string &sql, std::function<void (gsf::ModuleID, gsf::ArgsPtr)> callback)
{
	MYSQL_RES *result = nullptr;
	MYSQL_FIELD *fields = nullptr;

	int _progress = 1;

	auto errf = [&](const std::string &err) {
		if (callback) {
			callback(target, gsf::makeArgs(oper, false, 0, 0, err));
		}
		else {
			APP.ERR_LOG("dbProxy", "query", " {}", err);
		}
	};

	if (nullptr == basePtr_) {
		APP.ERR_LOG("MysqlConnect", "not connected!");
		return ;
	}

	if (mysql_query(basePtr_, sql.c_str())) {
		errf(mysql_error(basePtr_));
		return;
	}

	result = mysql_store_result(basePtr_);
	if (nullptr == result) {
		if (callback) {
			callback(target, gsf::makeArgs(oper, true, 0, 0, "success!"));
		}
		return;
	}

	int32_t rowCount = static_cast<int32_t>(mysql_affected_rows(basePtr_));
	if (0 == rowCount) {
		mysql_free_result(result);
		errf("row = 0");
		return;
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
	unsigned long* lengths = mysql_fetch_lengths(result);
	size_t col = 0;

	while (nullptr != row)
	{
		auto argsPtr = gsf::ArgsPool::get_ref().get();
		argsPtr->push(oper);
		argsPtr->push(true);
		argsPtr->push(rowCount);
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
			case gsf::at_block:
				std::string _str = "";
				_str.assign(row[col], lengths[col]);
				argsPtr->push_string(_str);
				break;
			}
		}
		callback(target, std::move(argsPtr));

		_progress++;

		row = mysql_fetch_row(result);
	}

	if (nullptr != result)
	{
		mysql_free_result(result);
	}
}

void gsf::modules::MysqlConnect::perpare(const std::string &sql, SqlStmtPtr &stmtPtr)
{

	auto itr = prepared_stmt_map.find(sql);
	if (itr != prepared_stmt_map.end()) {
		stmtPtr = itr->second;
		return;
	}

	do {
		if (nullptr == basePtr_) {
			std::cout << "mysql unuseable!" << std::endl;
			break;
		}

		stmtPtr = std::make_shared<SqlStmt>();
		stmtPtr->sql = sql;

		stmtPtr->stmt = mysql_stmt_init(basePtr_);
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

}