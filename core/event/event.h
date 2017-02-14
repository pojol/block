//
// Created by pojol on 2017/2/13.
//

#ifndef _GSF_EVENT_HEADER_
#define _GSF_EVENT_HEADER_

#include "../../common/single.h"
#include "../../common/any.h"

#include "../module/module.h"
#include "event_handler.h"

#include <functional>
#include <tuple>
#include <unordered_map>

namespace gsf
{
    namespace core
    {
		typedef std::function<void(uint32_t, std::tuple<gsf::utils::Any>)> EventFunc;

		class Door
		{
		public:
			virtual void listen(Door *self, EventFunc func){}

			virtual void listen(uint32_t event, EventFunc func) {}

			virtual void dispatch(uint32_t event, std::tuple<gsf::utils::Any> args, EventHandlerPtr callback = nullptr){}

        protected:

		};

		class EventModule
				: public Module
		{
		public:


		protected:
			void execute();

        private:

		};
    }
}

#endif