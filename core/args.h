#ifndef _GSF_ARGS_HEALDER_
#define _GSF_ARGS_HEALDER_

#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>

#include <memory>
#include <functional>

#include "types.h"
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
		void push(std::list<T> &list);

		template <typename T>
		void push(std::vector<T> &vec);

		template <typename Key, typename Value>
		void push(std::map<Key, Value> &map);

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

		TypeLen pop_typelen();

		gsf::SessionID pop_fd();
		gsf::MsgID pop_msgid();

		std::string pop_string();

		template<typename T>
		std::list<T> pop_list();

		template <typename T>
		std::vector<T> pop_vec();

		template <typename Key, typename Value>
		std::map<Key, Value> pop_map();

		/////////////////////////////

		void flush();
	private:
		template <typename T>
		void pop(T &val);

		void pop_impl(uint8_t &val);
		void pop_impl(int8_t &val);
		void pop_impl(uint16_t &val);
		void pop_impl(int16_t &val);
		void pop_impl(uint32_t &val);
		void pop_impl(int32_t &val);
		void pop_impl(uint64_t &val);
		void pop_impl(int64_t &val);
		void pop_impl(bool &val);
		void pop_impl(float &val);
		void pop_impl(double &val);

	private:

		char *buff_;
		char *read_;
		char *write_;
		char *tail_;

		uint32_t pos_;
		uint32_t size_;
	};



	using deleter_type = std::function<void(Args*)>;
	using ArgsPtr = std::unique_ptr<Args, deleter_type>;

	struct ArgsPool : public utils::Singleton<ArgsPool>
	{
		std::unique_ptr<Args, deleter_type> get()
		{
			std::unique_ptr<Args, deleter_type> _ptr(pool_.back().release(), [this](Args *args) {
				release_ptr(args);
			});

			pool_.pop_back();
			return _ptr;
		}

		void make(uint32_t size)
		{
			for (uint32_t i = 0; i < size; ++i)
			{
				pool_.push_back(std::make_unique<Args>());
			}
		}

 		Args * get_ptr()
		{
 			return nullptr;
  		}

		void release_ptr(Args *args)
		{
			args->flush();
			dirty_vec_.push_back(args);
		}

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

	*/

	template <typename P0, typename ...P>
	static void pushArg(ArgsPtr &args, P0 &&p0, P &&... p)
	{
		args->push(std::forward<P0>(p0));
		pushArg(args, std::forward<P>(p)...);
	}

	static void pushArg(ArgsPtr &args)
	{
		//template terminate
	}

	template <typename ...P>
	ArgsPtr make_args(P&& ...upvalues)
	{
		auto args = ArgsPool::get_ref().get();

		pushArg(args, std::forward<P>(upvalues)...);

		return args;
	}
}

#endif