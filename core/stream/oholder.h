#ifndef _O_HOLDER_HEADER
#define _O_HOLDER_HEADER

#include <string>
#include "block.h"


namespace gsf
{
	namespace stream
	{
		//! 输出占位符结构
		/*!
			类型T必须为POD类型
		*/
		template <class T>
		class OHolder
		{
			template <typename Traints>
			friend class OutputStream;
		public:
			//! 构造函数
			OHolder();

			OHolder & operator=(const T & v);

			operator T & ();
		private:
			BlockPtr	m_block;
			size_t		m_position;	
		};

		template <class T>
		inline OHolder<T>::OHolder()
		{
		}

		template <class T>
		inline OHolder<T> & OHolder<T>::operator=(const T & v)
		{
			* reinterpret_cast<T*>(m_block->buf_ + m_position) = v;
			return *this;
		}

		template <class T>
		inline OHolder<T>::operator T & ()
		{
			return *reinterpret_cast<T*>(m_block->buf_ + m_position);
		}
	}
}

#endif
