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

		Block(SessionID fd, MsgID msg_id, uint32_t len)
			: size_(len + MSGLEN_PACKAGESIZE + MSGLEN_MSGID)
			, pos_(0)
		{
			//! 暂不考虑大小端
			buf_ = (char*)malloc(size_);

			push_uint32(size_, buf_ + pos_);
			pos_ += MSGLEN_PACKAGESIZE;

			push_uint16(msg_id, buf_ + pos_);
			pos_ += MSGLEN_MSGID;
		}

		void push_uint32(uint32_t val, char *buf)
		{
			*(uint32_t*)(void*)buf = val;
		}

		void push_uint16(uint16_t val, char *buf)
		{
			*(uint16_t*)(void*)buf = val;
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
