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

		class Network
		{

		public:
			~Network();

			static Network& instance();

			int init(const NetworkConfig &config);

			int start();

			void uninit();

			//! temp
			void write(uint32_t session_id, const char *data, uint32_t len);

			int make_acceptor(const AcceptorConfig &config, std::function<void(int)> func);

		private:
			Network();
			static Network* instance_;
		};
	}
}


#endif
