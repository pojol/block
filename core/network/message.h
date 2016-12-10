#ifndef _MESSAGE_HEADER_
#define _MESSAGE_HEADER_

#include <stdint.h>
#include <memory>

namespace gsf
{
	namespace network
	{

		struct Block
		{
			typedef std::shared_ptr<Block> Ptr;

			Block(int len = 64);
			~Block();

			bool inc_size(uint32_t inc);

			char *buf_;
			uint32_t size_;
			uint32_t total_;
		};

		class Message
		{
		public:
			typedef std::shared_ptr<Message> Ptr;

			~Message(){}

			Message();

			Message(Block::Ptr blockPtr, uint32_t session_id)
				: block_ptr_(blockPtr)
				, session_id_(session_id)
			{
			}
			
			uint32_t get_message_id() const { return message_id_; }
			uint32_t get_session_id() const { return session_id_; }

			uint32_t get_message_len() const { return 0; }

			Block::Ptr get_block() const { return block_ptr_; }

		protected:
			Block::Ptr block_ptr_;

			uint32_t message_id_;
			uint32_t session_id_;
		};

	}
}

#endif