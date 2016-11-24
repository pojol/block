#ifndef _GSF_ACCEPTOR_HEADER_
#define _GSF_ACCEPTOR_HEADER_

#include <string>
#include <memory>

#include <event2/bufferevent.h>
#include <event2/listener.h>


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

		class AcceptHandler
		{
		public:
			virtual ~AcceptHandler(){}

			virtual void handler_new_connection(int acceptor_id, int session_id) = 0;
		};

		class Acceptor
		{
		public:
			Acceptor(int id, const AcceptorConfig &config);
			~Acceptor();

			int open(AcceptHandler *accept_handler);

			int close();

			void handler_new_connect(int32_t session_id);

			AcceptorConfig &get_config();

			int get_id() const;

		private:
			AcceptorConfig config_;
			AcceptHandler *handler_;

			::evconnlistener *listener_ptr_;

			int id_;
		};
	}
}


#endif
