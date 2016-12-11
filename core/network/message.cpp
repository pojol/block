#include "message.h"



gsf::network::Message::Message()
	: message_id_(0)
	, session_id_(0)
{
	block_ptr_ = std::make_shared<stream::Block>();
}
