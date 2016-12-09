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
			int len = evbuffer_get_length(buf);
			char * _buf = (char *)malloc(len);
			evbuffer_remove(buf, _buf, len);

			std::cout << "thread : " << std::this_thread::get_id() << " session : " << session_id << " recv : " << buf << std::endl;

			evbuffer_free(buf);

			typename MSG::Ptr msg = std::make_shared<MSG>();
			dispatch(msg);
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
