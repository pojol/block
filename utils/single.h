#pragma once

#include <assert.h>

namespace block
{
	namespace utils
	{
		template<class T>
		class Singleton
		{
		public:
			virtual ~Singleton(){
				assert(intance_ptr_);
				intance_ptr_ = nullptr;
			}
 
			static T & get_ref() {
				assert(intance_ptr_); return (*intance_ptr_);
			}
			static T * get_ptr() { 
				assert(intance_ptr_); return intance_ptr_;
			}

		public:
			Singleton()
			{
				intance_ptr_ = static_cast<T*>(this);
			}

		private:
			static T * intance_ptr_;
			Singleton(const Singleton<T> &) = delete;
		};

		template<class T>
		T * Singleton<T>::intance_ptr_ = nullptr;
	}
}