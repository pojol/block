#ifndef _GSF_SESSION_QUEUE_HEADER_
#define _GSF_SESSION_QUEUE_HEADER_

#include <memory>

namespace gsf
{
	class Session;
	struct NetworkThread;

	class SessionQueue
	{
	public:
		SessionQueue();
		~SessionQueue();

		Session *push(int32_t fd, NetworkThread *thread);

		int32_t pop(Session *sessionPtr);

	private:

		Session *head_ptr_;	//
		Session *tail_ptr_;
	};
}

#endif
