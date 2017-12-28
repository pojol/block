#ifndef _GSF_MYSQL_CONNECTOR_HEADER_
#define _GSF_MYSQL_CONNECTOR_HEADER_

#include <string>
#include <core/event.h>

#include "mysql.h"

namespace gsf 
{
	namespace modules
	{
		class MysqlConnect
		{
		public:
			MysqlConnect();
			~MysqlConnect();
			
			MysqlConnect(const MysqlConnect &) = delete;
			MysqlConnect & operator = (const MysqlConnect &) = delete;

			bool init(const std::string &host, int port, const std::string &user, const std::string &pwd, const std::string &name);

			bool execute(const std::string &order, const gsf::ArgsPtr &args);

			bool query(const std::string &sql);

			void startThread();
			void endThread();

		private:
			MYSQL *base = nullptr;

		};
	}
}

#endif
