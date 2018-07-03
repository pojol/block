#pragma once

#include <core/module.h>

#include <vector>
#include <map>
#include <tuple>
#include <functional>

namespace block
{
	namespace modules
	{
		class NodeModule
			: public block::Module
		{
		public:

			NodeModule();
			virtual ~NodeModule();

			void before_init() override;
			void init() override;
			void execute() override;
			void shut() override;

		protected:

		};

	}
}
