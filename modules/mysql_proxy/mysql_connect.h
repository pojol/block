#ifndef _GSF_MYSQL_CONNECTOR_HEADER_
#define _GSF_MYSQL_CONNECTOR_HEADER_

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



		};
	}
}

#endif
