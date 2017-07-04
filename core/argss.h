#ifndef _ARGSS_HEALDER_
#define _ARGSS_HEALDER_

#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>

#include <memory>
#include <functional>

#include "single.h"

namespace gsf
{
	struct Args
	{
		using TypeLen = uint16_t;

		Args();
		Args(int size);

		void push(const uint8_t &val);
		void push(const int8_t &val);

		void push(const uint16_t &val);
		void push(const int16_t &val);

		void push(const uint32_t &val);
		void push(const int32_t &val);

		void push(const uint64_t &val);
		void push(const int64_t &val);

		void push(const bool &val);
		void push(const float &val);
		void push(const double &val);

		void push(const char *val);
		void push(const std::string &val);

		template <typename T>
		void push_impl(const T &val);

		template <typename T>
		void push_list(std::list<T> &list);

		template <typename T>
		void push_vector(std::vector<T> &vec);

		template <typename Key, typename Value>
		void push_map(std::map<Key, Value> &map);

		/////////////////////////////

		uint8_t pop_ui8();
		int8_t pop_i8();

		uint16_t pop_ui16();
		int16_t pop_i16();

		uint32_t pop_ui32();
		int32_t pop_i32();

		uint64_t pop_ui64();
		int64_t pop_i64();

		bool pop_bool();
		float pop_float();
		double pop_double();

		std::string pop_string();

		template<typename T>
		std::list<T> pop_list();

		template <typename T>
		std::vector<T> pop_vec();

		template <typename Key, typename Value>
		std::map<Key, Value> pop_map();

		/////////////////////////////

		void flush();

		char * get_bits();

	private:

		char *buff_;
		char *read_;
		char *write_;
		char *tail_;

		uint32_t pos_;
		uint32_t size_;
	};

	template <typename T>
	void gsf::Args::push_impl(const T &val)
	{
		size_t _size = sizeof(T);
		*reinterpret_cast<T*>(write_) = val;
		write_ += _size;

		pos_ += _size;
	}

	void gsf::Args::push(const int32_t & val)
	{
		push_impl(val);
	}

	void gsf::Args::push(const char * val)
	{
		auto _len = std::strlen(val);

		push_impl(_len);
		
		memcpy(write_, val, _len);
		write_ += _len;

		pos_ += _len;
	}

	//////////////////////////////////////////////////////////////////////////


	int32_t Args::pop_i32()
	{
		auto _i32 = *reinterpret_cast<int32_t*>(read_);
		read_ += sizeof(int32_t);

		return _i32;
	}

	uint32_t Args::pop_ui32()
	{
		auto _ui32 = *reinterpret_cast<uint32_t*>(read_);
		read_ += sizeof(uint32_t);

		return _ui32;
	}

	std::string Args::pop_string()
	{
		auto _len = pop_ui32();

		std::string _str;
		_str.assign(read_, _len);
		read_ += _len;

		return _str;
	}

	//////////////////////////////////////////////////////////////////////////

	gsf::Args::Args()
	{
		buff_ = (char*)malloc(1024);
		size_ = 1024;
		pos_ = 0;

		read_ = write_ = tail_ = buff_;
	}

	void gsf::Args::flush()
	{
		memset(buff_, 0, pos_);
		read_ = write_ = tail_ = buff_;
		pos_ = 0;
	}

	struct ArgsPool : public utils::Singleton<ArgsPool>
	{
		using deleter_type = std::function<void(Args*)>;
		using ArgsPtr = std::unique_ptr<Args, deleter_type>;

		std::unique_ptr<Args, deleter_type> get()
		{
			std::unique_ptr<Args, deleter_type> _ptr(pool_.back().release(), [this](Args *args) {
				release_ptr(args);
			});

			pool_.pop_back();
			return _ptr;
		}

		// 服务启动的时候构建args pool，可以依据业务需求分配不同数额
		void make(uint32_t size)
		{
			for (uint32_t i = 0; i < size; ++i)
			{
				pool_.push_back(std::make_unique<Args>());
			}
		}

		// 在lua层中获取args的原始指针，需要手动释放（lua中使用智能指针会导致资源释放的相关问题
		Args * get_lptr()
		{
			return nullptr;
		}

		void release_ptr(Args *args)
		{
			args->flush();
			dirty_vec_.push_back(args);
		}

		// gsf 当前帧调用结束的时候，讲释放的args 重新放入池中。
		void reenter()
		{
			for (auto it : dirty_vec_)
			{
				pool_.push_back(std::unique_ptr<Args>(it));
			}
			dirty_vec_.clear();
		}

	private:

		std::vector<std::unique_ptr<Args>> pool_;
		std::vector<Args *> dirty_vec_;
	};

	/*! 
		auto args = make_args(11, "22");

		args->pop_i32();
		args->pop_string();

		//args容量超过1k的需要手动创建
	*/

	template <typename P0, typename ...P>
	static void pushArg(ArgsPool::ArgsPtr &args, P0 &&p0, P &&... p)
	{
		args->push(std::forward<P0>(p0));
		pushArg(args, std::forward<P>(p)...);
	}

	static void pushArg(ArgsPool::ArgsPtr &args)
	{
		//template terminate
	}

	template <typename ...P>
	ArgsPool::ArgsPtr make_args(P&& ...upvalues)
	{
		auto args = ArgsPool::get_ref().get();

		pushArg(args, std::forward<P>(upvalues)...);

		return args;
	}
}

#endif