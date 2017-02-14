#ifndef _ENDIAN_HEADER_
#define _ENDIAN_HEADER_

namespace gsf
{
	namespace stream
	{
		//! 整形字节反转
		/*
			@param src	源
			@param dst  目标
		*/
		//@{
		template<typename T>
		inline void reverse(const T & src,T &dst)
		{
			const char * pSrc = reinterpret_cast<const char *>(&src) + sizeof(T) -1;
			char * pDst = reinterpret_cast<char *>(&dst);
			for(int i = 0;i< sizeof(T);++i)
			{
				*pDst++ = *pSrc--;
			}
		}

		inline void reverse(const bool & src,bool &dst)
		{
			dst = src;
		}

		inline void reverse(const char & src,char &dst)
		{
			dst = src;
		}

		inline void reverse(const unsigned char & src,unsigned char & dst)
		{
			dst = src;
		}

		inline void  reverse(const signed char & src,signed char & dst)
		{
			dst = src;
		}
		//@}

		//! 大端
		class BigEndian
		{
		public:
			//! 转换
			template <typename T>
			static void convert(
				const T & src,	//!< 源
				T &dst			//!< 目标
				);
		};

		//! 小端
		class LittleEndian
		{
		public:
			template <typename T>
			static void convert(
				const T & src,	//!< 源
				T &dst			//!< 目标
				);
		};

	#if (0x1234&0xFF)==0x12
		typedef BigEndian SystemEndian;
		template <typename T>
		inline void BigEndian::convert(const T & src,T &dst)
		{
			dst = src;
		}
		
		template <typename T>
		inline void LittleEndian::convert(const T & src,T &dst)
		{
			reverse(src,dst);
		}
	#else
		typedef LittleEndian SystemEndian;
		template <typename T>
		inline void BigEndian::convert(const T & src,T &dst)
		{
			reverse(src,dst);
		}
		
		template <typename T>
		inline void LittleEndian::convert(const T & src,T &dst)
		{
			dst = src;
		}
	#endif	
	}
}

#endif
