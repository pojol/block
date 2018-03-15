#ifndef _GSF_SERVEICE_HEADER_
#define _GSF_SERVEICE_HEADER_

namespace gsf
{
	enum ServiceState
	{
		ss_beforeInit,
		ss_init,
		ss_execute,
		ss_shut,
		ss_afterShut
	};

	class Service
	{
	protected:

		/*!
			
		**/
		virtual void exec();

	private:
		ServiceState ss_;
	};
}

#endif