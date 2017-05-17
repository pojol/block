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


	static const uint32_t NETWORK_CONNECT_MAX = 65534;		//acceptor 接受的最大连接数
	static const uint32_t SESSION_READ_BUFFER_SIZE = 0;		//读缓冲区大小，填0 libevent会自动分配
	static const uint32_t SESSION_WRITE_BUFFER_SIZE = 0;	//写缓冲区大小
	
	static const uint32_t MSGLEN_PACKAGESIZE = 4;			//包大小占用的字节长度
	static const uint32_t MSGLEN_MSGID = 2;					//消息ID占用的字节长度
	static const uint32_t MSGLEN_CLIENTSIZE = 1024 * 4;		//和客户端交互消息包的最大值
	static const uint32_t MSGLEN_RPCSIZE = 1024 * 10;		//服务器内部交互消息包的最大值
}

#endif
