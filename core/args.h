#ifndef _GSF_ARGS_HEALDER_
#define _GSF_ARGS_HEALDER_

#include <string>
#include <cstring>
#include <vector>
#include <set>
#include <map>
#include <list>

#include <memory>
#include <sstream>
#include <functional>

#include "../depend/types.h"
#include "../utils/single.h"

namespace block
{
	class out_of_bound : public std::exception
	{
	public:
		out_of_bound()
			: err_("Out of bound")
		{}
		out_of_bound(const char *desc)
		{
			err_ = desc;
		}

		~out_of_bound() throw()
		{

		}

		virtual const char * what() const throw()
		{
			return err_.c_str();
		}

	private:
		std::string err_;
	};

	struct Bytes
	{
		Bytes(size_t total = 64)
			: total_(total)
			, size_(0)
		{
			assert(total != 0);
			assert(total < 1024 * 1024 * 100);

			buff_ = static_cast<char*>(calloc(1, total));
		}

		Bytes(const char *buf, size_t len)
			: total_(64)
		{
			assert(len != 0);
			assert(len < 1024 * 1024 * 100);

			if (total_ <= len) {
				size_t _double_size = total_;
				while (_double_size < len) {
					_double_size <<= 1;
				}
				total_ = _double_size;
			}

			buff_ = static_cast<char*>(calloc(1, total_));
			memcpy(buff_, buf, len);
		}

		virtual ~Bytes()
		{
			if (buff_) {
				free(buff_);
				buff_ = nullptr;
			}
		}

		void flush()
		{
			memset(buff_, 0, size_);
			size_ = 0;
		}

		bool inc(size_t len)
		{
			size_t _tmp = size_ + len;
			if (_tmp < size_) {
				throw out_of_bound();
			}

			if (_tmp <= total_) {
				size_ = _tmp;
				return false;
			}

			size_t _double_size = total_;
			while (_double_size < _tmp) {
				_double_size <<= 1;
			}

			char *_tmp_buf = static_cast<char *>(calloc(1, _double_size));
			memcpy(_tmp_buf, buff_, size_);
			free(buff_);

			buff_ = _tmp_buf;
			size_ = _tmp;
			total_ = _double_size;

			return true;
		}

		char *buff_;
		size_t size_;
		size_t total_;
	};

	typedef std::shared_ptr<Bytes> BytesPtr;

	/*
	int          string          list
	+---+-------+---+---+-------+---+---+-------+-----------+ small cache = 128
	|tag|content|tag|len|content|tag|len|content|000000000... resize cache = original * 2 [256, 512 ...]
	+---+-------+---+---+-------+---+---+-------+-----------+
	*/

	struct Args
	{
		using TypeLen = uint16_t;
		using TypeTag = uint8_t;

		Args();
		Args(const char *buf, size_t len);
		~Args();

		//////////////////////////////// push //////////////////////////////////////
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
		void push(const char *val, size_t len);
		void push(const std::string &val);

		template <typename T>
		void push_impl(const T &val);

		template <typename T>
		void push(std::list<T> &list);

		template <typename T>
		void push(std::vector<T> &vec);

		template <typename Key, typename Value>
		void push(std::map<Key, Value> &map);

		//void push_block(const char *block, int len);

		// lua
		void push_ui16(const uint16_t &val);
		void push_ui32(const uint32_t &val);
		void push_i32(const int32_t &val);
		void push_i64(const int64_t &val);
		void push_float(const float &val);
		void push_double(const double &val);
		void push_string(const std::string &val);
		void push_bool(const bool &val);

		///////////////////////////////// pop //////////////////////////////////
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

		TypeLen pop_typelen();

		std::string pop_string();

		template<typename T>
		std::list<T> pop_list();

		template <typename T>
		std::vector<T> pop_vec();

		template <typename Key, typename Value>
		std::map<Key, Value> pop_map();

		//std::string pop_block();

		template <typename T>
		void pop(T &val);

		// lua
		uint8_t pop_tag();
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

		block::SessionID pop_fd();
		block::MsgID pop_msgid();
		block::ModuleID pop_moduleid();
		block::TimerID pop_timerid();

		/////////////////////////////////get//////////////////////////////////////
		int get_tag();
		uint32_t get_size() const;
		uint32_t get_params_count() const;
		uint32_t get_offset() const;

		std::string exportBuf();
		void importBuf(const std::string &str);

		//! debug
		std::string print() const;
		///////////////////////////////peek///////////////////////////////////////
		uint8_t peek_tag();
		void * peek(uint8_t type);
		std::pair<void*, int> peek_str();

		//////////////////////////////////////////////////////////////////////////

		//! private
		void flush();

		void inc(size_t len);

	private:

		BytesPtr bytes_ = nullptr;
		char *write_ = nullptr;
		char *read_ = nullptr;

		const char *tail_ = nullptr;
		size_t params_ = 0;
		uint32_t offset_ = 0;
	};

	using deleter_type = std::function<void(Args*)>;
	using ArgsPtr = std::unique_ptr<Args, deleter_type>;

	struct ArgsPool : public block::utils::Singleton<ArgsPool>
	{
		std::unique_ptr<Args, deleter_type> get()
		{
#ifdef WATCH_PERF
			count_++;
#endif // WATCH_PERF
			pool_use_++;
			resize();

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

			pool_size_ += size;
		}

		void reenter()
		{
			for (auto it : dirty_vec_)
			{
				pool_.push_back(std::unique_ptr<Args>(it));
				pool_use_--;
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

	protected:
		//! 当池子不够的时候申请更多的空间
		void resize()
		{
			size_t _expanSize = static_cast<size_t>(pool_size_ * 0.7);
			if (pool_use_ > _expanSize) {
				make(pool_size_ * 2);
			}
		}

	private:
		std::vector<std::unique_ptr<Args>> pool_;
		int32_t pool_size_ = 0;
		uint32_t pool_use_ = 0;

		std::vector<Args *> dirty_vec_;
	};


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
	ArgsPtr makeArgs(P&& ...upvalues)
	{
		auto args = ArgsPool::get_ref().get();

		pushArg(args, std::forward<P>(upvalues)...);

		return args;
	}
}

#endif