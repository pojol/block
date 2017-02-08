#ifndef _MIN_HEAP_HEAD_
#define _MIN_HEAP_HEAD_

#include <stdlib.h>

namespace gsf
{
	namespace timer
	{
		template <typename T>
		struct min_heap
		{
			T** p;
			unsigned n, a;
		};

		template <typename T>
		static inline void	     min_heap_ctor(min_heap<T>* s);

		template <typename T>
		static inline void	     min_heap_dtor(min_heap<T>* s);

		template <typename T>
		static inline void	     min_heap_elem_init(T* e);
		
		template <typename T>
		static inline int	     min_heap_elt_is_top(const T *e);
		
		template <typename T>
		static inline int	     min_heap_elem_greater(T *a, T *b);

		template <typename T>
		static inline int	     min_heap_empty(min_heap<T>* s);

		template <typename T>
		static inline unsigned	     min_heap_size(min_heap<T>* s);

		template <typename T>
		static inline T*  min_heap_top(min_heap<T>* s);

		template <typename T>
		static inline int	     min_heap_reserve(min_heap<T>* s, unsigned n);
		
		template <typename T>
		static inline int	     min_heap_push(min_heap<T>* s, T* e);

		template <typename T>
		static inline T*  min_heap_pop(min_heap<T>* s);

		template <typename T>
		static inline int	     min_heap_erase(min_heap<T>* s, T* e);

		template <typename T>
		static inline void	     min_heap_shift_up_(min_heap<T>* s, unsigned hole_index, T* e);

		template <typename T>
		static inline void	     min_heap_shift_down_(min_heap<T>* s, unsigned hole_index, T* e);

		template <typename T>
		int min_heap_elem_greater(T *a, T *b)
		{
			return (a->tp_ > b->tp_);
		}

		template <typename T>
		void min_heap_ctor(min_heap<T>* s) { s->p = 0; s->n = 0; s->a = 0; }

		template <typename T>
		void min_heap_dtor(min_heap<T>* s) { if (s->p) free(s->p); }

		template <typename T>
		void min_heap_elem_init(T* e) { e->min_heap_idx = -1; }

		template <typename T>
		int min_heap_empty(min_heap<T>* s) { return 0u == s->n; }

		template <typename T>
		unsigned min_heap_size(min_heap<T>* s) { return s->n; }

		template <typename T>
		T* min_heap_top(min_heap<T>* s) { return s->n ? *s->p : 0; }

		template <typename T>
		int min_heap_push(min_heap<T>* s, T* e)
		{
			if (min_heap_reserve(s, s->n + 1))
				return -1;
			min_heap_shift_up_(s, s->n++, e);
			return 0;
		}

		template <typename T>
		T* min_heap_pop(min_heap<T>* s)
		{
			if (s->n)
			{
				T* e = *s->p;
				min_heap_shift_down_(s, 0u, s->p[--s->n]);
				e->min_heap_idx = -1;
				return e;
			}
			return 0;
		}

		template <typename T>
		int min_heap_elt_is_top(const T *e)
		{
			return e->min_heap_idx == 0;
		}

		template <typename T>
		int min_heap_erase(min_heap<T>* s, T* e)
		{
			if (-1 != e->min_heap_idx)
			{
				T *last = s->p[--s->n];
				unsigned parent = (e->min_heap_idx - 1) / 2;
				/* we replace e with the last element in the heap.  We might need to
				shift it upward if it is less than its parent, or downward if it is
				greater than one or both its children. Since the children are known
				to be less than the parent, it can't need to shift both up and
				down. */
				if (e->min_heap_idx > 0 && min_heap_elem_greater(s->p[parent], last))
					min_heap_shift_up_(s, e->min_heap_idx, last);
				else
					min_heap_shift_down_(s, e->min_heap_idx, last);
				e->min_heap_idx = -1;
				return 0;
			}
			return -1;
		}

		template <typename T>
		int min_heap_reserve(min_heap<T>* s, unsigned n)
		{
			if (s->a < n)
			{
				T** p;
				unsigned a = s->a ? s->a * 2 : 8;
				if (a < n)
					a = n;
				if (!(p = (T**)realloc(s->p, a * sizeof *p)))
					return -1;
				s->p = p;
				s->a = a;
			}
			return 0;
		}

		template <typename T>
		void min_heap_shift_up_(min_heap<T>* s, unsigned hole_index, T* e)
		{
			unsigned parent = (hole_index - 1) / 2;
			while (hole_index && min_heap_elem_greater(s->p[parent], e))
			{
				(s->p[hole_index] = s->p[parent])->min_heap_idx = hole_index;
				hole_index = parent;
				parent = (hole_index - 1) / 2;
			}
			(s->p[hole_index] = e)->min_heap_idx = hole_index;
		}

		template <typename T>
		void min_heap_shift_down_(min_heap<T>* s, unsigned hole_index, T* e)
		{
			unsigned min_child = 2 * (hole_index + 1);
			while (min_child <= s->n)
			{
				min_child -= min_child == s->n || min_heap_elem_greater(s->p[min_child], s->p[min_child - 1]);
				if (!(min_heap_elem_greater(e, s->p[min_child])))
					break;
				(s->p[hole_index] = s->p[min_child])->min_heap_idx = hole_index;
				hole_index = min_child;
				min_child = 2 * (hole_index + 1);
			}
			(s->p[hole_index] = e)->min_heap_idx = hole_index;
		}
	}
}

#endif