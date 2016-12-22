#ifndef _MESSAGE_BINDER_HEADER_
#define _MESSAGE_BINDER_HEADER_

#include <memory>
#include <unordered_map>

#include <event2/buffer.h>

#include "message.h"

//
#include <iostream>
#include <thread>

namespace gsf
{
	namespace network
	{
		template <typename MSG>
		struct DispatchMsg
		{
			typedef void(*DispatchFunc)(void *obj, std::shared_ptr<MSG>);
			DispatchFunc func_;
			void *obj_;

			DispatchMsg(DispatchFunc func, void *obj)
				: func_(func)
				, obj_(obj)
			{}

			void dispatch(std::shared_ptr<MSG> msg)
			{
				func_(obj_, msg);
			}
		};

		template<typename MSG, class T,void (T::*func)(std::shared_ptr<MSG>)>
		inline void DispatchFuncT(void *obj, std::shared_ptr<MSG> msg)
		{
			(((T*)obj)->*func)(msg);
		}

		class Binder
		{
		public:
			virtual void construct_msg(int session_id, ::evbuffer *buf) = 0;
		};

		template <typename MSG>
		class MessageBinder : public Binder
		{
		public:
			static MessageBinder<MSG>& instance();
			~MessageBinder();

			template <typename T, void(T::*func)(typename MSG::Ptr)>
			int regist_msg_proc(int msg_id, T *obj);

			int dispatch(typename MSG::Ptr msg);

			void construct_msg(int session_id, ::evbuffer *buf);

		private:
			MessageBinder();

			static MessageBinder<MSG> *instance_;

		private:
			char *recv_size_;
			std::unordered_map<int, DispatchMsg<MSG>> map_;
		};

		template <typename MSG>
		MessageBinder<MSG>::~MessageBinder()
		{
			map_.clear();
		}

		template <typename MSG>
		MessageBinder<MSG>::MessageBinder()
		{
			recv_size_ = (char*)malloc(4);
		}

		template <typename MSG>
		MessageBinder<MSG>* MessageBinder<MSG>::instance_ = NULL;

		template <typename MSG>
		MessageBinder<MSG>& MessageBinder<MSG>::instance()
		{
			static MessageBinder<MSG> instance_;
			return instance_;
		}

		template <typename MSG>
		template<class T, void (T::*func)(typename MSG::Ptr)>
		int MessageBinder<MSG>::regist_msg_proc(int msg_id, T* obj)
		{
			auto itr = map_.find(msg_id);
			if (itr == map_.end()){
				DispatchMsg<MSG> dispatcher(DispatchFuncT<MSG, T, func>, obj);
				map_.insert(std::make_pair(msg_id, dispatcher));
				return 0;
			}
			else {
				return -1;
			}
		}

		template <typename MSG>
		void MessageBinder<MSG>::construct_msg(int session_id, ::evbuffer *buf)
		{
			uint32_t _len = 1;
			while (_len)
			{
				evbuffer_copyout(buf, recv_size_, 4);
				uint32_t _msg_len = *reinterpret_cast<uint32_t*>(recv_size_);

				auto _block_ptr = std::make_shared<stream::Block>(_msg_len);
				_block_ptr->size_ = _msg_len;

				evbuffer_remove(buf, _block_ptr->buf_, _msg_len);
				//memcpy(_block_ptr->buf_, buf, len);

				typename MSG::Ptr msg = std::make_shared<MSG>(_block_ptr, 0, _msg_len, session_id);
				msg->pase_message_id();
				dispatch(msg);

				_len = evbuffer_get_length(buf);
			}

			evbuffer_free(buf);
		}

		template <typename MSG>
		int MessageBinder<MSG>::dispatch(typename MSG::Ptr msg)
		{
			auto itr = map_.find(msg->get_message_id());
			if (itr != map_.end()){
				itr->second.dispatch(msg);
			}
			else {
				return -1;
			}

			return 0;
		}
	}
}

#endif
