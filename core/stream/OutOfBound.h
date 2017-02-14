#ifndef _OUT_OF_BOUND_HEADER_
#define _OUT_OF_BOUND_HEADER_

#include <exception>

#define EXCEPT_OUT_OF_BOUND	"Out of bound"

namespace gsf
{
	namespace stream
	{
		//! 内存越界异常类
		class OutOfBound : public std::exception
		{
		public:
			//! 构造函数
			OutOfBound();
			OutOfBound(const char* descr);
			
			~OutOfBound() throw ();
			virtual const char* what() const throw ();
		private:
			std::string m_strError;
		};

		inline OutOfBound::OutOfBound()
		{
			m_strError = EXCEPT_OUT_OF_BOUND;
		}
		
		inline OutOfBound::~OutOfBound() throw ()
		{
		}

		inline OutOfBound::OutOfBound(const char* descr)
		{
			m_strError = descr;
		}

		inline const char* OutOfBound::what() const throw()
		{
			return m_strError.c_str();
		}
	}
}

#endif
