
	template <typename T>
	class TEventHandler;

	class EventHandler;
	typedef std::shared_ptr<EventHandler> EventHandlerPtr;

	//R ()
	template <typename R>
	class TEventHandler<R()>:public EventHandler
	{
	public:
		typedef R(*FUNC_TYPE)();
		TEventHandler(FUNC_TYPE func);
		void execute();
	private:
		FUNC_TYPE	m_func;
	};

	template <typename R>
	inline EventHandlerPtr make_event(R(*func)())
	{
		return std::make_shared<TEventHandler<R()>>(func);
	}

	template <typename R>
	inline TEventHandler<R()>::TEventHandler(typename TEventHandler<R()>::FUNC_TYPE func) :
		m_func(func)
	{
	}

	template <typename R>
	inline void TEventHandler<R()>::execute()
	{
		(*m_func)();
	}

	//R (P1)
	template <typename R, typename P1>
	class TEventHandler<R(P1)>:public EventHandler
	{
	public:
		typedef R(*FUNC_TYPE)(P1);
		TEventHandler(FUNC_TYPE func, P1 p1);
		void execute();
	private:
		FUNC_TYPE	m_func;
		P1			m_p1;
	};

	template <typename R, typename P1>
	inline EventHandlerPtr make_event(R(*func)(P1), P1 p1)
	{
		return std::make_shared<TEventHandler<R(P1)>>(func, p1);
	}

	template <typename R, typename P1>
	inline TEventHandler<R(P1)>::TEventHandler(typename TEventHandler<R(P1)>::FUNC_TYPE func, P1 p1) :
		m_func(func),
		m_p1(p1)
	{
	}

	template <typename R, typename P1>
	inline void TEventHandler<R(P1)>::execute()
	{
		(*m_func)(m_p1);
	}

	//R (P1,P2)
	template <typename R, typename P1, typename P2>
	class TEventHandler<R(P1, P2)>:public EventHandler
	{
	public:
		typedef R(*FUNC_TYPE)(P1, P2);
		TEventHandler(FUNC_TYPE func, P1 p1, P2 p2);
		void execute();
	private:
		FUNC_TYPE	m_func;
		P1			m_p1;
		P2			m_p2;
	};

	template <typename R, typename P1, typename P2>
	inline EventHandlerPtr make_event(R(*func)(P1, P2), P1 p1, P2 p2)
	{
		return std::make_shared<TEventHandler<R(P1, P2)>>(func, p1, p2);
	}

	template <typename R, typename P1, typename P2>
	inline TEventHandler<R(P1, P2)>::TEventHandler(typename TEventHandler<R(P1, P2)>::FUNC_TYPE func, P1 p1, P2 p2) :
		m_func(func),
		m_p1(p1),
		m_p2(p2)
	{
	}

	template <typename R, typename P1, typename P2>
		inline void TEventHandler<R(P1, P2)>::execute()
		{
			(*m_func)(m_p1, m_p2);
		}

		//R (P1,P2,P3)
		template <typename R, typename P1, typename P2, typename P3>
		class TEventHandler<R(P1, P2, P3)>:public EventHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3);
			TEventHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3);
			void execute();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
		};

		template <typename R, typename P1, typename P2, typename P3>
		inline EventHandlerPtr make_event(R(*func)(P1, P2, P3), P1 p1, P2 p2, P3 p3)
		{
			return std::make_shared<TEventHandler<R(P1, P2, P3)>>(func, p1, p2, p3);
		}

		template <typename R, typename P1, typename P2, typename P3>
		inline TEventHandler<R(P1, P2, P3)>::TEventHandler(typename TEventHandler<R(P1, P2, P3)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3)
		{
		}

		template <typename R, typename P1, typename P2, typename P3>
		inline void TEventHandler<R(P1, P2, P3)>::execute()
		{
			(*m_func)(m_p1, m_p2, m_p3);
		}

		//R (P1,P2,P3,P4)
		template <typename R, typename P1, typename P2, typename P3, typename P4>
		class TEventHandler<R(P1, P2, P3, P4)>:public EventHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4);
			TEventHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4);
			void execute();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4>
		inline EventHandlerPtr make_event(R(*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4)
		{
			return std::make_shared<TEventHandler<R(P1, P2, P3, P4)>>(func, p1, p2, p3, p4);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4>
		inline TEventHandler<R(P1, P2, P3, P4)>::TEventHandler(typename TEventHandler<R(P1, P2, P3, P4)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4>
		inline void TEventHandler<R(P1, P2, P3, P4)>::execute()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4);
		}

		//R (P1,P2,P3,P4,P5)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		class TEventHandler<R(P1, P2, P3, P4, P5)>:public EventHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5);
			TEventHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);
			void execute();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline EventHandlerPtr make_event(R(*func)(P1, P2, P3, P4, P5), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			return std::make_shared<TEventHandler<R(P1, P2, P3, P4, P5)>>(func, p1, p2, p3, p4, p5);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline TEventHandler<R(P1, P2, P3, P4, P5)>::TEventHandler(typename TEventHandler<R(P1, P2, P3, P4, P5)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline void TEventHandler<R(P1, P2, P3, P4, P5)>::execute()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5);
		}

		//R (P1,P2,P3,P4,P5,P6)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		class TEventHandler<R(P1, P2, P3, P4, P5, P6)>:public EventHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5, P6);
			TEventHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);
			void execute();
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
		inline EventHandlerPtr make_event(R(*func)(P1, P2, P3, P4, P5, P6), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			return std::make_shared<TEventHandler<R(P1, P2, P3, P4, P5, P6)>>(func, p1, p2, p3, p4, p5, p6);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline TEventHandler<R(P1, P2, P3, P4, P5, P6)>::TEventHandler(typename TEventHandler<R(P1, P2, P3, P4, P5, P6)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) :
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
		inline void TEventHandler<R(P1, P2, P3, P4, P5, P6)>::execute()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6);
		}

		//R (P1,P2,P3,P4,P5,P6,P7)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		class TEventHandler<R(P1, P2, P3, P4, P5, P6, P7)>:public EventHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7);
			TEventHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);
			void execute();
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
		inline EventHandlerPtr make_event(R(*func)(P1, P2, P3, P4, P5, P6, P7), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			return std::make_shared<TEventHandler<R(P1, P2, P3, P4, P5, P6, P7)>>(func, p1, p2, p3, p4, p5, p6, p7);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline TEventHandler<R(P1, P2, P3, P4, P5, P6, P7)>::TEventHandler(typename TEventHandler<R(P1, P2, P3, P4, P5, P6, P7)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) :
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
		inline void TEventHandler<R(P1, P2, P3, P4, P5, P6, P7)>::execute()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7);
		}

		//R (P1,P2,P3,P4,P5,P6,P7,P8)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		class TEventHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>:public EventHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7, P8);
			TEventHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8);
			void execute();
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
		inline EventHandlerPtr make_event(R(*func)(P1, P2, P3, P4, P5, P6, P7, P8), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			return std::make_shared<TEventHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>>(func, p1, p2, p3, p4, p5, p6, p7, p8);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline TEventHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>::TEventHandler(typename TEventHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) :
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
		inline void TEventHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>::execute()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7, m_p8);
		}

		//R (P1,P2,P3,P4,P5,P6,P7,P8,P9)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		class TEventHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>:public EventHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7, P8, P9);
			TEventHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9);
			void execute();
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
		inline EventHandlerPtr make_event(R(*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			return std::make_shared<TEventHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>>(func, p1, p2, p3, p4, p5, p6, p7, p8, p9);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline TEventHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::TEventHandler(typename TEventHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9) :
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
		inline void TEventHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::execute()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7, m_p8, m_p9);
		}

		//R (C::*)()
		template <typename C, typename R>
		class TEventHandler<R(C::*)()>:public EventHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)();
			TEventHandler(FUNC_TYPE func, C * obj);
			void execute();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
		};

		template <typename C, typename R>
		inline EventHandlerPtr make_event(R(C::*func)(), C * obj)
		{
			return std::make_shared<TEventHandler<R(C::*)()>>(func, obj);
		}

		template <typename C, typename R>
		inline TEventHandler<R(C::*)()>::TEventHandler(typename TEventHandler<R(C::*)()>::FUNC_TYPE func, C * obj) :
			m_func(func),
			m_obj(obj)
		{
		}

		template <typename C, typename R>
		inline void TEventHandler<R(C::*)()>::execute()
		{
			(m_obj->*m_func)();
		}

		//R (C::*)(P1)
		template <typename C, typename R, typename P1>
		class TEventHandler<R(C::*)(P1)>:public EventHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1);
			TEventHandler(FUNC_TYPE func, C * obj, P1 p1);
			void execute();

		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
		};

		template <typename C, typename R, typename P1>
		inline EventHandlerPtr make_event(R(C::*func)(P1), C * obj, P1 p1)
		{
			return std::make_shared<TEventHandler<R(C::*)(P1)>>(func, obj, p1);
		}

		template <typename C, typename R, typename P1>
		inline TEventHandler<R(C::*)(P1)>::TEventHandler(typename TEventHandler<R(C::*)(P1)>::FUNC_TYPE func, C * obj, P1 p1) :
			m_func(func),
			m_obj(obj),
			m_p1(p1)
		{
		}

		template <typename C, typename R, typename P1>
		inline void TEventHandler<R(C::*)(P1)>::execute()
		{
			(m_obj->*m_func)(m_p1);
		}

		//R (C::*)(P1,P2)
		template <typename C, typename R, typename P1, typename P2>
		class TEventHandler<R(C::*)(P1, P2)>:public EventHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2);
			TEventHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2);
			void execute();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
		};

		template <typename C, typename R, typename P1, typename P2>
		inline EventHandlerPtr make_event(R(C::*func)(P1, P2), C * obj, P1 p1, P2 p2)
		{
			return std::make_shared<TEventHandler<R(C::*(P1, P2))>>(func, obj, p1, p2);
		}

		template <typename C, typename R, typename P1, typename P2>
		inline TEventHandler<R(C::*)(P1, P2)>::TEventHandler(typename TEventHandler<R(C::*)(P1, P2)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2)
		{
		}

		template <typename C, typename R, typename P1, typename P2>
		inline void TEventHandler<R(C::*)(P1, P2)>::execute()
		{
			(m_obj->*m_func)(m_p1, m_p2);
		}

		//R (C::*)(P1,P2,P3)
		template <typename C, typename R, typename P1, typename P2, typename P3>
		class TEventHandler<R(C::*)(P1, P2, P3)>:public EventHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3);
			TEventHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3);
			void execute();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3>
		inline EventHandlerPtr make_event(R(C::*func)(P1, P2, P3), C * obj, P1 p1, P2 p2, P3 p3)
		{
			return std::make_shared<TEventHandler<R(C::*)(P1, P2, P3)>>(func, obj, p1, p2, p3);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3>
		inline TEventHandler<R(C::*)(P1, P2, P3)>::TEventHandler(typename TEventHandler<R(C::*)(P1, P2, P3)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3>
		inline void TEventHandler<R(C::*)(P1, P2, P3)>::execute()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3);
		}

		//R (C::*)(P1,P2,P3,P4)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4>
		class TEventHandler<R(C::*)(P1, P2, P3, P4)>:public EventHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4);
			TEventHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4);
			void execute();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4>
		inline EventHandlerPtr make_event(R(C::*func)(P1, P2, P3, P4), C * obj, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			return std::make_shared<TEventHandler<R(C::*)(P1, P2, P3, P4)>>(func, obj, p1, p2, p3, p4);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4>
		inline TEventHandler<R(C::*)(P1, P2, P3, P4)>::TEventHandler(typename TEventHandler<R(C::*)(P1, P2, P3, P4)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4>
		inline void TEventHandler<R(C::*)(P1, P2, P3, P4)>::execute()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4);
		}

		//R (C::*)(P1,P2,P3,P4,P5)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		class TEventHandler<R(C::*)(P1, P2, P3, P4, P5)>:public EventHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5);
			TEventHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);
			void execute();
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
		inline EventHandlerPtr make_event(R(C::*func)(P1, P2, P3, P4, P5), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			return std::make_shared<TEventHandler<R(C::*)(P1, P2, P3, P4, P5)>>(func, obj, p1, p2, p3, p4, p5);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline TEventHandler<R(C::*)(P1, P2, P3, P4, P5)>::TEventHandler(typename TEventHandler<R(C::*)(P1, P2, P3, P4, P5)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) :
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
		inline void TEventHandler<R(C::*)(P1, P2, P3, P4, P5)>::execute()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5);
		}

		//R (C::*)(P1,P2,P3,P4,P5,P6)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		class TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>:public EventHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5, P6);
			TEventHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);
			void execute();
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
		inline EventHandlerPtr make_event(R(C::*func)(P1, P2, P3, P4, P5, P6), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			return std::make_shared<TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>>(func, obj, p1, p2, p3, p4, p5, p6);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>::TEventHandler(typename TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) :
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
		inline void TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>::execute()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6);
		}

		//R (C::*)(P1,P2,P3,P4,P5,P6,P7)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		class TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>:public EventHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7);
			TEventHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);
			void execute();
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
		inline EventHandlerPtr make_event(R(C::*func)(P1, P2, P3, P4, P5, P6, P7), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			return std::make_shared<TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>>(func, obj, p1, p2, p3, p4, p5, p6, p7);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>::TEventHandler(typename TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) :
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
		inline void TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>::execute()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7);
		}

		//R (C::*)(P1,P2,P3,P4,P5,P6,P7,P8)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		class TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>:public EventHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7, P8);
			TEventHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8);
			void execute();
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
		inline EventHandlerPtr make_event(R(C::*func)(P1, P2, P3, P4, P5, P6, P7, P8), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			return std::make_shared<TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>>(func, obj, p1, p2, p3, p4, p5, p6, p7, p8);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>::TEventHandler(typename TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) :
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
		inline void TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>::execute()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7, m_p8);
		}

		//R (C::*)(P1,P2,P3,P4,P5,P6,P7,P8,P9)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		class TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>:public EventHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7, P8, P9);
			TEventHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9);
			void execute();
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
		inline EventHandlerPtr make_event(R(C::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			return std::make_shared<TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>>(func, obj, p1, p2, p3, p4, p5, p6, p7, p8, p9);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::TEventHandler(typename TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9) :
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
		inline void TEventHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::execute()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7, m_p8, m_p9);
		}


