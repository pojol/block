#ifndef _MESSAGE_HEADER_
#define _MESSAGE_HEADER_

#include <stdint.h>
#include <memory>

#include "../stream/block.h"

namespace gsf
{
	namespace network
	{
		class Message
		{
		public:
			typedef std::shared_ptr<Message> Ptr;

			~Message(){}

			Message();

			Message(stream::BlockPtr blockPtr, uint32_t session_id)
				: block_ptr_(blockPtr)
				, session_id_(session_id)
			{
			}

			void pase_message_id();
			
			uint32_t get_message_id() const { return message_id_; }
			uint32_t get_session_id() const { return session_id_; }

			uint32_t get_message_len() const { return 0; }

			stream::BlockPtr get_block() const { return block_ptr_; }

		protected:
			stream::BlockPtr block_ptr_;

			uint32_t message_id_;
			uint32_t session_id_;
		};

	}
}

#endif