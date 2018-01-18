#include "args.h"
#include <sstream>


gsf::Args::Args()
	: bytes_(new Bytes())
{
	write_ = read_ = bytes_->buff_;
	tail_ = bytes_->buff_ + bytes_->total_;
}

gsf::Args::Args(const char *buf, size_t len)
	: bytes_(new Bytes(buf, len))
{
	write_ = read_ = bytes_->buff_;
	tail_ = bytes_->buff_ + bytes_->total_;
}

gsf::Args::~Args()
{
	params_ = 0;
	write_ = read_ = nullptr;
	tail_ = nullptr;
}

void gsf::Args::flush()
{
	bytes_->flush();

	write_ = read_ = bytes_->buff_;
	tail_ = bytes_->buff_ + bytes_->total_;

	params_ = 0;
}

void gsf::Args::inc(size_t len)
{
	if (bytes_->inc(len)) {
		write_ = bytes_->buff_ + bytes_->size_ - len;
		read_ = bytes_->buff_;
		tail_ = bytes_->buff_ + bytes_->total_;
	}
}

//////////////////////////////////////////////////////////////////////////
template <typename T>
void gsf::Args::push_impl(const T &val)
{
	size_t _size = sizeof(T);

	inc(_size);

	*reinterpret_cast<T*>(write_) = val;
	write_ += _size;
}

void gsf::Args::push_block(const char *block, int len)
{
	inc(len);

	memcpy(write_, block, len);
	write_ += len;
}

void gsf::Args::push(const uint8_t &val)
{
	push_impl(at_uint8);
	push_impl(val);
	params_++;
}

void gsf::Args::push_ui16(const uint16_t &val)
{
	push_impl(at_uint16);
	push_impl(val);
	params_++;
}

void gsf::Args::push_ui32(const uint32_t &val)
{
	push_impl(at_uint32);
	push_impl(val);
	params_++;
}

void gsf::Args::push_i32(const int32_t &val)
{
	push_impl(at_int32);
	push_impl(val);
	params_++;
}

void gsf::Args::push_i64(const int64_t &val)
{
	push_impl(at_int64);
	push_impl(val);
	params_++;
}

void gsf::Args::push_float(const float &val)
{
	push_impl(at_float);
	push_impl(val);
	params_++;
}

void gsf::Args::push_double(const double &val)
{
	push_impl(at_double);
	push_impl(val);
	params_++;
}

void gsf::Args::push_string(const std::string &val)
{
	push(val.c_str());
}

void gsf::Args::push(const int8_t &val)
{
	push_impl(at_int8);
	push_impl(val);
	params_++;
}

void gsf::Args::push(const uint16_t &val)
{
	push_impl(at_uint16);
	push_impl(val);
	params_++;
}

void gsf::Args::push(const int16_t &val)
{
	push_impl(at_int16);
	push_impl(val);
	params_++;
}

void gsf::Args::push(const uint32_t &val)
{
	push_impl(at_uint32);
	push_impl(val);
	params_++;
}

void gsf::Args::push(const int32_t & val)
{
	push_impl(at_int32);
	push_impl(val);
	params_++;
}

void gsf::Args::push(const uint64_t &val)
{
	push_impl(at_uint64);
	push_impl(val);
	params_++;
}

void gsf::Args::push(const int64_t &val)
{
	push_impl(at_int64);
	push_impl(val);
	params_++;
}

void gsf::Args::push(const bool &val)
{
	push_impl(at_bool);
	push_impl(val);
	params_++;
}

void gsf::Args::push(const float &val)
{
	push_impl(at_float);
	push_impl(val);
	params_++;
}

void gsf::Args::push(const double &val)
{
	push_impl(at_double);
	push_impl(val);
	params_++;
}

void gsf::Args::push(const std::string &val)
{
	push(val.c_str());
}

template <typename T>
void gsf::Args::push(std::list<T> &list)
{
	push_impl(at_list);
	TypeLen _len = static_cast<TypeLen>(list.size());
	push_impl(_len);

	for (auto &it : list)
	{
		push_impl(it);
	}
	size_++;
}

template <typename T>
void gsf::Args::push(std::vector<T> &vec)
{
	push_impl(at_vec);
	TypeLen _len = static_cast<TypeLen>(vec.size());
	push_impl(_len);

	for (auto &it : vec)
	{
		push_impl(it);
	}
	size_++;
}

template <typename Key, typename Value>
void gsf::Args::push(std::map<Key, Value> &map)
{
	push_impl(at_map);
	TypeLen _len = static_cast<TypeLen>(map.size());
	push_impl(_len);

	for (auto &it : map)
	{
		push_impl(it.first);
		push_impl(it.second);
	}
	size_++;
}

void gsf::Args::push(const char * val)
{
	push_impl(at_string);
	TypeLen _len = static_cast<TypeLen>(std::strlen(val));

	push_impl(_len);

	inc(_len);
	memcpy(write_, val, _len);
	write_ += _len;

	params_++;
}
//////////////////////////////////////////////////////////////////////////

uint8_t gsf::Args::pop_tag()
{
	auto _ui8 = *reinterpret_cast<uint8_t*>(read_);
	read_ += sizeof(uint8_t);

	return _ui8;
}

uint8_t gsf::Args::pop_ui8()
{
	assert(pop_tag() == at_uint8);

	auto _ui8 = *reinterpret_cast<uint8_t*>(read_);
	read_ += sizeof(uint8_t);

	params_--;

	return _ui8;
}

int8_t gsf::Args::pop_i8()
{
	assert(pop_tag() == at_int8);

	auto _val = *reinterpret_cast<int8_t*>(read_);
	read_ += sizeof(int8_t);

	params_--;

	return _val;
}

uint16_t gsf::Args::pop_ui16()
{
	assert(pop_tag() == at_uint16);

	auto _val = *reinterpret_cast<uint16_t*>(read_);
	read_ += sizeof(uint16_t);

	params_--;

	return _val;
}

int16_t gsf::Args::pop_i16()
{
	assert(pop_tag() == at_int16);

	auto _val = *reinterpret_cast<int16_t*>(read_);
	read_ += sizeof(int16_t);

	params_--;

	return _val;
}

int32_t gsf::Args::pop_i32()
{
	assert(pop_tag() == at_int32);

	auto _i32 = *reinterpret_cast<int32_t*>(read_);
	read_ += sizeof(int32_t);

	params_--;

	return _i32;
}

uint64_t gsf::Args::pop_ui64()
{
	assert(pop_tag() == at_uint64);

	auto _val = *reinterpret_cast<uint64_t*>(read_);
	read_ += sizeof(uint64_t);

	params_--;

	return _val;
}

int64_t gsf::Args::pop_i64()
{
	assert(pop_tag() == at_int64);

	auto _val = *reinterpret_cast<int64_t*>(read_);
	read_ += sizeof(int64_t);

	params_--;

	return _val;
}

bool gsf::Args::pop_bool()
{
	assert(pop_tag() == at_bool);

	auto _val = *reinterpret_cast<bool*>(read_);
	read_ += sizeof(bool);

	params_--;

	return _val;
}

float gsf::Args::pop_float()
{
	assert(pop_tag() == at_float);

	auto _val = *reinterpret_cast<float*>(read_);
	read_ += sizeof(float);

	params_--;

	return _val;
}

double gsf::Args::pop_double()
{
	assert(pop_tag() == at_double);

	auto _val = *reinterpret_cast<double*>(read_);
	read_ += sizeof(double);

	params_--;

	return _val;
}

uint32_t gsf::Args::pop_ui32()
{
	assert(pop_tag() == at_uint32);

	auto _ui32 = *reinterpret_cast<uint32_t*>(read_);
	read_ += sizeof(uint32_t);

	params_--;

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
	return pop_i32();
}

gsf::ModuleID gsf::Args::pop_moduleid()
{
	return pop_i32();
}

gsf::TimerID gsf::Args::pop_timerid()
{
	return pop_ui64();
}

std::string gsf::Args::pop_string()
{
	assert(pop_tag() == at_string);

	TypeLen _len = pop_typelen();

	std::string _str;
	_str.assign(read_, _len);
	read_ += _len;

	return _str;
}

template<typename T>
std::list<T> gsf::Args::pop_list()
{
	assert(pop_tag() == at_list);

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
	assert(pop_tag() == at_vec);

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
	assert(pop_tag() == at_map);

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

std::string gsf::Args::pop_block(int beg, int size)
{
	std::string _str;
	_str.append(bytes_->buff_ + beg, size);
	return _str;
}

//////////////////////////////////////////////////////////////////////////
uint8_t gsf::Args::peek_tag()
{
	auto _tag = *reinterpret_cast<uint8_t*>(read_);
	read_ += sizeof(uint8_t);
	return _tag;
}

void * gsf::Args::peek(uint8_t type)
{
	assert(type == peek_tag());

	void * sp = nullptr;
	sp = (void*)read_;

	if (type == at_int8) {
		read_ += sizeof(int8_t);
	}
	if (type == at_uint8) {
		read_ += sizeof(uint8_t);
	}
	if (type == at_int16) {
		read_ += sizeof(int16_t);
	}
	if (type == at_uint16) {
		read_ += sizeof(uint16_t);
	}
	if (type == at_int32) {
		read_ += sizeof(int32_t);
	}
	if (type == at_uint32) {
		read_ += sizeof(uint32_t);
	}
	if (type == at_int64) {
		read_ += sizeof(int64_t);
	}
	if (type == at_uint64) {
		read_ += sizeof(uint64_t);
	}
	if (type == at_float) {
		read_ += sizeof(float);
	}
	if (type == at_double) {
		read_ += sizeof(double);
	}
	if (type == at_bool) {
		read_ += sizeof(bool);
	}

	return sp;
}

std::pair<void*, int> gsf::Args::peek_str()
{
	void *sp = nullptr;

	assert(peek_tag() == at_string);

	TypeLen _len = pop_typelen();
	sp = (void*)read_;

	read_ += _len;

	return std::make_pair(sp, _len);
}

//////////////////////////////////////////////////////////////////////////
uint8_t gsf::Args::get_tag()
{
	auto _ui8 = *reinterpret_cast<uint8_t*>(read_);
	return _ui8;
}

uint32_t gsf::Args::get_size() const
{
	return bytes_->size_;
}

uint32_t gsf::Args::get_params_count() const
{
	return params_;
}

std::string gsf::Args::to_string() const
{
	auto _args = Args(bytes_->buff_, get_size());
	std::stringstream ss;

	auto tag = _args.get_tag();
	ss << "for testing only" << std::endl;
	while (tag != 0)
	{
		switch (tag)
		{
		case at_bool:
			ss << "bool:" << _args.pop_bool() << std::endl;
			break;
		case at_uint8:
			ss << "uint8:" << _args.pop_ui8() << std::endl;
			break;
		case at_int8:
			ss << "int8:" << _args.pop_i8() << std::endl;
			break;
		case at_int16:
			ss << "int16:" << _args.pop_i16() << std::endl;
			break;
		case at_uint16:
			ss << "uint16:" << _args.pop_ui16() << std::endl;
			break;
		case at_int32:
			ss << "int32:" << _args.pop_i32() << std::endl;
			break;
		case at_uint32:
			ss << "uint32:" << _args.pop_ui32() << std::endl;
			break;
		case at_int64:
			ss << "int64:" << _args.pop_i64() << std::endl;
			break;
		case at_uint64:
			ss << "uint64:" << _args.pop_ui64() << std::endl;
			break;
		case at_string:
			ss << "string:" << _args.pop_string() << std::endl;
			break;
		}
		tag = _args.get_tag();
	}

	return ss.str();
}

//////////////////////////////////////////////////////////////////////////

gsf::ArgsPtr gsf::log_info(const std::string &module, const std::string &content)
{
	return std::move(make_args(gsf::LogInfo, module, content));
}

gsf::ArgsPtr gsf::log_warring(const std::string &module, const std::string &content)
{
	return std::move(make_args(gsf::LogWarning, module, content));
}

gsf::ArgsPtr gsf::log_error(const std::string &module, const std::string &content)
{
	return make_args(gsf::LogErr, module, content);
}