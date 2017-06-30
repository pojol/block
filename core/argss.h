#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>

struct Args
{
	using TypeLen = uint16_t;

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


struct ArgsPool
{
	using deleter_type = std::function<void(Args*)>;
	using ArgsPtr = std::unique_ptr<Args, deleter_type>;

	void make(uint32_t size)
	{
		for (uint32_t i = 0; i < size; ++i)
		{
			pool_.push_back(std::make_unique<Args>());
		}
	}

	std::unique_ptr<Args, deleter_type> get()
	{
		std::unique_ptr<Args, deleter_type> _ptr(pool_.back().release(), [this](Args *args) {
			pool_.push_back(std::unique_ptr<Args>(args));
		});

		pool_.pop_back();
		return _ptr;
	}


	////lua
	Args * get_ptr()
	{

	}

	void reenter(Args *args)
	{

	}

private:

	std::vector<std::unique_ptr<Args>> pool_;

};

/*
template <typename ...T>
ArgsPool::ArgsPtr make_args(T... args)
{
	
}
*/