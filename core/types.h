#pragma once

#include <stdint.h>

namespace gsf
{
	typedef int32_t ModuleID;
	static const int32_t ModuleNil = INT32_MAX;

	typedef int32_t EventID;
	static const int32_t EventNil = INT32_MAX;

	typedef uint64_t TimerID;
	static const uint64_t TimerNil = UINT64_MAX;

	typedef uint16_t SessionID;
	static const uint16_t SessionNil = UINT16_MAX;

	typedef int32_t NodeID;
	static const int32_t NodeNil = INT32_MAX;

	typedef int32_t MsgID;
	static const int32_t MsgNil = INT32_MAX;
	static const int32_t MsgMax = 65534;

	static const uint16_t LogErr = 3;
	static const uint16_t LogWarning = 2;
	static const uint16_t LogInfo = 1;
	static const uint16_t LogDebug = 0;
	
	typedef int32_t ErrorID;
	static const int32_t ErrNil = INT32_MAX;

	typedef int32_t MsgHeadLen;
	static const int32_t MsgHeadLenNil = 0;

	static const uint16_t ARGS_MAX_SIZE = 1024;				//

	static const uint32_t NETWORK_CONNECT_MAX = 65534;		//
	static const uint32_t SESSION_READ_BUFFER_SIZE = 0;		//
	static const uint32_t SESSION_WRITE_BUFFER_SIZE = 0;	//
	
	static const uint32_t MSGLEN_CLIENTSIZE = 1024 * 4;		//
	static const uint32_t MSGLEN_RPCSIZE = 1024 * 10;		//

	// args type
	static const uint8_t at_uint8 = 1;
	static const uint8_t at_int8 = 2;
	static const uint8_t at_uint16 = 3;
	static const uint8_t at_int16 = 4;
	static const uint8_t at_uint32 = 5;
	static const uint8_t at_int32 = 6;
	static const uint8_t at_uint64 = 7;
	static const uint8_t at_int64 = 8;
	static const uint8_t at_bool = 9;
	static const uint8_t at_float = 10;
	static const uint8_t at_double = 11;
	static const uint8_t at_string = 12;
	static const uint8_t at_list = 13;
	static const uint8_t at_vec = 14;
	static const uint8_t at_map = 15;
	static const uint8_t at_block = 16;
	static const uint8_t at_eof = 20;
}
