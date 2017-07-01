#ifndef _GSF_TYPES_HEADER_
#define _GSF_TYPES_HEADER_

#include <stdint.h>

namespace gsf
{
	typedef int32_t ModuleID;
	static const int32_t ModuleNil = 0;

	typedef int32_t EventID;
	static const int32_t EventNil = 0;

	typedef uint64_t TimerID;
	static const uint64_t TimerNil = 0;

	typedef uint16_t SessionID;
	static const uint16_t SessionNil = 0;

	typedef uint16_t MsgID;
	static const uint16_t MsgNil = 0;
	
	typedef int32_t ErrorID;
	static const uint32_t ErrNil = 0;

	typedef int32_t MsgHeadLen;
	static const int32_t MsgHeadLenNil = 0;

	static const uint16_t ARGS_MAX_SIZE = 1024;				// 用于设定 args_pool中 自动分配的args容量。

	static const uint32_t NETWORK_CONNECT_MAX = 65534;		//
	static const uint32_t SESSION_READ_BUFFER_SIZE = 0;		//
	static const uint32_t SESSION_WRITE_BUFFER_SIZE = 0;	//
	
	static const uint32_t MSGLEN_CLIENTSIZE = 1024 * 4;		//
	static const uint32_t MSGLEN_RPCSIZE = 1024 * 10;		//
}

#endif
