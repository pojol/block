#ifndef _SINGLE_HEADER_
#define _SINGLE_HEADER_

namespace gsf
{
	namespace utils
	{
		template<class T>
		class Singleton
		{
		public:
			virtual ~Singleton(){}

			static T * instance()
			{
				if (intance_ptr_){
					return intance_ptr_;
				}
				else {
					return intance_ptr_ = new T();
				}
			}
			//warning.  a single instance must new by user. 
			static T & get_ref() { return *instance(); }
			static T * get_ptr() { return instance(); }

		public:
			Singleton() = default;

		private:
			static T * intance_ptr_;
			Singleton(const Singleton<T> &) = delete;
		};

		template<class T>
		T * Singleton<T>::intance_ptr_ = nullptr;
	}
}

#endif