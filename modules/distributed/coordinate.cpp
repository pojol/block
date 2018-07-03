
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
	/*
	listen(block::event::coordinat_regist, std::bind(&CoodinatorModule::eCoordinatRegist, this, _1, _2));
	listen(block::event::coordinat_unregit, std::bind(&CoodinatorModule::eCoordinatUnregist, this, _1, _2));

	listen(block::event::coordinat_adjust_weight
		, std::bind(&CoodinatorModule::eCoordinatAdjustWeight, this, _1, _2));

	listen(block::event::coordinat_select
		, std::bind(&CoodinatorModule::eCoordinatSelect, this, _1, _2));
		*/
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
