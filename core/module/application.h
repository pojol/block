#ifndef _GSF_APPLICATION_HEADER_
#define _GSF_APPLICATION_HEADER_

#include "module.h"

#include <stdint.h>
#include <list>
#include <unordered_map>

#include "../../common/single.h"

namespace gsf
{
	class Application
		: public gsf::utils::Singleton<Application>
	{

	public:
		Application();

		void init_args();

		template <typename T>
		void regist_module(T *module);

		template <typename T>
		uint32_t find_module_id();

		void run();

		virtual void tick() {}

		void exit();

	protected:
		uint32_t delay_;

		//！ 临时先写在这里，未来如果支持分布式可能要放在其他地方生成，保证服务器集群唯一。
		uint32_t make_module_id();

	private:

		std::list<Module *> module_list_;
		std::unordered_map<uint32_t, uint32_t> module_id_map_;

		bool shutdown_;

		uint32_t module_id_;
	};

	template <typename T>
	void gsf::Application::regist_module(T *module)
	{
		auto _type_id = typeid(T).hash_code();
		auto _id_itr = module_id_map_.find(_type_id);
		if (_id_itr != module_id_map_.end()){
			printf("regist repeated module!\n");
			return;
		}

		module_list_.push_back(module);
		module->set_id(make_module_id());

		module_id_map_.insert(std::make_pair(_type_id, module->get_module_id()));
	}

	template <typename T>
	uint32_t gsf::Application::find_module_id()
	{
		auto _type_id = typeid(T).hash_code();
		auto _id_itr = module_id_map_.find(_type_id);
		if (_id_itr != module_id_map_.end()){
			return _id_itr->second;
		}

		return 0;
	}
}

#define AppRef gsf::Application::get_ref()

#endif
