#include "message.h"



gsf::network::Message::Message()
	: message_id_(0)
	, session_id_(0)
{
	is_ptr_ = nullptr;
	os_ptr_ = new stream::OStream();
}

gsf::network::Message::Message(stream::BlockPtr blockPtr, uint32_t start, uint32_t len, uint32_t session_id)
	: message_id_(0)
	, session_id_(session_id)
{
	os_ptr_ = nullptr;
	is_ptr_ = new stream::IStream(blockPtr, start, len);
}

gsf::network::Message::~Message()
{
	if (is_ptr_){
		delete is_ptr_;
		is_ptr_ = nullptr;
	}

	if (os_ptr_){
		delete os_ptr_;
		os_ptr_ = nullptr;
	}
}

void gsf::network::Message::pase_message_id()
{
	is_ptr_->skip(sizeof(uint32_t));
	*is_ptr_ >> message_id_;
}

uint32_t gsf::network::Message::get_message_id() const
{
	return message_id_;
}

uint32_t gsf::network::Message::get_session_id() const
{
	return session_id_;
}

gsf::stream::BlockPtr gsf::network::Message::get_out_block() const
{
	return os_ptr_->getBlock();
}

gsf::stream::IStream * gsf::network::Message::get_istream()
{
	return is_ptr_;
}

gsf::stream::OStream * gsf::network::Message::get_ostream()
{
	return os_ptr_;
}

