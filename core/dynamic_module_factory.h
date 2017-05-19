
#ifndef _DYNAMIC_FACTORY_HEADER_
#define _DYNAMIC_FACTORY_HEADER_

#include <functional>
#include <map>

// 如非必要尽量使用静态显示构建Module

typedef std::function<void *()> create_ins;

class DynamicModuleFactory
{
public:
	static void * create(const std::string &name)
	{
		auto itr = class_map_.find(name);
		if (itr == class_map_.end()) { return nullptr; }

		return itr->second();
	}

	static void regist(const std::string &name, create_ins func)
	{
		auto itr = class_map_.find(name);

		if (itr == class_map_.end()) {
			class_map_.insert(std::make_pair(name, func));
		}
	}

private:
	static std::map<std::string, create_ins> class_map_;
};

// 避免对静态成员的重复定义
#ifdef WIN32
__declspec(selectany) std::map<std::string, create_ins> DynamicModuleFactory::class_map_;
#else
__attribute((weak)) std::map<std::string, create_ins> DynamicModuleFactory::class_map_;
#endif

struct DynamicRegister
{
	DynamicRegister(const std::string &name, create_ins func)
	{
		DynamicModuleFactory::regist(name, func);
	}
};

#define REGISTER_CLASS(class_name)		\
class class_name##DynamicRegister {		\
	public:								\
		static void * new_instance()	\
		{								\
			return new class_name;		\
		}								\
	private:							\
		static DynamicRegister reg_;	\
};										\
DynamicRegister class_name##DynamicRegister::reg_(#class_name, class_name##DynamicRegister::new_instance);


#endif // !_DYNAMIC_FACTORY_HEADER_