#ifndef _COORDINATE_HEADER_
#define _COORDINATE_HEADER_

#include <core/module.h>

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace block
{
	namespace modules
	{
		class CoodinatorModule
			: public block::Module
		{
		public:

			CoodinatorModule();
			virtual ~CoodinatorModule() {}

			void before_init() override;
			void init() override;
			void execute() override;
			void shut() override;

		private:
		};
	}
}


#endif // !_COORDINATE_HEADER_
