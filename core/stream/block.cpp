#include "block.h"

gsf::stream::Block::Block(size_t total /*= 64*/)
{
	total_ = total;
	if (total_ == 0){
		total_ = 1;
	}

	size_ = 0;
	buf_ = static_cast<char*>(malloc(total_));
}

gsf::stream::Block::~Block()
{
	if (buf_)
	{
		free(buf_);
		buf_ = NULL;
	}
}

bool gsf::stream::Block::inc_size(uint32_t inc)
{
	uint32_t _tmp_size = size_ + inc;

	if (_tmp_size < size_) {
		throw gsf::stream::OutOfBound("wrapped in Block::incDataSize");
	}

	if (_tmp_size <= total_) {
		size_ = _tmp_size;
		return false;
	}

	uint32_t _double_size = total_;
	while (_double_size < _tmp_size)
	{
		_double_size <<= 1;
	}

	char* _tmp_buf = static_cast<char*>(malloc(_double_size));
	memcpy(_tmp_buf, buf_, size_);
	free(buf_);

	buf_ = _tmp_buf;
	size_ = _tmp_size;
	total_ = _double_size;

	return true;
}