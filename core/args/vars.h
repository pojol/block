enum AType
{
	AT_UINT,
	AT_STRING,
};

class Arg
{
	typedef Variant<bool, uint32_t, int32_t, uint64_t, int64_t, std::string> av;
public:
	void set_uint32(const uint32_t var)
	{
		type_ = AT_UINT;
		v_ = uint32_t(var);
	}

	void set_string(const std::string &var)
	{
		type_ = AT_STRING;
		v_ = std::string(var);
	}

	AType type_;
	av v_;
};

class Args
{
public:
	Args()
		: size_(0)
	{

	}

	void add(const std::string &value)
	{
		auto _arg = std::make_shared<Arg>();
		_arg->set_string(value);

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

	const uint32_t pop_uint32(const int index)
	{
		auto var = arg_list_[index];

		return var->v_.Get<uint32_t>();
	}

	const std::string & pop_string(const int index)
	{

		// 检查index

		auto var = arg_list_[index];

		// 检查类型

		return var->v_.Get<std::string>();
	}

	Args & operator << (const uint32_t value)
	{
		add(value);
		return *this;
	}

	Args & operator << (const std::string &value)
	{
		add(value);
		return *this;
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
