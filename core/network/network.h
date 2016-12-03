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
		struct NetworkConfig;

		//event 
		typedef std::function<void(int)> NewConnectFunc;
		typedef std::function<void(int, int)> DisConnectFunc;
		typedef std::function<void()> UpdateFunc;

		class Network
		{

		public:
			~Network();

			static Network& instance();

			//interface 

			int init(const NetworkConfig &config);

			int start();

			void uninit();

			int listen(const AcceptorConfig &config, NewConnectFunc func);

			//! temp
			int make_connect();

			void write(uint32_t session_id, const char *data, uint32_t len);

		private:
			Network();
			static Network* instance_;
		};
	}
}


#endif
