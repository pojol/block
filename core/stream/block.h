#ifndef _BLOCK_HEADER_
#define _BLOCK_HEADER_

#include <string>
#include <string.h>
#include <memory>

#include <stdint.h>

#include "OutOfBound.h"

namespace gsf
{
	namespace stream
	{
		//! 内存块结构
		class Block
		{
		public:
			Block(size_t total = 64);
			
			virtual ~Block();

			bool inc_size(uint32_t inc);

			char * buf_;		//!< 缓冲区指针
			uint32_t size_;		//!< 数据长度
			uint32_t total_;		//!< 总的数据大小
		};
		
		typedef std::shared_ptr<Block> BlockPtr;
	}
}

#endif
