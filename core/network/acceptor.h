#ifndef _GSF_ACCEPTOR_HEADER_
#define _GSF_ACCEPTOR_HEADER_

#include <string>
#include <memory>

#include <event2/bufferevent.h>
#include <event2/listener.h>

#include <functional>

namespace gsf
{
	namespace network
	{
		class AcceptHandler;
		class Session;

		struct AcceptorConfig
		{
			int32_t port;
			std::string address;
		};

		class Acceptor
		{
		public:
			Acceptor(const AcceptorConfig &config, std::function<void(int)> func);
			~Acceptor();

			int close();

			void handler_new_connect(int32_t session_id);

			AcceptorConfig &get_config();

			int get_id() const;

		private:
			AcceptorConfig config_;

			std::function<void(int)> accept_handler_;

			::evconnlistener *listener_ptr_;
		};
	}
}


#endif
