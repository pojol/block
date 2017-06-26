#ifndef _GSF_DISTRIBUTED_NODE_HEADER_
#define _GSF_DISTRIBUTED_NODE_HEADER_

#include <core/module.h>
#include <core/event.h>

namespace gsf
{
	namespace distributed
	{

		class NodeModule
			: public gsf::Module
			, public gsf::IEvent
		{
		public:

			NodeModule();
			virtual ~NodeModule();

			void init() override;
			void execute() override;
			void shut() override;

			

		private:

		};

	}
}


#endif