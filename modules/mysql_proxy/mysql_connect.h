#ifndef _GSF_MYSQL_CONNECTOR_HEADER_
#define _GSF_MYSQL_CONNECTOR_HEADER_

#include <string>
#include <core/event.h>

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

			void update(const std::string &order, const gsf::ArgsPtr &args);

			void execute(gsf::ModuleID target, gsf::ModuleID remote, const std::string &sql, std::function<void (gsf::ModuleID, const gsf::ArgsPtr &)> callback);

		private:

			void perpare(const std::string &sql, SqlStmtPtr &stmtPtr);

			void startThread();
			void endThread();

		private:
			MYSQL *base = nullptr;

			std::unordered_map<std::string, SqlStmtPtr> prepared_stmt_map;
		};
	}
}

#endif
