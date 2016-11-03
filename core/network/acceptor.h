#ifndef _GSF_ACCEPTOR_HEADER_
#define _GSF_ACCEPTOR_HEADER_

#include <stdint.h>
#include <string>

namespace gsf
{
	class AcceptHandler;

	struct AcceptorConfig
	{
		int32_t port;
		std::string address;
	};

	class Acceptor
	{
	public:
		Acceptor(AcceptorConfig &config, AcceptHandler *handler);

		int32_t open();
	};
}


#endif
