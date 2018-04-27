
#include "coordinate.h"
#include <core/application.h>

#include <algorithm>

block::modules::CoodinatorModule::CoodinatorModule()
	: Module("CoodinatorModule")
{

}

void block::modules::CoodinatorModule::before_init()
{
	using namespace std::placeholders;

	listen(eid::distributed::coordinat_regist, std::bind(&CoodinatorModule::eCoordinatRegist, this, _1, _2));
	listen(eid::distributed::coordinat_unregit, std::bind(&CoodinatorModule::eCoordinatUnregist, this, _1, _2));

	listen(eid::distributed::coordinat_adjust_weight
		, std::bind(&CoodinatorModule::eCoordinatAdjustWeight, this, _1, _2));

	listen(eid::distributed::coordinat_select
		, std::bind(&CoodinatorModule::eCoordinatSelect, this, _1, _2));
}

void block::modules::CoodinatorModule::init()
{

}

void block::modules::CoodinatorModule::execute()
{

}

void block::modules::CoodinatorModule::shut()
{

}

//int32_t port, const std::string &module, block::ModuleID module_id, int32_t weight
void block::modules::CoodinatorModule::eCoordinatAdjustWeight(block::ModuleID target, block::ArgsPtr args)
{
	auto _nod_id = args->pop_i32();
	auto _module = args->pop_string();
	auto _characteristic = args->pop_i32();
	auto _weight = args->pop_i32();

	adjustModuleWeight(_nod_id, _module, 0, _characteristic, _weight);
}

void block::modules::CoodinatorModule::eCoordinatSelect(block::ModuleID target, block::ArgsPtr args)
{
	auto _module_name = args->pop_string();
	auto _module_characteristic = args->pop_i32();

	auto _count = nodeNameMap_.count(_module_name);
	if (_count == 0) {
		ERROR_FMTLOG("coodinator Did not find the module name:{}", _module_name);
		return ;
	}
	else {
		NodePtr _ptr;

		int32_t _light_port = 0;
		int32_t _weight = 0;
		auto _itr = nodeNameMap_.find(_module_name);
		for (size_t i = 0; i < _count; ++i, ++_itr)
		{
			auto _modules = _itr->second->modules;
			auto _ctItr = std::find_if(_modules.begin(), _modules.end(), [&](std::vector<CModuleInfo>::value_type it) {
				return (it.module_characteristic_ == _module_characteristic);
			});
			if (_ctItr == _modules.end()) {
				continue;
			}

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
		// tmp
		//callback(block::makeArgs(_ptr->nod_id, _ptr->type_, _ptr->weight_, _ptr->acceptor_ip_, _ptr->acceptor_port_));
	}
}

//const std::string &type, const std::string &ip, int32_t port
void block::modules::CoodinatorModule::eCoordinatRegist(block::ModuleID target, block::ArgsPtr args)
{
	auto _type = args->pop_string();
	auto _nod_id = args->pop_i32();

	auto itr = nodeIDMap_.find(_nod_id);
	if (itr != nodeIDMap_.end()) {
		ERROR_FMTLOG("coodinator regist app repeat! nod:{}", _nod_id);
		return;
	}

	auto _acceptor_ip = args->pop_string();
	auto _acceptor_port = args->pop_i32();

	auto nod = std::make_shared<CNodeInfo>();
	nod->nod_id = _nod_id;
	nod->type_ = _type;
	nod->acceptor_ip_ = _acceptor_ip;
	nod->acceptor_port_ = _acceptor_port;

	nodeIDMap_.insert(std::make_pair(_nod_id, nod));

	auto _count = args->pop_i32();
	for (int i = 0; i < _count; ++i)
	{
		auto _module_name = args->pop_string();
		auto _module_id = args->pop_moduleid();
		auto _module_characteristic = args->pop_i32();

		adjustModuleWeight(_nod_id, _module_name, _module_id, _module_characteristic, 0);
	}
}

void block::modules::CoodinatorModule::eCoordinatUnregist(block::ModuleID target, block::ArgsPtr args)
{
	auto _port = args->pop_i32();
}

void block::modules::CoodinatorModule::adjustModuleWeight(int32_t nod_id, const std::string &module_name, block::ModuleID module_id, int32_t characteristic, int32_t weight)
{
	auto _itr = nodeIDMap_.find(nod_id);
	if (_itr == nodeIDMap_.end()) {
		WARN_FMTLOG("coodinator adjust module weight fail, unregist! node:{}", nod_id);
		return;
	}

	if (_itr->second->weight_ + weight < 0) {
		_itr->second->weight_ = 0;
	}
	else {
		_itr->second->weight_ += weight;
	}

	auto _count = nodeNameMap_.count(module_name);
	auto _flag = false;
	if (_count > 0) {
		auto _mitr = nodeNameMap_.find(module_name);
		for (size_t i = 0; i < _count; ++i, ++_mitr)
		{
			if (_mitr->second->nod_id == nod_id) {

				auto _modules = _mitr->second->modules;
				auto _ctItr = std::find_if(_modules.begin(), _modules.end(), [&](std::vector<CModuleInfo>::value_type it) {
					return (it.module_characteristic_ == characteristic);
				});

				if (_ctItr != _modules.end()) {
					_flag = true;
					break;
				}
			}
		}
	}

	if (!_flag && module_id != 0) {
		CModuleInfo _info;
		_info.module_id_ = module_id;
		_info.module_name_ = module_name;
		_info.module_characteristic_ = characteristic;

		_itr->second->modules.push_back(_info);
		nodeNameMap_.insert(std::make_pair(module_name, _itr->second));
	}
}

