
#include "coordinate.h"

gsf::modules::Coodinator::Coodinator()
	: Module("Coodinator")
{

}

void gsf::modules::Coodinator::before_init()
{
	using namespace std::placeholders;

	listen(this, eid::distributed::coordinat_regist, std::bind(&Coodinator::event_regist, this, _1, _2));
	listen(this, eid::distributed::coordinat_unregit, std::bind(&Coodinator::event_unregist, this, _1, _2));

	listen(this, eid::distributed::coordinat_adjust_weight
		, std::bind(&Coodinator::event_adjust_module_weight, this, _1, _2));

	listen(this, eid::distributed::coordinat_get
		, std::bind(&Coodinator::event_get_light_module, this, _1, _2));
}

//int32_t port, const std::string &module, gsf::ModuleID module_id, int32_t weight
void gsf::modules::Coodinator::event_adjust_module_weight(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
{
	auto _port = args->pop_i32();
	auto _module = args->pop_string();
	auto _module_id = args->pop_moduleid();
	auto _weight = args->pop_i32();

	auto _itr = node_id_map_.find(_port);
	if (_itr == node_id_map_.end()) {
		printf("adjust module weight fail, unregist! \n");
		return;
	}

	if (_itr->second->weight_ + _weight < 0) {
		_itr->second->weight_ = 0;
	}
	else {
		_itr->second->weight_ += _weight;
	}

	auto _count = node_name_map_.count(_module);
	auto _flag = false;
	if (_count > 0) {
		auto _mitr = node_name_map_.find(_module);
		for (size_t i = 0; i < _count; ++i, ++_mitr)
		{
			if (_mitr->second->port_ == _port) {
				_flag = true;
				break;
			}
		}
	}

	if (!_flag) {
		_itr->second->modules.push_back(std::make_pair(_module_id, _module));
		node_name_map_.insert(std::make_pair(_module, _itr->second));
	}
}

void gsf::modules::Coodinator::event_get_light_module(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
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

		assert(_ptr->port_ != 0);
		callback(gsf::make_args(_ptr->port_));
	}
}

//const std::string &type, const std::string &ip, int32_t port
void gsf::modules::Coodinator::event_regist(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
{
	auto _type = args->pop_string();
	auto _ip = args->pop_string();
	auto _port = args->pop_i32();

	auto itr = node_id_map_.find(_port);
	if (itr != node_id_map_.end()) {
		printf("coordnate regist app repeat! %d \n", _port);
		return;
	}

	auto nod = std::make_shared<NodeInfo>();
	nod->ip_ = _ip;
	nod->port_ = _port;
	nod->type_ = _type;

	node_id_map_.insert(std::make_pair(_port, nod));
}

void gsf::modules::Coodinator::event_unregist(const gsf::ArgsPtr &args, gsf::CallbackFunc callback)
{
	auto _port = args->pop_i32();
}

