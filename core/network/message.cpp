#include "message.h"

gsf::network::Block::Block(int len /* = 64 */)
{
	total_ = len;
	if (total_ == 0) total_ = 1;
	size_ = 0;
	buf_ = static_cast<char*>(malloc(total_));
}

gsf::network::Block::~Block()
{
	if (buf_)
	{
		free(buf_);
		buf_ = nullptr;
	}
}

bool gsf::network::Block::inc_size(uint32_t inc)
{
	uint32_t _tmp = size_ + inc;

	if (_tmp < size_)
	{
		return false;
	}

	if (_tmp <= total_)
	{
		size_ = _tmp;
		return false;
	}

	uint32_t _double_size = total_;
	while (_double_size < _tmp)
	{
		_double_size <<= 1;
	}

	char* _tmp_buf = static_cast<char*>(malloc(_double_size));
	memcpy(_tmp_buf, buf_, size_);

	free(buf_);

	buf_ = _tmp_buf;
	size_ = _tmp;
	total_ = _double_size;

	return true;
}

gsf::network::Message::Message()
	: message_id_(0)
	, session_id_(0)
{
	block_ptr_ = std::make_shared<Block>();
}
