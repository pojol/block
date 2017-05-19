#ifndef _GSF_TYPES_HEADER_
#define _GSF_TYPES_HEADER_

#include <stdint.h>

namespace gsf
{
	typedef uint32_t ModuleID;
	static const uint32_t ModuleNil = 0;

	typedef uint32_t EventID;
	static const uint32_t EventNil = 0;

	typedef uint64_t TimerID;
	static const uint64_t TimerNil = 0;

	typedef uint16_t SessionID;
	static const uint16_t SessionNil = 0;

	typedef uint16_t MsgID;
	static const uint16_t MsgNil = 0;
	
	typedef int32_t ErrorID;
	static const uint32_t ErrNil = 0;

	static const uint32_t LogInfo = 1;
	static const uint32_t LogWarning = 2;
	static const uint32_t LogError = 3;

	static const uint32_t NETWORK_CONNECT_MAX = 65534;		//
	static const uint32_t SESSION_READ_BUFFER_SIZE = 0;		//
	static const uint32_t SESSION_WRITE_BUFFER_SIZE = 0;	//
	
	static const uint32_t MSGLEN_PACKAGESIZE = 4;			//
	static const uint32_t MSGLEN_MSGID = 2;					//
	static const uint32_t MSGLEN_CLIENTSIZE = 1024 * 4;		//
	static const uint32_t MSGLEN_RPCSIZE = 1024 * 10;		//
}

#endif
