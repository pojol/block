#ifndef _ARGSS_HEALDER_
#define _ARGSS_HEALDER_

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

		std::string & pop_string();

		template<typename T>
		std::list<T> pop_list();

		template <typename T>
		std::vector<T> pop_vec();

		template <typename Key, typename Value>
		std::map<Key, Value> pop_map();

		/////////////////////////////

		void flush();


	private:

		char *buff_;
		char *read_;
		char *write_;
		char *tail_;

		uint32_t pos_;
	};

	gsf::Args::Args()
	{
		buff_ = (char*)malloc(ARGS_MAX_SIZE);

		read_ = write_ = tail_ = buff_;
		pos_ = 0;
	}

	void gsf::Args::push(const int32_t & val)
	{
	}

	void gsf::Args::push(const char * val)
	{
	}

	void gsf::Args::flush()
	{

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

	private:
		void make(uint32_t size)
		{
			for (uint32_t i = 0; i < size; ++i)
			{
				pool_.push_back(std::make_unique<Args>());
			}
		}

		////lua
		Args * get_ptr()
		{

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

	/*!  args容量超过1k的需要手动创建
		auto args = make_args(11, "22");

		args->pop_i32();
		args->pop_string();
	*/

	template <typename T>
	void unpack(ArgsPool::ArgsPtr args, const T &val)
	{
		args->push(val);
	}

	template <typename T, typename ...A>
	ArgsPool::ArgsPtr make_args(const T &t, A ...alist)
	{
		auto args = ArgsPool::get_ref().get();

		args->push(t);
		unpack(std::move(args), alist...);

		return args;
	}
}

#endif