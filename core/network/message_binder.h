#ifndef _MESSAGE_BINDER_HEADER_
#define _MESSAGE_BINDER_HEADER_

#include <memory>
#include <unordered_map>

namespace gsf
{
	namespace network
	{
		template <typename MSG>
		struct DispatchMsg
		{
			typedef void(*DispatchFunc)(void *obj, std::shared_ptr<MSG> &);
			DispatchFunc func_;
			void *obj_;

			DispatchMsg(DispatchFunc func, void *obj)
				: func_(func)
				, obj_(obj)
			{}

			void dispatch(std::shared_ptr<MSG> &msg)
			{
				func_(obj_, msg);
			}
		};

		template <typename MSG>
		class MessageBinder
		{
		public:
			static MessageBinder<MSG>& instance();
			~MessageBinder();

			template <typename T, void(T::*func)(typename MSG::Ptr &)>
			int regist_msg_proc(int msg_id, T *obj);

			int dispatch(typename MSG::Ptr &msg);

		private:
			static MessageBinder<MSG> *instance_;

		private:
			std::unordered_map<int, DispatchMsg<MSG>> map_;
		};

		template <typename MSG>
		MessageBinder<MSG>::~MessageBinder()
		{
			map_.clear();
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
		template<class T, void (T::*func)(typename MSG::Ptr &)>
		int MessageBinder<MSG>::regist_msg_proc(int msg_id, T* obj)
		{
			DispatchMsg<MSG> dispatcher(func, obj);
			map_.insert(std::make_pair(msg_id, dispatcher));
		}

		template <typename MSG>
		int MessageBinder<MSG>::dispatch(typename MSG::Ptr &msg)
		{
			auto itr = map_.find(msg->get_message_id());
			if (itr != map_.end){
				itr->second->dispatch(msg);
			}
			else {
				return -1;
			}

			return 0;
		}
	}
}

#endif