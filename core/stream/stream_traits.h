#ifndef _STREAM_TRAITS_HEADER_
#define _STREAM_TRAITS_HEADER_

#include "endian.h"

#include <stdint.h>

namespace gsf
{
	namespace stream
	{
		//! 默认流特征类
		struct DefaultStreamTraits
		{
		public:
			typedef LittleEndian	IntEndian;				//!< 整数类型序列
			typedef uint16_t		LengthType;				//!< 字符串长度类型
		};
	}
}

#endif
