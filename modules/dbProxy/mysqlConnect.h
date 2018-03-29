#ifndef _GSF_MYSQL_CONNECTOR_HEADER_
#define _GSF_MYSQL_CONNECTOR_HEADER_

#include <string>
#include <memory>
#include <core/application.h>
#include "mysql.h"

namespace gsf 
{
	namespace modules
	{
		struct SqlStmt
		{
			SqlStmt()
				: result(nullptr)
				, stmt(nullptr)
			{}

			~SqlStmt()
			{
				if (stmt) {
					mysql_free_result(result);
					mysql_stmt_close(stmt);
				}
			}

			uint32_t params;
			uint32_t columns;

			bool is_query;
			bool is_prepared;

			std::string sql = "";

			MYSQL_RES*		result;
			MYSQL_STMT*		stmt;
		};

		typedef std::shared_ptr<SqlStmt> SqlStmtPtr;

		class MysqlConnect
		{
		public:
			MysqlConnect();
			~MysqlConnect();
			
			MysqlConnect(const MysqlConnect &) = delete;
			MysqlConnect & operator = (const MysqlConnect &) = delete;

			bool init(const std::string &host, int port, const std::string &user, const std::string &pwd, const std::string &name);

			void execute(const std::string &query, const char *buf, unsigned long len);

			void execSql(gsf::ModuleID target, int oper, const std::string &sql, std::function<void (gsf::ModuleID, gsf::ArgsPtr)> callback);

		private:

			void perpare(const std::string &sql, SqlStmtPtr &stmtPtr);

		private:
			MYSQL *basePtr_ = nullptr;

			std::unordered_map<std::string, SqlStmtPtr> prepared_stmt_map;
		};

		typedef std::shared_ptr<MysqlConnect> MysqlPtr;
	}
}

#endif
