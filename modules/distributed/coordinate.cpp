
#include "coordinate.h"

gsf::modules::CoodinatorModule::CoodinatorModule()
	: Module("CoodinatorModule")
{

}

void gsf::modules::CoodinatorModule::before_init()
{
	using namespace std::placeholders;

	listen(this, eid::distributed::coordinat_regist, std::bind(&CoodinatorModule::event_regist, this, _1, _2));
	listen(this, eid::distributed::coordinat_unregit, std::bind(&CoodinatorModule::event_unregist, this, _1, _2));

	listen(this, eid::distributed::coordinat_adjust_weight
		, std::bind(&CoodinatorModule::event_adjust_module_weight, this, _1, _2));

	listen(this, eid::distributed::coordinat_get
		, std::bind(&CoodinatorModule::event_get_light_module, this, _1, _2));
}

//int32_t port, const std::string &module, gsf::ModuleID module_id, int32_t weight
void gsf::modules::CoodinatorModule::event_adjust_module_weight(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
{
	auto _nod_id = args->pop_i32();
	auto _module = args->pop_string();
	auto _weight = args->pop_i32();

	adjust_module_weight(_nod_id, _module, 0, _weight);
}

void gsf::modules::CoodinatorModule::event_get_light_module(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
{
	auto _module_name = args->pop_string();

	auto _count = node_name_map_.count(_module_name);
	if (_count == 0) {
		return;
	}
	else {
		NodePtr _ptr;

		int32_t _light_port = 0;
		int32_t _weight = 0;
		auto _itr = node_name_map_.find(_module_name);
		for (size_t i = 0; i < _count; ++i, ++_itr)
		{
			if (!_ptr) {
				_ptr = _itr->second;
			}
			else {

				if (_itr->second->weight_ < _ptr->weight_) {
					_ptr = _itr->second;
				}

			}
		}

		assert(_ptr->nod_id != 0);
		callback(gsf::make_args(_ptr->nod_id));
	}
}

//const std::string &type, const std::string &ip, int32_t port
void gsf::modules::CoodinatorModule::event_regist(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
{
	auto _type = args->pop_string();
	auto _ip = args->pop_string();
	auto _nod_id = args->pop_i32();

	auto itr = node_id_map_.find(_nod_id);
	if (itr != node_id_map_.end()) {
		printf("coordnate regist app repeat! %d \n", _nod_id);
		return;
	}

	auto nod = std::make_shared<NodeInfo>();
	nod->ip_ = _ip;
	nod->nod_id = _nod_id;
	nod->type_ = _type;

	node_id_map_.insert(std::make_pair(_nod_id, nod));

	auto _count = args->pop_i32();
	for (int i = 0; i < _count; ++i)
	{
		auto _module_name = args->pop_string();
		auto _module_id = args->pop_moduleid();

		adjust_module_weight(_nod_id, _module_name, _module_id, 0);
	}

	callback(nullptr);
}

void gsf::modules::CoodinatorModule::event_unregist(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
{
	auto _port = args->pop_i32();
}

void gsf::modules::CoodinatorModule::adjust_module_weight(int32_t nod_id, const std::string &module_name, gsf::ModuleID module_id, int32_t weight)
{
	auto _itr = node_id_map_.find(nod_id);
	if (_itr == node_id_map_.end()) {
		printf("adjust module weight fail, unregist! \n");
		return;
	}

	if (_itr->second->weight_ + weight < 0) {
		_itr->second->weight_ = 0;
	}
	else {
		_itr->second->weight_ += weight;
	}

	auto _count = node_name_map_.count(module_name);
	auto _flag = false;
	if (_count > 0) {
		auto _mitr = node_name_map_.find(module_name);
		for (size_t i = 0; i < _count; ++i, ++_mitr)
		{
			if (_mitr->second->nod_id == nod_id) {
				_flag = true;
				break;
			}
		}
	}

	if (!_flag && module_id != 0) {
		_itr->second->modules.push_back(std::make_pair(module_id, module_name));
		node_name_map_.insert(std::make_pair(module_name, _itr->second));
	}
}

