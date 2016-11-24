
#include <memory>
#include "timer_handler.h"

namespace gsf
{
	namespace utils
	{
		template <typename T>
		class TTimerHandler;

		//R ()
		template <typename R>
		class TTimerHandler<R()>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)();
			TTimerHandler(FUNC_TYPE func);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
		};

		template <typename R>
		inline TimerHandler * makeTimerHandler(R(*func)())
		{
			return new TTimerHandler<R()>(func);
		}

		template <typename R>
		inline TTimerHandler<R()>::TTimerHandler(typename TTimerHandler<R()>::FUNC_TYPE func) :
			m_func(func)
		{
		}

		template <typename R>
		inline void TTimerHandler<R()>::handleTimeout()
		{
			(*m_func)();
		}

		//R (P1)
		template <typename R, typename P1>
		class TTimerHandler<R(P1)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1);
			TTimerHandler(FUNC_TYPE func, P1 p1);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
		};

		template <typename R, typename P1>
		inline TimerHandler * makeTimerHandler(R(*func)(P1), P1 p1)
		{
			return std::make_shared<TTimerHandler<R(P1)>>(func, p1);
		}

		template <typename R, typename P1>
		inline TTimerHandler<R(P1)>::TTimerHandler(typename TTimerHandler<R(P1)>::FUNC_TYPE func, P1 p1) :
			m_func(func),
			m_p1(p1)
		{
		}

		template <typename R, typename P1>
		inline void TTimerHandler<R(P1)>::handleTimeout()
		{
			(*m_func)(m_p1);
		}

		//R (P1,P2)
		template <typename R, typename P1, typename P2>
		class TTimerHandler<R(P1, P2)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
		};

		template <typename R, typename P1, typename P2>
		inline TimerHandler * makeTimerHandler(R(*func)(P1, P2), P1 p1, P2 p2)
		{
			return std::make_shared(TTimerHandler<R(P1, P2)>)(func, p1, p2);
		}

		template <typename R, typename P1, typename P2>
		inline TTimerHandler<R(P1, P2)>::TTimerHandler(typename TTimerHandler<R(P1, P2)>::FUNC_TYPE func, P1 p1, P2 p2) :
			m_func(func),
			m_p1(p1),
			m_p2(p2)
		{
		}

		template <typename R, typename P1, typename P2>
		inline void TTimerHandler<R(P1, P2)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2);
		}

		//R (P1,P2,P3)
		template <typename R, typename P1, typename P2, typename P3>
		class TTimerHandler<R(P1, P2, P3)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
		};

		template <typename R, typename P1, typename P2, typename P3>
		inline TimerHandler * makeTimerHandler(R(*func)(P1, P2, P3), P1 p1, P2 p2, P3 p3)
		{
			return std::make_shared(TTimerHandler<R(P1, P2, P3)>)(func, p1, p2, p3);
		}

		template <typename R, typename P1, typename P2, typename P3>
		inline TTimerHandler<R(P1, P2, P3)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3)
		{
		}

		template <typename R, typename P1, typename P2, typename P3>
		inline void TTimerHandler<R(P1, P2, P3)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3);
		}

		//R (P1,P2,P3,P4)
		template <typename R, typename P1, typename P2, typename P3, typename P4>
		class TTimerHandler<R(P1, P2, P3, P4)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4>
		inline TimerHandler * makeTimerHandler(R(*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4)
		{
			return std::make_shared(TTimerHandler<R(P1, P2, P3, P4)>)(func, p1, p2, p3, p4);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4>
		inline TTimerHandler<R(P1, P2, P3, P4)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3, P4)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4>
		inline void TTimerHandler<R(P1, P2, P3, P4)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4);
		}

		//R (P1,P2,P3,P4,P5)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		class TTimerHandler<R(P1, P2, P3, P4, P5)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline TimerHandler * makeTimerHandler(R(*func)(P1, P2, P3, P4, P5), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			return std::make_shared(TTimerHandler<R(P1, P2, P3, P4, P5)>)(func, p1, p2, p3, p4, p5);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline TTimerHandler<R(P1, P2, P3, P4, P5)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3, P4, P5)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline void TTimerHandler<R(P1, P2, P3, P4, P5)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5);
		}

		//R (P1,P2,P3,P4,P5,P6)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		class TTimerHandler<R(P1, P2, P3, P4, P5, P6)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5, P6);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline TimerHandler * makeTimerHandler(R(*func)(P1, P2, P3, P4, P5, P6), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			return std::make_shared(TTimerHandler<R(P1, P2, P3, P4, P5, P6)>)(func, p1, p2, p3, p4, p5, p6);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline TTimerHandler<R(P1, P2, P3, P4, P5, P6)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3, P4, P5, P6)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline void TTimerHandler<R(P1, P2, P3, P4, P5, P6)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6);
		}

		//R (P1,P2,P3,P4,P5,P6,P7)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		class TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
			P7			m_p7;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline TimerHandler * makeTimerHandler(R(*func)(P1, P2, P3, P4, P5, P6, P7), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			return std::make_shared(TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7)>)(func, p1, p2, p3, p4, p5, p6, p7);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6),
			m_p7(p7)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline void TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7);
		}

		//R (P1,P2,P3,P4,P5,P6,P7,P8)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		class TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7, P8);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
			P7			m_p7;
			P8			m_p8;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline TimerHandler * makeTimerHandler(R(*func)(P1, P2, P3, P4, P5, P6, P7, P8), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			return std::make_shared(TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>)(func, p1, p2, p3, p4, p5, p6, p7, p8);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6),
			m_p7(p7),
			m_p8(p8)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline void TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7, m_p8);
		}

		//R (P1,P2,P3,P4,P5,P6,P7,P8,P9)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		class TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7, P8, P9);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
			P7			m_p7;
			P8			m_p8;
			P9			m_p9;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline TimerHandler * makeTimerHandler(R(*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			return std::make_shared(TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>)(func, p1, p2, p3, p4, p5, p6, p7, p8, p9);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6),
			m_p7(p7),
			m_p8(p8),
			m_p9(p9)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline void TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7, m_p8, m_p9);
		}

		//R (C::*)()
		template <typename C, typename R>
		class TTimerHandler<R(C::*)()>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)();
			TTimerHandler(FUNC_TYPE func, C * obj);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
		};

		template <typename C, typename R>
		inline TimerHandler * makeTimerHandler(R(C::*func)(), C * obj)
		{
			return new TTimerHandler<R(C::*)()>(func, obj);
		}

		template <typename C, typename R>
		inline TTimerHandler<R(C::*)()>::TTimerHandler(typename TTimerHandler<R(C::*)()>::FUNC_TYPE func, C * obj) :
			m_func(func),
			m_obj(obj)
		{
		}

		template <typename C, typename R>
		inline void TTimerHandler<R(C::*)()>::handleTimeout()
		{
			(m_obj->*m_func)();
		}

		//R (C::*)(P1)
		template <typename C, typename R, typename P1>
		class TTimerHandler<R(C::*)(P1)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
		};

		template <typename C, typename R, typename P1>
		inline TimerHandler * makeTimerHandler(R(C::*func)(P1), C * obj, P1 p1)
		{
			return std::make_shared(TTimerHandler<R(C::*)(P1)>)(func, obj, p1);
		}

		template <typename C, typename R, typename P1>
		inline TTimerHandler<R(C::*)(P1)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1)>::FUNC_TYPE func, C * obj, P1 p1) :
			m_func(func),
			m_obj(obj),
			m_p1(p1)
		{
		}

		template <typename C, typename R, typename P1>
		inline void TTimerHandler<R(C::*)(P1)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1);
		}

		//R (C::*)(P1,P2)
		template <typename C, typename R, typename P1, typename P2>
		class TTimerHandler<R(C::*)(P1, P2)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
		};

		template <typename C, typename R, typename P1, typename P2>
		inline TimerHandler * makeTimerHandler(R(C::*func)(P1, P2), C * obj, P1 p1, P2 p2)
		{
			return std::make_shared(TTimerHandler<R(C::*)(P1, P2)>)(func, obj, p1, p2);
		}

		template <typename C, typename R, typename P1, typename P2>
		inline TTimerHandler<R(C::*)(P1, P2)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2)
		{
		}

		template <typename C, typename R, typename P1, typename P2>
		inline void TTimerHandler<R(C::*)(P1, P2)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2);
		}

		//R (C::*)(P1,P2,P3)
		template <typename C, typename R, typename P1, typename P2, typename P3>
		class TTimerHandler<R(C::*)(P1, P2, P3)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3>
		inline TimerHandler * makeTimerHandler(R(C::*func)(P1, P2, P3), C * obj, P1 p1, P2 p2, P3 p3)
		{
			return std::make_shared(TTimerHandler<R(C::*)(P1, P2, P3)>)(func, obj, p1, p2, p3);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3>
		inline TTimerHandler<R(C::*)(P1, P2, P3)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3>
		inline void TTimerHandler<R(C::*)(P1, P2, P3)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3);
		}

		//R (C::*)(P1,P2,P3,P4)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4>
		class TTimerHandler<R(C::*)(P1, P2, P3, P4)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4>
		inline TimerHandler * makeTimerHandler(R(C::*func)(P1, P2, P3, P4), C * obj, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			return std::make_shared(TTimerHandler<R(C::*)(P1, P2, P3, P4)>)(func, obj, p1, p2, p3, p4);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4>
		inline TTimerHandler<R(C::*)(P1, P2, P3, P4)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3, P4)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4>
		inline void TTimerHandler<R(C::*)(P1, P2, P3, P4)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4);
		}

		//R (C::*)(P1,P2,P3,P4,P5)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		class TTimerHandler<R(C::*)(P1, P2, P3, P4, P5)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline TimerHandler * makeTimerHandler(R(C::*func)(P1, P2, P3, P4, P5), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			return std::make_shared(TTimerHandler<R(C::*)(P1, P2, P3, P4, P5)>)(func, obj, p1, p2, p3, p4, p5);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline TTimerHandler<R(C::*)(P1, P2, P3, P4, P5)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3, P4, P5)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline void TTimerHandler<R(C::*)(P1, P2, P3, P4, P5)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5);
		}

		//R (C::*)(P1,P2,P3,P4,P5,P6)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		class TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5, P6);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline TimerHandler * makeTimerHandler(R(C::*func)(P1, P2, P3, P4, P5, P6), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			return std::make_shared(TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>)(func, obj, p1, p2, p3, p4, p5, p6);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline void TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6);
		}

		//R (C::*)(P1,P2,P3,P4,P5,P6,P7)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		class TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
			P7			m_p7;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline TimerHandler * makeTimerHandler(R(C::*func)(P1, P2, P3, P4, P5, P6, P7), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			return std::make_shared(TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>)(func, obj, p1, p2, p3, p4, p5, p6, p7);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6),
			m_p7(p7)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline void TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7);
		}

		//R (C::*)(P1,P2,P3,P4,P5,P6,P7,P8)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		class TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7, P8);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
			P7			m_p7;
			P8			m_p8;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline TimerHandler * makeTimerHandler(R(C::*func)(P1, P2, P3, P4, P5, P6, P7, P8), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			return std::make_shared(TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>)(func, obj, p1, p2, p3, p4, p5, p6, p7, p8);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6),
			m_p7(p7),
			m_p8(p8)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline void TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7, m_p8);
		}

		//R (C::*)(P1,P2,P3,P4,P5,P6,P7,P8,P9)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		class TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7, P8, P9);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
			P7			m_p7;
			P8			m_p8;
			P9			m_p9;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline TimerHandler * makeTimerHandler(R(C::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			return std::make_shared(TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>)(func, obj, p1, p2, p3, p4, p5, p6, p7, p8, p9);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6),
			m_p7(p7),
			m_p8(p8),
			m_p9(p9)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline void TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7, m_p8, m_p9);
		}


	}
}

