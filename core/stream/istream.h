#ifndef _ISTREAM_HEADER_
#define _ISTREAM_HEADER_

#include <string>
#include <list>
#include <vector>
#include <set>
#include <map>
#include "block.h"
#include "stream_traits.h"

#include "OutOfBound.h"

namespace gsf
{
	namespace stream
	{
		//! 输入流模块类
		template <typename StreamTraits>
		class InputStream
		{
		public:
			//! 构造函数
			InputStream(
				BlockPtr block			//!< 内存块指针
				);
			InputStream(
				BlockPtr block,			//!< 内存块指针
				size_t start,			//!< 块内开始位置
				size_t size				//!< 块内数据长度
				);
			
			//! 输入接口
			/*!
				如果输入的数据不足够，会抛出OutOfBound异常
			*/
			//@{
			InputStream<StreamTraits> & operator >> (bool & o);
			InputStream<StreamTraits> & operator >> (char & o);
			InputStream<StreamTraits> & operator >> (signed char & o);
			InputStream<StreamTraits> & operator >> (unsigned char & o);
			InputStream<StreamTraits> & operator >> (wchar_t & o);
			InputStream<StreamTraits> & operator >> (short & o);
			InputStream<StreamTraits> & operator >> (unsigned short & o);
			InputStream<StreamTraits> & operator >> (int & o);
			InputStream<StreamTraits> & operator >> (unsigned int & o);
			InputStream<StreamTraits> & operator >> (long & o);
			InputStream<StreamTraits> & operator >> (unsigned long & o);
			InputStream<StreamTraits> & operator >> (long long & o);
			InputStream<StreamTraits> & operator >> (unsigned long long & o);
			InputStream<StreamTraits> & operator >> (float & o);
			InputStream<StreamTraits> & operator >> (double & o);
			
			template <typename Alloc>
			InputStream<StreamTraits> & operator >> (std::basic_string<char,std::char_traits<char>,Alloc> & o);
			template <typename Alloc>
			InputStream<StreamTraits> & operator >> (std::basic_string<wchar_t,std::char_traits<wchar_t>,Alloc> & o);			

			template <typename T, typename Alloc>
			InputStream<StreamTraits> & operator >> (std::vector<T,Alloc> & o);
			template <typename T, typename Alloc>
			InputStream<StreamTraits> & operator >> (std::list<T,Alloc> & o);
			template <typename T, typename Traits, typename Alloc>
			InputStream<StreamTraits> & operator >> (std::set<T, Traits, Alloc> & o);
			template <typename key, typename value, typename Traits, typename Alloc>
			InputStream<StreamTraits> & operator >> (std::map<key, value, Traits, Alloc> & o);
			//@}

			//!读取全部的缓存内容
			void read(
				const char * & buffer,	//!< 缓存指针
				size_t & size			//!< 缓存大小
				);
			
			//! 忽略部分内容
			/*!
				直接将内部读指针增长size字节<br>
				如果输入的数据不足够，会抛出OutOfBound异常
			*/
			void skip(
				size_t size				//!< 忽略的字节数
				);
		private:
			template<typename T>
			InputStream<StreamTraits> & pop(T & t);
			template<typename T>
			InputStream<StreamTraits> & popInt(T & t);
			BlockPtr			m_block;
			const char *		m_read;
			const char *		m_base;
			const char *		m_tail;
		};

		template <typename StreamTraits>
		inline InputStream<StreamTraits>::InputStream(BlockPtr block) : m_block(block)
		{
			m_base = m_block->buf_;
			m_read = m_base;
			m_tail = m_read + m_block->size_;
		}

		template <typename StreamTraits>
		inline InputStream<StreamTraits>::InputStream(
				BlockPtr block,	
				size_t start,
				size_t size	
				) : m_block(block)
		{
			m_base = m_block->buf_ + start;
			m_read = m_base;
			m_tail = m_read + size;
		}
		
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (bool & o)
		{
			return popInt(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (char & o)
		{
			return popInt(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (signed char & o)
		{
			return popInt(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (unsigned char & o)
		{
			return popInt(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (wchar_t & o)
		{
			return popInt(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (short & o)
		{
			return popInt(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (unsigned short & o)
		{
			return popInt(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (int & o)
		{
			return popInt(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (unsigned int & o)
		{
			return popInt(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (long & o)
		{
			return popInt(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (unsigned long & o)
		{
			return popInt(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (long long & o)
		{
			return popInt(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (unsigned long long & o)
		{
			return popInt(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (float & o)
		{
			return pop(o);
		}
		template <typename StreamTraits>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (double & o)
		{
			return pop(o);
		}
		template <typename StreamTraits>
		template <typename Alloc>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (std::basic_string<char,std::char_traits<char>,Alloc> & o)
		{
			typename StreamTraits::LengthType len;
			*this >> len;
			
			const char * readed = m_read + len*sizeof(char); 
			if (readed > m_tail || readed < m_read )
				throw OutOfBound();
			o.assign(m_read, len);
			m_read = readed;
			return *this;
		}
		template <typename StreamTraits>
		template <typename Alloc>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (std::basic_string<wchar_t,std::char_traits<wchar_t>,Alloc> & o)
		{
			typename StreamTraits::LengthType len;
			*this >> len;

			const char * readed = m_read + len*sizeof(wchar_t); 
			if ( readed > m_tail || readed < m_read)
				throw OutOfBound();
			o.assign(reinterpret_cast<const wchar_t*>(m_read), len);
			m_read = readed;
			return *this;
		}

		template <typename StreamTraits>
		template <typename T, typename Alloc>
		InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (std::vector<T, Alloc> & o)
		{
			typename StreamTraits::LengthType len;
			*this >> len;

			for (typename StreamTraits::LengthType i = 0; i < len; ++i)
			{
				T t;
				*this >> t;
				o.push_back(t);
			}
			return *this;
		}

		template <typename StreamTraits>
		template <typename T, typename Alloc>
		InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (std::list<T, Alloc> & o)
		{
			typename StreamTraits::LengthType len;
			*this >> len;

			for (typename StreamTraits::LengthType i = 0; i < len; ++i)
			{
				T t;
				*this >> t;
				o.push_back(t);
			}
			return *this;
		}

		template <typename StreamTraits>
		template <typename T, typename Traits, typename Alloc>
		InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (std::set<T, Traits, Alloc> & o)
		{
			typename StreamTraits::LengthType len;
			*this >> len;

			for (typename StreamTraits::LengthType i = 0; i < len; ++i)
			{
				T t;
				*this >> t;
				o.insert(t);
			}
			return *this;
		}

		template <typename StreamTraits>
		template <typename key, typename value, typename Traits, typename Alloc>
		InputStream<StreamTraits> & InputStream<StreamTraits>::operator >> (std::map<key, value, Traits, Alloc> & o)
		{
			typename StreamTraits::LengthType len;

			*this >> len;
			for (typename StreamTraits::LengthType i = 0; i < len; ++i)
			{
				key k;
				value v;
				*this >> k;
				*this >> v;

				o.insert(std::pair<key, value>(k, v));
			}

			return *this;
		}

		template <typename StreamTraits>
		template<typename T>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::pop(T & t)
		{
			const char * readed = m_read + sizeof(T);
			if (readed> m_tail || readed < m_read)
				throw OutOfBound();
			t = *(reinterpret_cast<const T *>(m_read));
			m_read = readed;
			return *this;
		}

		template <typename StreamTraits>
		template<typename T>
		inline InputStream<StreamTraits> & InputStream<StreamTraits>::popInt(T & t)
		{
			const char * readed = m_read + sizeof(T);
			if (readed> m_tail || readed < m_read)
				throw OutOfBound();
			StreamTraits::IntEndian::convert(*reinterpret_cast<const T *>(m_read),t);
			m_read = readed;
			return *this;
		}

		template <typename StreamTraits>
		inline void InputStream<StreamTraits>::read(
			const char * & buffer,
			size_t & size
			)
		{
			buffer = m_base;
			size = m_tail - m_base;
		}
		
		template <typename StreamTraits>
		inline void InputStream<StreamTraits>::skip(
			size_t size
			)
		{
			const char * readed = m_read + size;
			if (readed > m_tail || readed < m_read)
				throw OutOfBound();
			m_read = readed;
		}

		//!//!默认输入流类型
		typedef InputStream<DefaultStreamTraits>	IStream;
	}
}
#endif
