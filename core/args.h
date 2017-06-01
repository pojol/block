#ifndef _GSF_VARS_HEADER_
#define _GSF_VARS_HEADER_

#include "variant.h"
#include <vector>
#include <assert.h>
#include <memory>

#include "block.h"

namespace gsf
{
	class Args;

	typedef std::function<void(uint32_t, uint32_t, std::string)> RemoteFunc;
	typedef std::function<void(uint32_t, const char*, const gsf::Args&)> LogFunc;

	class Arg
	{
		typedef Variant<bool, uint32_t, int32_t, uint64_t, int64_t, std::string, RemoteFunc, LogFunc> av;
	public:
		void set_bool(const bool var)
		{
			v_ = bool(var);
			idx_ = 0;
		}

		void set_uint32(const uint32_t var)
		{
			v_ = uint32_t(var);
			idx_ = 1;
		}

		void set_int32(const int32_t var)
		{
			v_ = int32_t(var);
			idx_ = 2;
		}

		void set_uint64(const uint64_t var)
		{
			v_ = uint64_t(var);
			idx_ = 3;
		}

		void set_int64(const int64_t var)
		{
			v_ = int64_t(var);
			idx_ = 4;
		}

		void set_string(const std::string &var)
		{
			v_ = std::string(var);
			idx_ = 5;
		}

		void set_remote_callback(RemoteFunc var)
		{
			v_ = var;
			idx_ = 6;
		}

		void set_log_callback(LogFunc var)
		{
			v_ = var;
			idx_ = 7;
		}

		av v_;
		uint32_t idx_;
	};

	class Args
	{
	public:
		Args()
			: size_(0)
		{

		}

		template <typename T1>
		Args(T1 arg1)
			: size_(0)
		{	
			add(arg1);
		}

		template <typename T1, typename T2>
		Args(T1 arg1, T2 arg2)
			: size_(0)
		{
			add(arg1);
			add(arg2);
		}

		template <typename T1, typename T2, typename T3>
		Args(T1 arg1, T2 arg2, T3 arg3)
			: size_(0)
		{
			add(arg1);
			add(arg2);
			add(arg3);
		}

		template <typename T1, typename T2, typename T3, typename T4>
		Args(T1 arg1, T2 arg2, T3 arg3, T4 arg4)
			: size_(0)
		{
			add(arg1);
			add(arg2);
			add(arg3);
			add(arg4);
		}

		template <typename T1, typename T2, typename T3, typename T4, typename T5>
		Args(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
			: size_(0)
		{
			add(arg1);
			add(arg2);
			add(arg3);
			add(arg4);
			add(arg5);
		}

		void add(const bool value)
		{
			auto _arg = std::make_shared<Arg>();
			_arg->set_bool(value);

			size_++;
			arg_list_.push_back(_arg);
		}

		void add(const uint32_t value)
		{
			auto _arg = std::make_shared<Arg>();
			_arg->set_uint32(value);

			size_++;
			arg_list_.push_back(_arg);
		}

		void add(const int32_t value)
		{
			auto _arg = std::make_shared<Arg>();
			_arg->set_int32(value);

			size_++;
			arg_list_.push_back(_arg);
		}

		void add(const uint64_t value)
		{
			auto _arg = std::make_shared<Arg>();
			_arg->set_uint64(value);

			size_++;
			arg_list_.push_back(_arg);
		}

		void add(const int64_t value)
		{
			auto _arg = std::make_shared<Arg>();
			_arg->set_int64(value);

			size_++;
			arg_list_.push_back(_arg);
		}

		void add(const char *str)
		{
			auto _arg = std::make_shared<Arg>();
			_arg->set_string(std::string(str));
			
			size_++;
			arg_list_.push_back(_arg);
		}

		void add(const std::string &value)
		{
			auto _arg = std::make_shared<Arg>();
			_arg->set_string(value);

			size_++;
			arg_list_.push_back(_arg);
		}

		void add(RemoteFunc func)
		{
			auto _arg = std::make_shared<Arg>();
			_arg->set_remote_callback(func);

			size_++;
			arg_list_.push_back(_arg);
		}

		void add(LogFunc func)
		{
			auto _arg = std::make_shared<Arg>();
			_arg->set_log_callback(func);

			size_++;
			arg_list_.push_back(_arg);
		}

		const bool pop_bool(const int index) const
		{
			#ifdef _DEBUG
				assert(index >= 0 && index < size_);
			#else
				if (index < 0 || index >= size_){
					return false;
				}
			#endif

			auto var = arg_list_[index];
			return var->v_.Get<bool>();
		}

		const int32_t pop_int32(const int index) const
		{
			#ifdef _DEBUG
				assert(index >= 0 && index < size_);
			#else
				if (index < 0 || index >= size_){
					return 0;
				}
			#endif

			auto var = arg_list_[index];
			return var->v_.Get<int32_t>();
		}

		const uint32_t pop_uint32(const int index) const
		{
			#ifdef _DEBUG
				assert(index >= 0 && index < size_);
			#else
				if (index < 0 || index >= size_){
					return 0;
				}
			#endif

			auto var = arg_list_[index];
			return var->v_.Get<uint32_t>();
		}

		const uint64_t pop_uint64(const int index) const
		{
			#ifdef _DEBUG
				assert(index >= 0 && index < size_);
			#else
				if (index < 0 || index >= size_){
					return 0;
				}
			#endif

			auto var = arg_list_[index];
			return var->v_.Get<uint64_t>(); 
		}

		const uint64_t pop_int64(const int index) const
		{
			#ifdef _DEBUG
				assert(index >= 0 && index < size_);
			#else
				if (index < 0 || index >= size_){
					return 0;
				}
			#endif

			auto var = arg_list_[index];
			return var->v_.Get<int64_t>(); 
		}

		const std::string & pop_string(const int index) const
		{
			#ifdef _DEBUG
				assert(index >= 0 && index < size_);
			#else
				if (index < 0 || index >= size_){
					return "";
				}
			#endif

			auto var = arg_list_[index];
			return var->v_.Get<std::string>();
		}

		const RemoteFunc pop_remote_callback(const int index) const
		{
#ifdef _DEBUG
			assert(index >= 0 && index < size_);
#else
			if (index < 0 || index >= size_) {
				return nullptr;
			}
#endif
			auto var = arg_list_[index];
			return var->v_.Get<RemoteFunc>();
		}

		const LogFunc pop_log_callback(const int index) const
		{
#ifdef _DEBUG
			assert(index >= 0 && index < size_);
#else
			if (index < 0 || index >= size_) {
				return nullptr;
			}
#endif
			auto var = arg_list_[index];
			return var->v_.Get<LogFunc>();
		}

		uint32_t get_typeid(const int index) const
		{
			auto var = arg_list_[index];
			return var->idx_;
		}

		Args & operator << (const bool value)
		{
			add(value);
			return *this;
		}

		Args & operator << (const uint32_t value)
		{
			add(value);
			return *this;
		}

		Args & operator << (const int32_t value)
		{
			add(value);
			return *this;
		}

		Args & operator << (const uint64_t value)
		{
			add(value);
			return *this;
		}

		Args & operator << (const int64_t value)
		{
			add(value);
			return *this;
		}

		Args & operator << (const std::string &value)
		{
			add(value);
			return *this;
		}

		Args & operator << (RemoteFunc value)
		{
			add(value);
			return *this;
		}

		void push_uint32(const uint32_t value)
		{
			add(value);
		}

		void push_string(const std::string &value)
		{
			add(value);
		}

		void push_remote_callback(RemoteFunc value)
		{
			add(value);
		}

		void push_log_callback(LogFunc value)
		{
			add(value);
		}

		int get_count() const
		{
			return size_;
		}

	private:
		std::vector<std::shared_ptr<Arg>> arg_list_;

		int size_;
	};

	/*
		Args args;
		args.add("hello");

		std::cout << args.pop_string(0).c_str() << std::endl;
		
		Args args;
		args << 10 << "hello";

		std::cout << args.pop_uint32(0) << args.pop_string(1).c_str() << std::endl;
	*/
}


#endif
