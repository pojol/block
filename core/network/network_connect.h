#ifndef _NETWORK_CONNECT_HEADER_
#define _NETWORK_CONNECT_HEADER_

#include <thread>
#include <mutex>
#include <stdlib.h>

// memcache thread.c

namespace gsf
{
	class Acceptor;
	class Connector;
  
	static std::mutex cqi_freelist_lock;
	static std::mutex stack_lock;

#define ITEMS_PER_ALLOC 64

	/* An item in the connection queue. */
	typedef struct conn_queue_item CQ_ITEM;
	struct conn_queue_item {
		int             sfd;
		int             event_flags;
      
		void			*ListenPtr;

		CQ_ITEM          *next;
	};

  /* Free list of CQ_ITEM structs */
  static CQ_ITEM *cqi_freelist;

  /* A connection queue. */
  typedef struct conn_queue CQ;
  struct conn_queue 
  {
      CQ_ITEM *head;
      CQ_ITEM *tail;
      std::mutex lock;
  };
  
    /*
   * Initializes a connection queue.
   */
  static void cq_init(CQ *cq) 
  {
      cq->head = NULL;
      cq->tail = NULL;
  }

  /*
   * Looks for an item on a connection queue, but doesn't block if there isn't
   * one.
   * Returns the item, or NULL if no item is available
   */
  static CQ_ITEM *cq_pop(CQ *cq) 
  {
      CQ_ITEM *item;

      cq->lock.lock();
      item = cq->head;
      if (NULL != item) {
          cq->head = item->next;
          if (NULL == cq->head)
              cq->tail = NULL;
      }
      cq->lock.unlock();

      return item;
  }

  /*
   * Adds an item to a connection queue.
   */
  static void cq_push(CQ *cq, CQ_ITEM *item) 
  {
      item->next = NULL;

      cq->lock.lock();
      if (NULL == cq->tail)
          cq->head = item;
      else
          cq->tail->next = item;
      cq->tail = item;
      cq->lock.unlock();
  }

  /*
   * Returns a fresh connection queue item.
   */
  static CQ_ITEM *cqi_new(void) 
  {
      CQ_ITEM *item = NULL;
      cqi_freelist_lock.lock();
      if (cqi_freelist) {
          item = cqi_freelist;
          cqi_freelist = item->next;
      }
      cqi_freelist_lock.unlock();

      if (NULL == item) {
          int i;

          /* Allocate a bunch of items at once to reduce fragmentation */
          item = (CQ_ITEM*)malloc(sizeof(CQ_ITEM) * ITEMS_PER_ALLOC);
          if (NULL == item) {
              //err dispose
              return NULL;
          }

          /*
           * Link together all the new items except the first one
           * (which we'll return to the caller) for placement on
           * the freelist.
           */
          for (i = 2; i < ITEMS_PER_ALLOC; i++)
              item[i - 1].next = &item[i];

          cqi_freelist_lock.lock();
          item[ITEMS_PER_ALLOC - 1].next = cqi_freelist;
          cqi_freelist = &item[1];
          cqi_freelist_lock.unlock();
      }

      return item;
  }


  /*
   * Frees a connection queue item (adds it to the freelist.)
   */
  static void cqi_free(CQ_ITEM *item) 
  {
      cqi_freelist_lock.lock();
      item->next = cqi_freelist;
      cqi_freelist = item;
      cqi_freelist_lock.unlock();
  }

}

#endif
