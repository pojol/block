#ifndef _GSF_ARGS_HEALDER_
#define _GSF_ARGS_HEALDER_

#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>

#include <memory>
#include <sstream>
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

		void push_block(const char *block, int len);

		////////////////lua/////////////////////
		void push_ui32(const uint32_t &val);
		void push_i32(const int32_t &val);
		void push_string(const std::string &val);

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
		gsf::ModuleID pop_moduleid();

		std::string pop_string();

		template<typename T>
		std::list<T> pop_list();

		template <typename T>
		std::vector<T> pop_vec();

		template <typename Key, typename Value>
		std::map<Key, Value> pop_map();

		/////////////////////////////

		std::string pop_block(int beg, int end);

		uint32_t get_size() const;

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
#ifdef WATCH_PERF
			count_++;
#endif // WATCH_PERF

			std::unique_ptr<Args, deleter_type> _ptr(pool_.back().release(), [this](Args *args) {
				args->flush();
				dirty_vec_.push_back(args);
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

		void reenter()
		{
			for (auto it : dirty_vec_)
			{
				pool_.push_back(std::unique_ptr<Args>(it));
			}
			dirty_vec_.clear();
#ifdef WATCH_PERF
			if (count_ > maximun_) {
				maximun_ = count_;
				count_ = 0;
			}
#endif // WATCH_PERF
		}

#ifdef WATCH_PERF
		std::string get_perf()
		{
			std::string _perf = "event pool : single frame maximun use " + std::to_string(maximun_) + '\n';
			_perf += "event pool : size = " + std::to_string(pool_.size()) + '\n';
			maximun_ = 0;
			return _perf;
		}

		uint32_t count_ = 0;
		uint32_t maximun_ = 0;
#endif

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

	// 用于打印日志
	template <typename P0, typename ...P>
	static void pushFmt(std::ostringstream &oss, P0 &&p0, P &&... p)
	{
		oss << std::forward<P0>(p0);
		pushFmt(oss, std::forward<P>(p)...);
	}

	static void pushFmt(std::ostringstream &oss)
	{
		//template terminate
	}

	template <typename ...P>
	ArgsPtr make_log(uint32_t loglv, const char *title, P && ...upvalues)
	{
		std::ostringstream oss;
		pushFmt(oss, std::forward<P>(upvalues)...);
		return std::move(make_args(loglv, title, oss.str()));
	}
}

#endif