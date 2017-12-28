#include "mysql_connect.h"

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
		std::cout << "err" << std::endl;
		return false;
	}
	
	base = mysql_real_connect(init, host.c_str(), user.c_str(), pwd.c_str(), name.c_str(), port, nullptr, 0);
	if (nullptr == base) {
		mysql_close(base);
		return false;
	}

	return true;
}

bool gsf::modules::MysqlConnect::execute(const std::string &order, const gsf::ArgsPtr &args)
{
	

	return true;
}

bool gsf::modules::MysqlConnect::query(const std::string &sql)
{
	MYSQL_RES *result = nullptr;
	MYSQL_FIELD *fields = nullptr;

	if (mysql_query(base, sql.c_str())) {
		std::cout << sql << " " << mysql_error(base) << std::endl;
		return false;
	}

	result = mysql_store_result(base);
	if (nullptr == result) {
		std::cout << sql << " " << mysql_error(base) << std::endl;
		return false;
	}

	uint64_t rowCount = mysql_affected_rows(base);
	if (0 == rowCount) {
		mysql_free_result(result);
		std::cout << "row 0 " << std::endl;
	}

	uint32_t fieldCount = mysql_field_count(base);
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

		std::cout << argsPtr->toString() << std::endl;
		std::cout << "---" << std::endl;

		row = mysql_fetch_row(result);
	}

	if (nullptr != result)
	{
		mysql_free_result(result);
	}

	return false;
}

void gsf::modules::MysqlConnect::startThread()
{
	if (nullptr != base) {
		mysql_thread_init();
	}	
}

void gsf::modules::MysqlConnect::endThread()
{
	if (nullptr != base) {
		mysql_thread_end();
	}
}