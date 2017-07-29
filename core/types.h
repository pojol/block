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

	typedef int32_t MsgID;
	static const int32_t MsgNil = 0;
	static const int32_t MsgMax = 65534;

	static const uint16_t LogErr = 0;
	static const uint16_t LogWarning = 1;
	static const uint16_t LogInfo = 2;
	
	typedef int32_t ErrorID;
	static const uint32_t ErrNil = 0;

	typedef int32_t MsgHeadLen;
	static const int32_t MsgHeadLenNil = 0;

	static const uint16_t ARGS_MAX_SIZE = 1024;				// �����趨 args_pool�� �Զ������args������

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
}

#endif
