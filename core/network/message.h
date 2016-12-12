#ifndef _MESSAGE_HEADER_
#define _MESSAGE_HEADER_

#include <stdint.h>
#include <memory>

#include "../stream/istream.h"
#include "../stream/ostream.h"

namespace gsf
{
	namespace network
	{
		class Message
		{
		public:
			typedef std::shared_ptr<Message> Ptr;

			~Message();

			Message();

			Message(stream::BlockPtr blockPtr, uint32_t start, uint32_t len, uint32_t session_id);

			void pase_message_id();
			
			uint32_t get_message_id() const { return message_id_; }
			uint32_t get_session_id() const { return session_id_; }

			uint32_t get_message_len() const { return 0; }

			stream::BlockPtr get_out_block() const { return os_ptr_->getBlock(); }

			stream::IStream * get_istream() { return is_ptr_; }
			stream::OStream * get_ostream() { return os_ptr_; }

		protected:
			stream::IStream *is_ptr_;
			stream::OStream *os_ptr_;

			uint32_t message_id_;
			uint32_t session_id_;
		};

	}
}

#endif