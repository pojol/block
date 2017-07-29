#ifndef _GSF_MSG_BLOCK_HEADER_
#define _GSF_MSG_BLOCK_HEADER_

#include <memory>
#include <core/event.h>

namespace gsf
{
	// 对二进制块做下简单封装， 方便回收内存。

	struct Block
	{
		Block(uint32_t size)
		{
			buf_ = (char*)malloc(size);
			size_ = size;
			pos_ = 0;
		}

		Block(SessionID fd, MsgID msg_id, const std::string &block)
		{
			auto _len = block.length();
			size_ = _len + sizeof(MsgHeadLen) + sizeof(MsgID);
			pos_ = 0;

			buf_ = (char*)malloc(size_);
			
			push_head_len(size_, buf_ + pos_);
			push_msg_id(msg_id, buf_ + pos_);

			memcpy(buf_ + pos_, block.c_str(), _len);
		}

		void push_head_len(MsgHeadLen val, char *buf)
		{
			*(MsgHeadLen*)(void*)buf = val;
			pos_ += sizeof(MsgHeadLen);
		}

		void push_msg_id(MsgID val, char *buf)
		{
			*(MsgID*)(void*)buf = val;
			pos_ += sizeof(MsgID);
		}

		MsgID get_msg_id()
		{
			MsgID _id = *(MsgID*)(void*)(buf_ + sizeof(MsgHeadLen));
			return _id;
		}

		int32_t get_body_size()
		{
			return (size_ - (sizeof(MsgHeadLen) + sizeof(MsgID)));
		}

		int32_t get_head_size()
		{
			return (sizeof(MsgHeadLen) + sizeof(MsgID));
		}

		bool check()
		{
			MsgHeadLen _len = *(MsgHeadLen*)(void*)(buf_);
			if (_len != size_) {
				return false;
			}

			MsgID _id = get_msg_id();
			if (_id <= 0 || _id > MsgMax) {
				return false;
			}

			return true;
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
