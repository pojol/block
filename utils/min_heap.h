#ifndef _MIN_HEAP_HEAD_
#define _MIN_HEAP_HEAD_

#include <vector>
#include <stdexcept>

/**!
	from https://github.com/AlirezaShahabi/CPP-MinHeap/blob/master/MinHeap.h
*/

namespace gsf
{
	namespace utils
	{
		template <typename T> class MinHeap 
		{

		private:
			std::vector<T> data;
			// private utility functions
			void heapify();
			void bubble_up(int);
			void bubble_down(int);
			void swap_val(int, int);

		public:
			// constructor
			MinHeap();
			MinHeap(const std::vector<T>&);
			MinHeap(T*, int);
			// public member functions
			MinHeap& add(const T&);
			MinHeap& rem_min();
			T get_min() const;
			inline int size() const { return data.size(); }
			inline bool empty() const { return data.empty(); }
		};


		// constructors
		template <typename T> MinHeap<T>::MinHeap() :data() {}

		template <typename T> MinHeap<T>::MinHeap(const std::vector<T>& v) :
			data(v) {
			heapify();
		}

		template <typename T> MinHeap<T>::MinHeap(T* arr, int n) : data(n) {
			std::copy(arr, arr + n, data.begin());
			heapify();
		}

		// add a new element to MinHeap
		template <typename T> MinHeap<T>& MinHeap<T>::add(const T& obj) {
			data.push_back(obj);
			int last_index = size() - 1;
			bubble_up(last_index);
			return *this;
		}

		// remove the minimum element from the MinHeap
		template <typename T> MinHeap<T>& MinHeap<T>::rem_min() {
			if (!empty()) {
				int last_index = size() - 1;
				swap_val(0, last_index);
				data.pop_back();
				bubble_down(0);
			}
			return *this;
		}

		// return the minimum value
		template <typename T> T MinHeap<T>::get_min() const {
			if (empty()) { throw std::out_of_range("empty heap!"); }
			return data[0];
		}


		// private utility functions


		// heapify an ordinary array
		template <typename T> void MinHeap<T>::heapify() {
			if (empty()) { return; }
			int last_index = size() - 1;
			for (int i = last_index; i >= 0; --i) { bubble_down(i); }
		}

		// bubble_up operation
		template <typename T> void MinHeap<T>::bubble_up(int index) {
			if (index == 0) { return; }
			int parent_index = static_cast<int>((index - 1) / 2);
			if (data[parent_index] > data[index]) {
				swap_val(index, parent_index);
				bubble_up(parent_index);
			}
			return;
		}

		// bubble_down operation
		template <typename T> void MinHeap<T>::bubble_down(int index) {
			int l_index = 2 * index + 1;
			int r_index = 2 * index + 2;
			if (l_index >= size()) { return; }
			if (r_index == size()) {
				if (data[index] > data[l_index]) { swap_val(index, l_index); }
				return;
			}
			int less_index = data[l_index] < data[r_index] ? l_index : r_index;
			if (data[index] > data[less_index]) {
				swap_val(index, less_index);
				bubble_down(less_index);
			}
			else {
				return;
			}
		}

		// swap the values at the two index: i1 and i2
		template <typename T> void MinHeap<T>::swap_val(int i1, int i2) {
			T temp = data[i1];
			data[i1] = data[i2];
			data[i2] = temp;
		}

	}
}

#endif