#ifndef _GSF_BLOCK_HEADER_
#define _GSF_BLOCK_HEADER_

#include <memory>

namespace gsf
{
	// 对二进制块做下简单封装， 方便回收内存。

	struct Block
	{
		Block(uint32_t size)
			: size_(size)
			, pos_(0)
		{
			buf_ = (char*)malloc(size_);
		}

		Block(uint32_t fd, uint32_t msg_id, uint32_t len)
			: size_(len + sizeof(fd) + sizeof(msg_id))
			, pos_(0)
		{
			//! 暂不考虑大小端
			buf_ = (char*)malloc(size_);

			push_uint32(size_, buf_ + pos_);
			pos_ += sizeof(uint32_t);

			push_uint32(msg_id, buf_ + pos_);
			pos_ += sizeof(uint32_t);
		}

		void push_uint32(uint32_t val, char *buf)
		{
			*(uint32_t*)(void*)buf = val;
		}

		~Block()
		{
			free(buf_);
			size_ = 0;
			pos_ = 0;
		}

		char *buf_;
		uint32_t size_;
		uint32_t pos_;
	};

	typedef std::shared_ptr<Block> BlockPtr;
}

#endif
