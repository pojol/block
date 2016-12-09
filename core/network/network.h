#ifndef _GSF_NETWORK_HEADER_
#define _GSF_NETWORK_HEADER_

#include <stdint.h>
#include <vector>
#include <memory>
#include <functional>


namespace gsf
{
	namespace network
	{
		struct AcceptorConfig;
		struct ConnectorConfig;
		struct NetworkConfig;

		class Binder;

		//event 
		typedef std::function<void(int)> NewConnectFunc;
		typedef std::function<void(int, int, std::string &, int)> ConnectFailedFunc;
		typedef std::function<void(int, int)> DisConnectFunc;
		typedef std::function<void()> UpdateFunc;

		class Network
		{
		public:
			~Network();

			static Network& instance();

			//interface 

			int init(const NetworkConfig &config);

			int start(UpdateFunc func);

			void uninit();

			int listen(const AcceptorConfig &config, NewConnectFunc func);

			int connect(const ConnectorConfig &config, NewConnectFunc new_connect, ConnectFailedFunc connect_failed);

			void regist_binder(Binder *binder);

			//! temp

			void write(uint32_t session_id, const char *data, uint32_t len);

		private:
			Network();
			static Network* instance_;
		};

	}
}


#endif
