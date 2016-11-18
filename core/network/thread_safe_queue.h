#ifndef _THREAD_SAFE_QUEUE_HEADER_
#define _THREAD_SAFE_QUEUE_HEADER_

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

namespace gsf
{
	template <typename T>
	class ThreadSafeQueue
    {
		mutable std::mutex mtx_;
		std::queue<T> data_queue_;
		std::condition_variable data_cond_;

	public:
		ThreadSafeQueue(){}

		ThreadSafeQueue(const ThreadSafeQueue &other)
		{
			std::lock_guard<std::mutex> lk(other.mtx_);
			data_queue_ = other.data_queue_;
		}

		void push(T new_value)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			data_queue_.push(new_value);
			data_cond_.notify_one();
		}

		void wait_and_pop(T &value)
		{
			std::unique_lock<std::mutex> lk(mtx_);
			data_cond_.wait(lk, [this]{ return !data_queue_.empty(); });
			value = data_queue_.front();
			data_queue_.pop();
		}

		std::shared_ptr<T> wait_and_pop()
		{
			std::unique_lock<std::mutex> lk(mtx_);
			data_cond_.wait(lk, [this]{ return !data_queue_.empty(); });
			std::shared_ptr<T> res(std::make_shared<T>(data_queue_.front()));
			data_queue_.pop();
			return res;
		}

		bool try_pop(T &value)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			if (data_queue_.empty()){
				return false;
			}

			value = data_queue_.front();
			data_queue_.pop();

			return true;
		}

		std::shared_ptr<T> try_pop()
		{
			std::lock_guard<std::mutex> lk(mtx_);
			if (data_queue_.empty()){
				return std::shared_ptr<T>();
			}

			std::shared_ptr<T> res(std::make_shared<T>(data_queue_.front()));
			data_queue_.pop();
			
			return res;
		}

		bool empty() const
		{
			std::lock_guard<std::mutex> lk(mtx_);
			return data_queue_.empty();
		}
    };
}

#endif