#ifndef _GSF_BLOCK_HEADER_
#define _GSF_BLOCK_HEADER_

#include <memory>

namespace gsf
{
	// 对char*做下简易封装，方便回收和使用

	struct Block
	{
		Block(uint32_t size)
			: size_(size)
		{
			buf_ = (char*)malloc(size_);
		}

		~Block()
		{
			free(buf_);
		}

		void push_msghead(uint32_t len, uint32_t msg_id)
		{
			// 这里要考虑下大小端的问题。
			
		}

		char *buf_;
		uint32_t size_;
	};

	typedef std::shared_ptr<Block> BlockPtr;
}

#endif
