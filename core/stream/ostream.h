#ifndef _O_STREAM_HEADER_
#define _O_STREAM_HEADER_

#include <string>
#include <list>
#include <vector>
#include <set>
#include <map>

#include "block.h"
#include "oholder.h"
#include "stream_traits.h"

namespace gsf
{
	namespace stream
	{
		//! 输出流模板类
		template <typename StreamTraits>
		class OutputStream
		{
		public:
			//! 构造函数
			OutputStream();
			
			//! 输出接口
			/*!
			*/
			//@{
			OutputStream<StreamTraits> & operator << (const bool & o);
			OutputStream<StreamTraits> & operator << (const char & o);
			OutputStream<StreamTraits> & operator << (const signed char & o);
			OutputStream<StreamTraits> & operator << (const unsigned char & o);
			OutputStream<StreamTraits> & operator << (const wchar_t & o);
			OutputStream<StreamTraits> & operator << (const short & o);
			OutputStream<StreamTraits> & operator << (const unsigned short & o);
			OutputStream<StreamTraits> & operator << (const int & o);
			OutputStream<StreamTraits> & operator << (const unsigned int & o);
			OutputStream<StreamTraits> & operator << (const long & o);
			OutputStream<StreamTraits> & operator << (const unsigned long & o);
			OutputStream<StreamTraits> & operator << (const long long & o);
			OutputStream<StreamTraits> & operator << (const unsigned long long & o);
			OutputStream<StreamTraits> & operator << (const float & o);
			OutputStream<StreamTraits> & operator << (const double & o);
			
			template <typename Alloc>
			OutputStream<StreamTraits> & operator << (const std::basic_string<char,std::char_traits<char>,Alloc> & o);
			template <typename Alloc>
			OutputStream<StreamTraits> & operator << (const std::basic_string<wchar_t,std::char_traits<wchar_t>,Alloc> & o);			
			template <typename T>
			OutputStream<StreamTraits> & operator << (OHolder<T> & o);

			template <typename T, typename Alloc>
			OutputStream<StreamTraits> & operator << (const std::vector<T,Alloc> & o);
			template <typename T, typename Alloc>
			OutputStream<StreamTraits> & operator << (const std::list<T,Alloc> & o);
			template <typename T, typename Traits, typename Alloc>
			OutputStream<StreamTraits> & operator << (const std::set<T, Traits, Alloc> & o);
			template <typename key, typename value, typename Traits, typename Alloc>
			OutputStream<StreamTraits> & operator << (const std::map<key, value, Traits, Alloc> & o);
			//@}

			//!将长度为size数据写入IStream
			void write(
				const char* data,		//!< 数据
				size_t size				//!< 长度
				);

			//!获得共享内存对象
			BlockPtr getBlock() const;

			//! 清除内部数据
			/*
				内部会重新创建Block
			*/
			void clear();
		private:
			template <typename T>
			OutputStream<StreamTraits> & push(const T & o);
			template <typename T>
			OutputStream<StreamTraits> & pushInt(const T & o);
			void incDataSize(size_t incSize);
			BlockPtr			m_block;
			char *				m_write;
			const char *		m_tail;
		};

		template <typename StreamTraits>
		inline OutputStream<StreamTraits>::OutputStream():
			m_block(new Block())
		{
			m_write = m_block->buffer;
			m_tail = m_write + m_block->total;
		}
		
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const bool & o)
		{
			return pushInt(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const char & o)
		{
			return pushInt(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const signed char & o)
		{
			return pushInt(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const unsigned char & o)
		{
			return pushInt(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const wchar_t & o)
		{
			return pushInt(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const short & o)
		{
			return pushInt(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const unsigned short & o)
		{
			return pushInt(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const int & o)
		{
			return pushInt(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const unsigned int & o)
		{
			return pushInt(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const long & o)
		{
			return pushInt(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const unsigned long & o)
		{
			return pushInt(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const long long & o)
		{
			return pushInt(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const unsigned long long & o)
		{
			return pushInt(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const float & o)
		{
			return push(o);
		}
		template <typename StreamTraits>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const double & o)
		{
			return push(o);
		}
		
		template <typename StreamTraits>
		template <typename Alloc>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const std::basic_string<char,std::char_traits<char>,Alloc> & o)
		{
			typename StreamTraits::LengthType len = static_cast<typename StreamTraits::LengthType>(o.length());
			*this << len;
			size_t incSize = len;
			incDataSize(incSize);
			memcpy(static_cast<void*>(m_write), o.c_str(), incSize);
			m_write += len;

			return *this;
		}

		template <typename StreamTraits>
		template <typename Alloc>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const std::basic_string<wchar_t,std::char_traits<wchar_t>,Alloc> & o)
		{
			typename StreamTraits::LengthType len = static_cast<typename StreamTraits::LengthType>(o.length());
			*this << len;
			size_t incSize = len * sizeof(wchar_t);
			incDataSize(incSize);
			memcpy(static_cast<void*>(m_write), o.c_str(),incSize);
			m_write += incSize;

			return *this;
		}

		template <typename StreamTraits>
		template <typename T>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (OHolder<T> & o)
		{
			size_t incSize = sizeof(T);
			incDataSize(incSize);
			o.m_block = m_block;
			o.m_position = m_write - m_block->buffer;
			m_write += incSize;
			return *this;
		}

		template <typename StreamTraits>
		template <typename T, typename Alloc>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const std::vector<T, Alloc> & o)
		{
			typename StreamTraits::LengthType len = static_cast<typename StreamTraits::LengthType>(o.size());
			*this << len;
			for (typename std::vector<T>::const_iterator it = o.begin(); it != o.end(); ++it)
			{
				*this << *it;
			}

			return *this;
		}

		template <typename StreamTraits>
		template <typename T, typename Alloc>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const std::list<T, Alloc> & o)
		{
			typename StreamTraits::LengthType len = static_cast<typename StreamTraits::LengthType>(o.size());
			*this << len;
			for (typename std::list<T>::const_iterator it = o.begin(); it != o.end(); ++it)
			{
				*this << *it;
			}

			return *this;
		}

		template <typename StreamTraits>
		template <typename T, typename Traits, typename Alloc>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const std::set<T, Traits, Alloc> & o)
		{
			typename StreamTraits::LengthType len = static_cast<typename StreamTraits::LengthType>(o.size());
			*this << len;
			for (typename std::set<T>::const_iterator it = o.begin(); it != o.end(); ++it)
			{
				*this << *it;
			}

			return *this;
		}

		template <typename StreamTraits>
		template <typename key, typename value, typename Traits, typename Alloc>
		OutputStream<StreamTraits> & OutputStream<StreamTraits>::operator << (const std::map<key, value, Traits, Alloc> & o)
		{
			typename StreamTraits::LengthType len = static_cast<typename StreamTraits::LengthType>(o.size());
			*this << len;
			for (typename std::map<key, value, Traits, Alloc>::const_iterator it = o.begin(); it != o.end(); ++it)
			{
				*this << it->first;
				*this << it->second;
			}

			return *this;
		}

		template <typename StreamTraits>
		inline BlockPtr OutputStream<StreamTraits>::getBlock() const
		{
			return m_block;
		}

		template <typename StreamTraits>
		inline void OutputStream<StreamTraits>::clear()
		{
			m_block = new Block;
			m_write = m_block->buffer;
			m_tail = m_write + m_block->total;
		}

		template <typename StreamTraits>
		template <typename T>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::push(const T & o)
		{
			size_t incSize = sizeof(T);
			incDataSize(incSize);
			*(reinterpret_cast<T*>(m_write)) = o;
			m_write += incSize;
			return *this;
		}

		template <typename StreamTraits>
		template <typename T>
		inline OutputStream<StreamTraits> & OutputStream<StreamTraits>::pushInt(const T & o)
		{
			size_t incSize = sizeof(T);
			incDataSize(incSize);
			StreamTraits::IntEndian::convert(o,*(reinterpret_cast<T*>(m_write)));
			m_write += incSize;
			return *this;
		}

		template <typename StreamTraits>
		inline void OutputStream<StreamTraits>::incDataSize(size_t incSize)
		{
			if (m_block->incDataSize(incSize))
			{
				m_write = m_block->buffer + m_block->size - incSize;
				m_tail = m_block->buffer + m_block->total;
			}
		}

		template <typename StreamTraits>
		inline void OutputStream<StreamTraits>::write(const char* data, size_t size)
		{
			incDataSize(size);
			memcpy(m_write, data, size);
			m_write += size;
		}

		//!默认输出流类型
		typedef OutputStream<DefaultStreamTraits>	OStream;
	}
}

#endif
