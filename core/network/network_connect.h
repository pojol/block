#ifndef _NETWORK_CONNECT_HEADER_
#define _NETWORK_CONNECT_HEADER_

#include <thread>
#include <mutex>
#include <stdlib.h>

#include <memory>
// memcache thread.c

namespace gsf
{
	class Acceptor;
	class Connector;
 
#define ITEMS_PER_ALLOC 64

	/* An item in the connection queue. */
	typedef struct conn_queue_item CQ_ITEM;
	struct conn_queue_item {
		int             sfd;
		int             event_flags;

		void			*ptr;

		CQ_ITEM          *next;
	};

	/* A connection queue. */
	typedef struct conn_queue CQ;
	struct conn_queue
	{
		CQ_ITEM *head;
		CQ_ITEM *tail;
		std::mutex lock;
	};

	class NetworkConnect
	{
	public:
		~NetworkConnect();

		static NetworkConnect &instance();

		/*
		* Initializes a connection queue.
		*/
		void cq_init(CQ *cq);

		/*
		* Looks for an item on a connection queue, but doesn't block if there isn't
		* one.
		* Returns the item, or NULL if no item is available
		*/
		CQ_ITEM *cq_pop(CQ *cq);

		/*
		* Adds an item to a connection queue.
		*/
		void cq_push(CQ *cq, CQ_ITEM *item);

		/*
		* Returns a fresh connection queue item.
		*/
		CQ_ITEM *cqi_new(void);

		/*
		* Frees a connection queue item (adds it to the freelist.)
		*/
		void cqi_free(CQ_ITEM *item);
	private:
		NetworkConnect();
		static NetworkConnect *instance_;

	private:
		std::mutex cqi_freelist_lock;
		std::mutex stack_lock;

		/* Free list of CQ_ITEM structs */
		CQ_ITEM *cqi_freelist;
	};
}

#endif
