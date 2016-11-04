#ifndef _GSF_ACCEPTOR_HEADER_
#define _GSF_ACCEPTOR_HEADER_

#include <stdint.h>
#include <string>

namespace gsf
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
		Acceptor(AcceptorConfig &config, AcceptHandler *handler);

        Session * get_session();

        void hander_new_connect();

	private:
		AcceptorConfig config_;
		AcceptHandler *handler_;

        Session *session_;
	};
}


#endif
