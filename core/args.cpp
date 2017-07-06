#include "args.h"

template <typename T>
void gsf::Args::push_impl(const T &val)
{
	size_t _size = sizeof(T);
	*reinterpret_cast<T*>(write_) = val;
	write_ += _size;

	pos_ += _size;
}

void gsf::Args::push(const uint8_t &val)
{
	push_impl(val);
}

void gsf::Args::push(const int8_t &val)
{
	push_impl(val);
}

void gsf::Args::push(const uint16_t &val)
{
	push_impl(val);
}

void gsf::Args::push(const int16_t &val)
{
	push_impl(val);
}

void gsf::Args::push(const uint32_t &val)
{
	push_impl(val);
}

void gsf::Args::push(const int32_t & val)
{
	push_impl(val);
}

void gsf::Args::push(const uint64_t &val)
{
	push_impl(val);
}

void gsf::Args::push(const int64_t &val)
{
	push_impl(val);
}

void gsf::Args::push(const bool &val)
{
	push_impl(val);
}

void gsf::Args::push(const float &val)
{
	push_impl(val);
}

void gsf::Args::push(const double &val)
{
	push_impl(val);
}

void gsf::Args::push(const std::string &val)
{
	push(val.c_str());
}

template <typename T>
void gsf::Args::push(std::list<T> &list)
{
	TypeLen _len = static_cast<TypeLen>(list.size());
	push_impl(_len);

	for (auto &it : list)
	{
		push_impl(it);
	}
}

template <typename T>
void gsf::Args::push(std::vector<T> &vec)
{
	TypeLen _len = static_cast<TypeLen>(vec.size());
	push_impl(_len);

	for (auto &it : vec)
	{
		push_impl(it);
	}
}

template <typename Key, typename Value>
void gsf::Args::push(std::map<Key, Value> &map)
{
	TypeLen _len = static_cast<TypeLen>(map.size());
	push_impl(_len);

	for (auto &it : map)
	{
		push_impl(it.first);
		push_impl(it.second);
	}
}

void gsf::Args::push(const char * val)
{
	TypeLen _len = static_cast<TypeLen>(std::strlen(val));

	push_impl(_len);

	memcpy(write_, val, _len);
	write_ += _len;

	pos_ += _len;
}

//////////////////////////////////////////////////////////////////////////
uint8_t gsf::Args::pop_ui8()
{
	auto _ui8 = *reinterpret_cast<uint8_t*>(read_);
	read_ += sizeof(uint8_t);

	return _ui8;
}

int8_t gsf::Args::pop_i8()
{
	auto _val = *reinterpret_cast<int8_t*>(read_);
	read_ += sizeof(int8_t);

	return _val;
}

uint16_t gsf::Args::pop_ui16()
{
	auto _val = *reinterpret_cast<uint16_t*>(read_);
	read_ += sizeof(uint16_t);

	return _val;
}

int16_t gsf::Args::pop_i16()
{
	auto _val = *reinterpret_cast<int16_t*>(read_);
	read_ += sizeof(int16_t);

	return _val;
}

int32_t gsf::Args::pop_i32()
{
	auto _i32 = *reinterpret_cast<int32_t*>(read_);
	read_ += sizeof(int32_t);

	return _i32;
}

uint64_t gsf::Args::pop_ui64()
{
	auto _val = *reinterpret_cast<uint64_t*>(read_);
	read_ += sizeof(uint64_t);

	return _val;
}

int64_t gsf::Args::pop_i64()
{
	auto _val = *reinterpret_cast<int64_t*>(read_);
	read_ += sizeof(int64_t);

	return _val;
}

bool gsf::Args::pop_bool()
{
	auto _val = *reinterpret_cast<bool*>(read_);
	read_ += sizeof(bool);

	return _val;
}

float gsf::Args::pop_float()
{
	auto _val = *reinterpret_cast<float*>(read_);
	read_ += sizeof(float);

	return _val;
}

double gsf::Args::pop_double()
{
	auto _val = *reinterpret_cast<double*>(read_);
	read_ += sizeof(double);

	return _val;
}

uint32_t gsf::Args::pop_ui32()
{
	auto _ui32 = *reinterpret_cast<uint32_t*>(read_);
	read_ += sizeof(uint32_t);

	return _ui32;
}

template <typename T>
void gsf::Args::pop(T &val)
{
	pop_impl(val);
}

void gsf::Args::pop_impl(uint8_t &val)
{
	val = pop_ui8();
}

void gsf::Args::pop_impl(int8_t &val)
{
	val = pop_i8();
}

void gsf::Args::pop_impl(uint16_t &val)
{
	val = pop_ui16();
}

void gsf::Args::pop_impl(int16_t &val)
{
	val = pop_i16();
}

void gsf::Args::pop_impl(uint32_t &val)
{
	val = pop_ui32();
}

void gsf::Args::pop_impl(int32_t &val)
{
	val = pop_i32();
}

void gsf::Args::pop_impl(uint64_t &val)
{
	val = pop_ui64();
}

void gsf::Args::pop_impl(int64_t &val)
{
	val = pop_i64();
}

void gsf::Args::pop_impl(bool &val)
{
	val = pop_bool();
}

void gsf::Args::pop_impl(float &val)
{
	val = pop_float();
}

void gsf::Args::pop_impl(double &val)
{
	val = pop_double();
}

gsf::Args::TypeLen gsf::Args::pop_typelen()
{
	TypeLen _len = *reinterpret_cast<TypeLen*>(read_);
	read_ += sizeof(TypeLen);
	return _len;
}

gsf::SessionID gsf::Args::pop_fd()
{
	return pop_ui16();
}

gsf::MsgID gsf::Args::pop_msgid()
{
	return pop_ui16();
}

std::string gsf::Args::pop_string()
{
	TypeLen _len = pop_typelen();

	std::string _str;
	_str.assign(read_, _len);
	read_ += _len;

	return _str;
}

template<typename T>
std::list<T> gsf::Args::pop_list()
{
	TypeLen _len = pop_typelen();

	std::list<T> _list;
	for (TypeLen i = 0; i < _len; ++i)
	{
		T _t;
		pop(_t);
		_list.push_back(_t);
	}

	return _list;
}

template <typename T>
std::vector<T> gsf::Args::pop_vec()
{
	TypeLen _len = pop_typelen();

	std::vector<T> _vec;
	for (TypeLen i = 0; i < _len; ++i)
	{
		T _t;
		pop(_t);
		_vec.push_back(_t);
	}

	return _vec;
}

template <typename Key, typename Value>
std::map<Key, Value> gsf::Args::pop_map()
{
	TypeLen _len = pop_typelen();

	std::map<Key, Value> _map;
	for (TypeLen i = 0; i < _len; ++i)
	{
		Key _k;
		Value _v;
		pop(_k);
		pop(_v);

		_map.insert(std::make_pair(_k, _v));
	}

	return _map;
}

//////////////////////////////////////////////////////////////////////////

gsf::Args::Args()
{
	buff_ = (char*)malloc(1024);
	size_ = 1024;
	pos_ = 0;

	read_ = write_ = tail_ = buff_;
}

gsf::Args::Args(int size)
{
	buff_ = (char*)malloc(size);
	size_ = size;
	pos_ = 0;

	read_ = write_ = tail_ = buff_;
}

void gsf::Args::flush()
{
	memset(buff_, 0, pos_);
	read_ = write_ = tail_ = buff_;
	pos_ = 0;
}


//////////////////////////////////////////////////////////////////////////


