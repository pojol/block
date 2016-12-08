#ifndef _MESSAGE_HEADER_
#define _MESSAGE_HEADER_

#include <stdint.h>

namespace gsf
{
	namespace network
	{

		class Message
		{
		public:
			~Message();
			Message();

			uint32_t get_message_id() const;
			uint32_t get_session_id() const;

		private:
			uint32_t message_id_;
			uint32_t session_id_;
		};

	}
}

#endif