#ifndef GX_EVENTS_H
#define GX_EVENTS_H

#include "Common.h"
#include "List.h"

namespace CoreLib
{
	namespace Basic
	{
	/***************************************************************************

	Events.h

	Usage:

		class A
		{
		public:
			void EventHandler(int a)
			{
				cout<<endl<<"function of object handler invoked. a*a = ";
				cout<<a*a<<endl;
			}
		};

		class B
		{
		public:
			typedef gxEvent1<int> gxOnEvent;
		public:
			gxOnEvent OnEvent;
			void DoSomething()
			{
				OnEvent.Invoke(4);
			}
		};

		void FuncHandler()
		{
			cout<<"Function invoked."<<endl;
		}

		void main()
		{
			A a;
			B b;
			b.OnEvent.Bind(&a,&A::EventHandler);	
			b.OnEvent.Bind(FuncHandler);			
			b.DoSomething();
			b.OnEvent.Unbind(FuncHandler);			
			b.OnEvent.Unbind(&a,&A::EventHandler);
			b.DoSomething();                       
		}

	***************************************************************************/

	#define GX_EVENT_CALL_CONVENTION __stdcall

	class NullClass
	{};

	/***************************************************************************
					------------------   10   -------------------
	****************************************************************************/


	#define GX_EVENT_TEMPLATE_ARGLIST typename Type1=NullClass, typename Type2=NullClass, typename Type3=NullClass, typename Type4=NullClass, typename Type5=NullClass, typename Type6=NullClass, typename Type7=NullClass, typename Type8=NullClass, typename Type9=NullClass, typename Type10=NullClass
	#define GX_EVENT_TEMPLATE_PARAMS Type1,Type2,Type3,Type4,Type5,Type6,Type7,Type8, Type9, Type10
	#define GX_EVENT_TEMPLATE_FULL_PARAMS Type1,Type2,Type3,Type4,Type5,Type6,Type7,Type8, Type9, Type10
	#define GX_EVENT_ARGLIST Type1 param1, Type2 param2, Type3 param3, Type4 param4, Type5 param5, Type6 param6, Type7 param7, Type8 param8, Type9 param9, Type10 param10
	#define GX_EVENT_PARAMS	param1, param2, param3, param4, param5, param6,param7,param8,param9,param10

	#pragma region gxEventElement_10Params

	template <GX_EVENT_TEMPLATE_ARGLIST>
	class EventHandler
	{
	public:
		virtual void Invoke(GX_EVENT_ARGLIST) = 0;
		virtual void *GetOwner()=0;
		virtual void *GetHandler()=0;
		virtual EventHandler<GX_EVENT_TEMPLATE_PARAMS> * Clone()=0;
	};

	template <GX_EVENT_TEMPLATE_ARGLIST>
	class EventFuncHandler : public EventHandler<GX_EVENT_TEMPLATE_PARAMS>
	{
	public:
		typedef void (*FuncHandler)(GX_EVENT_ARGLIST);
	private:
		FuncHandler Func;
	public:
		EventFuncHandler(FuncHandler h)
		{
			Func = h;
		}
		void Invoke(GX_EVENT_ARGLIST)
		{
			Func(GX_EVENT_PARAMS);
		};
		void * GetOwner()
		{
			return 0;
		}
		void * GetHandler()
		{
			return Func;
		}
		EventFuncHandler<GX_EVENT_TEMPLATE_PARAMS> * Clone()
		{
			return new EventFuncHandler<GX_EVENT_TEMPLATE_PARAMS>(Func);
		}
	};

	template <typename Class, GX_EVENT_TEMPLATE_ARGLIST>
	class EventFuncOfObjHandler : public EventHandler<GX_EVENT_TEMPLATE_PARAMS>
	{
	public:
		typedef void (Class::*FuncHandler) (GX_EVENT_ARGLIST);
	private:
		FuncHandler FHandler;
		Class * FOwner;
	
	public:
		void Invoke(GX_EVENT_ARGLIST)
		{
			(FOwner->*FHandler)(GX_EVENT_PARAMS);
		};
		EventFuncOfObjHandler(Class *Owner, FuncHandler handler)
		{
			FOwner = Owner;
			FHandler = handler;
		}
		void * GetOwner()
		{
			return FOwner;
		}
		void * GetHandler()
		{
			return (void *)(*(Int *)&FHandler);
		}
		EventHandler<GX_EVENT_TEMPLATE_PARAMS>* Clone()
		{
			return new EventFuncOfObjHandler<GX_EVENT_TEMPLATE_PARAMS>(FOwner, FHandler);
		}
	};

	template <GX_EVENT_TEMPLATE_ARGLIST>
	class Event : public Object
	{
		typedef EventHandler<GX_EVENT_TEMPLATE_PARAMS> Handler;
	private:
		List<Handler *> Handlers;
		void Bind(Handler * fobj)
		{
			Handlers.Add(fobj);
		}
		void Unbind(Handler *fobj)
		{
			int id = -1;
			for (int i=0; i<Handlers.Count(); i++)
			{
				if (Handlers[i]->GetOwner()==fobj->GetOwner() 
					&& Handlers[i]->GetHandler()==fobj->GetHandler())
				{
					id = i;
					break;
				}
			}
			if (id != -1)
			{
				delete Handlers[id];
				Handlers.Delete(id);
			}
		}
	public:
		Event()
		{
		}
		~Event()
		{
			for (int i=0; i<Handlers.Count(); i++)
				delete Handlers[i];
		}
		Event(const Event<GX_EVENT_TEMPLATE_FULL_PARAMS> & e)
		{
			operator=(e);
		}
		Event<GX_EVENT_TEMPLATE_FULL_PARAMS> & operator = (const Event<GX_EVENT_TEMPLATE_FULL_PARAMS> & e)
		{
			for (int i=0; i<e.Handlers.Count(); i++)
				Handlers.Add(e.Handlers[i]->Clone());
			return *this;
		}
		template <typename Class>
		void Bind(Class * Owner, typename EventFuncOfObjHandler<Class,GX_EVENT_TEMPLATE_PARAMS>::FuncHandler handler)
		{
			Handlers.Add(new EventFuncOfObjHandler<Class,GX_EVENT_TEMPLATE_PARAMS>(Owner,handler));
		}
		template <typename Class>
		void Unbind(Class * Owner, typename EventFuncOfObjHandler<Class,GX_EVENT_TEMPLATE_PARAMS>::FuncHandler handler)
		{
			EventFuncOfObjHandler<Class,GX_EVENT_TEMPLATE_PARAMS> h(Owner,handler);
			Unbind(&h);
		}
		void Bind(typename EventFuncHandler<GX_EVENT_TEMPLATE_PARAMS>::FuncHandler f)
		{
			Bind(new EventFuncHandler<GX_EVENT_TEMPLATE_PARAMS>(f));
		}
		void Unbind(typename EventFuncHandler<GX_EVENT_TEMPLATE_PARAMS>::FuncHandler f)
		{
			EventFuncHandler<GX_EVENT_TEMPLATE_PARAMS> h(f);
			Unbind(&h);
		}
		void Invoke(GX_EVENT_ARGLIST)
		{
			for (int i=0; i<Handlers.Count(); i++)
				Handlers[i]->Invoke(GX_EVENT_PARAMS);
		}
		void operator ()(GX_EVENT_ARGLIST)
		{
			Invoke(GX_EVENT_PARAMS);
		}
	};
	#pragma endregion gxEventElement_10Params

	/***************************************************************************
					------------------   0   -------------------
	****************************************************************************/
	#undef GX_EVENT_TEMPLATE_ARGLIST
	#undef GX_EVENT_TEMPLATE_PARAMS
	#undef GX_EVENT_TEMPLATE_FULL_PARAMS
	#undef GX_EVENT_ARGLIST
	#undef GX_EVENT_PARAMS

	#define GX_EVENT_TEMPLATE_ARGLIST
	#define GX_EVENT_TEMPLATE_FULL_PARAMS NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass
	#define GX_EVENT_TEMPLATE_PARAMS
	#define GX_EVENT_ARGLIST
	#define GX_EVENT_PARAMS

	#pragma region gxEventElement0Params

	template<GX_EVENT_TEMPLATE_ARGLIST>
	class EventHandler<GX_EVENT_TEMPLATE_FULL_PARAMS>
	{
	public:
		virtual void Invoke(GX_EVENT_ARGLIST) = 0;
		virtual void *GetOwner()=0;
		virtual void *GetHandler()=0;
		virtual EventHandler<GX_EVENT_TEMPLATE_FULL_PARAMS> * Clone()=0;
	};

	template<GX_EVENT_TEMPLATE_ARGLIST>
	class EventFuncHandler<GX_EVENT_TEMPLATE_FULL_PARAMS> : public EventHandler<GX_EVENT_TEMPLATE_PARAMS>
	{
	public:
		typedef void (GX_EVENT_CALL_CONVENTION *FuncHandler)(GX_EVENT_ARGLIST);
	private:
		FuncHandler Func;
	public:
		EventFuncHandler(FuncHandler h)
		{
			Func = h;
		}
		EventHandler<GX_EVENT_TEMPLATE_PARAMS>* Clone()
		{
			return new EventFuncHandler<GX_EVENT_TEMPLATE_PARAMS>(Func);
		}
		void Invoke(GX_EVENT_ARGLIST)
		{
			Func(GX_EVENT_PARAMS);
		};
		void * GetOwner()
		{
			return 0;
		}
		void * GetHandler()
		{
			return Func;
		}
	};

	template <typename Class>
	class EventFuncOfObjHandler<Class,GX_EVENT_TEMPLATE_FULL_PARAMS> : public EventHandler<GX_EVENT_TEMPLATE_PARAMS>
	{
	public:
		typedef void (Class::*FuncHandler) (GX_EVENT_ARGLIST);
	private:
		FuncHandler FHandler;
		Class * FOwner;
	
	public:
		void Invoke(GX_EVENT_ARGLIST)
		{
			(FOwner->*FHandler)(GX_EVENT_PARAMS);
		};
		EventHandler<GX_EVENT_TEMPLATE_PARAMS>* Clone()
		{
			return new EventFuncOfObjHandler<Class>(FOwner, FHandler);
		}
		EventFuncOfObjHandler(Class *Owner, FuncHandler handler)
		{
			FOwner = Owner;
			FHandler = handler;
		}
		void * GetOwner()
		{
			return FOwner;
		}
		void * GetHandler()
		{
			return (void *)(*(Int *)&FHandler);
		}
	};


	template<GX_EVENT_TEMPLATE_ARGLIST>
	class Event<GX_EVENT_TEMPLATE_FULL_PARAMS> : public Object
	{
		typedef EventHandler<GX_EVENT_TEMPLATE_PARAMS> Handler;
	private:
		List<Handler *> Handlers;
		void Bind(Handler * fobj)
		{
			Handlers.Add(fobj);
		}
		void Unbind(Handler *fobj)
		{
			int id = -1;
			for (int i=0; i<Handlers.Count(); i++)
			{
				if (Handlers[i]->GetOwner()==fobj->GetOwner() 
					&& Handlers[i]->GetHandler()==fobj->GetHandler())
				{
					id = i;
					break;
				}
			}
			if (id != -1)
			{
				delete Handlers[id];
				Handlers.RemoveAt(id);
			}
		}
	public:
		Event()
		{

		}
		~Event()
		{
			for (int i=0; i<Handlers.Count(); i++)
				delete Handlers[i];
		}
		Event(const Event<GX_EVENT_TEMPLATE_FULL_PARAMS> & e)
		{
			operator=(e);
		}
		Event<GX_EVENT_TEMPLATE_FULL_PARAMS> & operator = (const Event<GX_EVENT_TEMPLATE_FULL_PARAMS> & e)
		{
			for (int i=0; i<e.Handlers.Count(); i++)
				Handlers.Add(e.Handlers[i]->Clone());
			return *this;
		}
		template <typename Class>
		void Bind(Class * Owner, typename EventFuncOfObjHandler<Class>::FuncHandler handler)
		{
			Handlers.Add(new EventFuncOfObjHandler<Class>(Owner,handler));
		}
		template <typename Class>
		void Unbind(Class * Owner, typename EventFuncOfObjHandler<Class>::FuncHandler handler)
		{
			EventFuncOfObjHandler<Class> h(Owner,handler);
			Unbind(&h);
		}
		void Bind(EventFuncHandler<GX_EVENT_TEMPLATE_PARAMS>::FuncHandler f)
		{
			Bind(new EventFuncHandler<GX_EVENT_TEMPLATE_PARAMS>(f));
		}
		void Unbind(EventFuncHandler<GX_EVENT_TEMPLATE_PARAMS> ::FuncHandler f)
		{
			EventFuncHandler<GX_EVENT_TEMPLATE_PARAMS> h(f);
			Unbind(&h);
		}
		void Invoke(GX_EVENT_ARGLIST)
		{
			for (int i=0; i<Handlers.Count(); i++)
				Handlers[i]->Invoke(GX_EVENT_PARAMS);
		}
		void operator ()(GX_EVENT_ARGLIST)
		{
			Invoke(GX_EVENT_PARAMS);
		}
	};
	#pragma endregion gxEventElement0Params

	/***************************************************************************
					------------------   1   -------------------
	****************************************************************************/

	#undef GX_EVENT_TEMPLATE_ARGLIST
	#undef GX_EVENT_TEMPLATE_FULL_PARAMS
	#undef GX_EVENT_TEMPLATE_PARAMS
	#undef GX_EVENT_ARGLIST
	#undef GX_EVENT_PARAMS

	#define GX_EVENT_TEMPLATE_ARGLIST typename Type1
	#define GX_EVENT_TEMPLATE_PARAMS Type1
	#define GX_EVENT_TEMPLATE_FULL_PARAMS Type1,NullClass,NullClass,NullClass,NullClass,NullClass,NullClass,NullClass,NullClass,NullClass
	#define GX_EVENT_ARGLIST Type1 param1
	#define GX_EVENT_PARAMS	param1

	#include "Events_Element.h"

	/***************************************************************************
					------------------   2   -------------------

					这是对具有2个参数的回调函数的相关模板类定义

					------------------   2   -------------------
	****************************************************************************/
	#undef GX_EVENT_TEMPLATE_ARGLIST
	#undef GX_EVENT_TEMPLATE_FULL_PARAMS
	#undef GX_EVENT_TEMPLATE_PARAMS
	#undef GX_EVENT_ARGLIST
	#undef GX_EVENT_PARAMS

	#define GX_EVENT_TEMPLATE_ARGLIST typename Type1, typename Type2
	#define GX_EVENT_TEMPLATE_PARAMS Type1,Type2
	#define GX_EVENT_TEMPLATE_FULL_PARAMS Type1,Type2,NullClass,NullClass,NullClass,NullClass,NullClass,NullClass,NullClass,NullClass
	#define GX_EVENT_ARGLIST Type1 param1, Type2 param2
	#define GX_EVENT_PARAMS	param1, param2

	#include "Events_Element.h"

	/***************************************************************************
					------------------   3   -------------------

					这是对具有3个参数的回调函数的相关模板类定义

					------------------   3   -------------------
	****************************************************************************/
	#undef GX_EVENT_TEMPLATE_ARGLIST
	#undef GX_EVENT_TEMPLATE_FULL_PARAMS
	#undef GX_EVENT_TEMPLATE_PARAMS
	#undef GX_EVENT_ARGLIST
	#undef GX_EVENT_PARAMS

	#define GX_EVENT_TEMPLATE_ARGLIST typename Type1, typename Type2, typename Type3
	#define GX_EVENT_TEMPLATE_PARAMS Type1,Type2,Type3
	#define GX_EVENT_TEMPLATE_FULL_PARAMS Type1,Type2,Type3,NullClass,NullClass,NullClass,NullClass,NullClass,NullClass,NullClass
	#define GX_EVENT_ARGLIST Type1 param1, Type2 param2, Type3 param3
	#define GX_EVENT_PARAMS	param1, param2, param3

	#include "Events_Element.h"

	/***************************************************************************
					------------------   4   -------------------

					这是对具有4个参数的回调函数的相关模板类定义

					------------------   4   -------------------
	****************************************************************************/
	#undef GX_EVENT_TEMPLATE_ARGLIST
	#undef GX_EVENT_TEMPLATE_FULL_PARAMS
	#undef GX_EVENT_TEMPLATE_PARAMS
	#undef GX_EVENT_ARGLIST
	#undef GX_EVENT_PARAMS

	#define GX_EVENT_TEMPLATE_ARGLIST typename Type1, typename Type2, typename Type3, typename Type4
	#define GX_EVENT_TEMPLATE_PARAMS Type1,Type2,Type3,Type4
	#define GX_EVENT_TEMPLATE_FULL_PARAMS Type1,Type2,Type3,Type4,NullClass,NullClass,NullClass,NullClass,NullClass,NullClass
	#define GX_EVENT_ARGLIST Type1 param1, Type2 param2, Type3 param3, Type4 param4
	#define GX_EVENT_PARAMS	param1, param2, param3, param4

	#include "Events_Element.h"

	/***************************************************************************
					------------------   5   -------------------

					这是对具有5个参数的回调函数的相关模板类定义

					------------------   5   -------------------
	****************************************************************************/
	#undef GX_EVENT_TEMPLATE_ARGLIST
	#undef GX_EVENT_TEMPLATE_FULL_PARAMS
	#undef GX_EVENT_TEMPLATE_PARAMS
	#undef GX_EVENT_ARGLIST
	#undef GX_EVENT_PARAMS

	#define GX_EVENT_TEMPLATE_ARGLIST typename Type1, typename Type2, typename Type3, typename Type4, typename Type5
	#define GX_EVENT_TEMPLATE_PARAMS Type1,Type2,Type3,Type4,Type5
	#define GX_EVENT_TEMPLATE_FULL_PARAMS Type1,Type2,Type3,Type4,Type5,NullClass,NullClass,NullClass,NullClass,NullClass
	#define GX_EVENT_ARGLIST Type1 param1, Type2 param2, Type3 param3, Type4 param4, Type5 param5
	#define GX_EVENT_PARAMS	param1, param2, param3, param4, param5

	#include "Events_Element.h"

	/***************************************************************************
					------------------   6   -------------------

					这是对具有6个参数的回调函数的相关模板类定义

					------------------   6   -------------------
	****************************************************************************/
	#undef GX_EVENT_TEMPLATE_ARGLIST
	#undef GX_EVENT_TEMPLATE_FULL_PARAMS
	#undef GX_EVENT_TEMPLATE_PARAMS
	#undef GX_EVENT_ARGLIST
	#undef GX_EVENT_PARAMS

	#define GX_EVENT_TEMPLATE_ARGLIST typename Type1, typename Type2, typename Type3, typename Type4, typename Type5, typename Type6
	#define GX_EVENT_TEMPLATE_PARAMS Type1,Type2,Type3,Type4,Type5,Type6
	#define GX_EVENT_TEMPLATE_FULL_PARAMS Type1,Type2,Type3,Type4,Type5,Type6,NullClass,NullClass,NullClass,NullClass
	#define GX_EVENT_ARGLIST Type1 param1, Type2 param2, Type3 param3, Type4 param4, Type5 param5, Type6 param6
	#define GX_EVENT_PARAMS	param1, param2, param3, param4, param5, param6

	#include "Events_Element.h"

	/***************************************************************************
					------------------   7   -------------------

					这是对具有7个参数的回调函数的相关模板类定义

					------------------   7   -------------------
	****************************************************************************/
	#undef GX_EVENT_TEMPLATE_ARGLIST
	#undef GX_EVENT_TEMPLATE_FULL_PARAMS
	#undef GX_EVENT_TEMPLATE_PARAMS
	#undef GX_EVENT_ARGLIST
	#undef GX_EVENT_PARAMS

	#define GX_EVENT_TEMPLATE_ARGLIST typename Type1, typename Type2, typename Type3, typename Type4, typename Type5, typename Type6, typename Type7
	#define GX_EVENT_TEMPLATE_PARAMS Type1,Type2,Type3,Type4,Type5,Type6,Type7
	#define GX_EVENT_TEMPLATE_FULL_PARAMS Type1,Type2,Type3,Type4,Type5,Type6,Type7,NullClass,NullClass,NullClass
	#define GX_EVENT_ARGLIST Type1 param1, Type2 param2, Type3 param3, Type4 param4, Type5 param5, Type6 param6, Type7 param7
	#define GX_EVENT_PARAMS	param1, param2, param3, param4, param5, param6, param7

	#include "Events_Element.h"


	/***************************************************************************
					------------------   8   -------------------

					这是对具有8个参数的回调函数的相关模板类定义

					------------------   8   -------------------
	****************************************************************************/
	#undef GX_EVENT_TEMPLATE_ARGLIST
	#undef GX_EVENT_TEMPLATE_FULL_PARAMS
	#undef GX_EVENT_TEMPLATE_PARAMS
	#undef GX_EVENT_ARGLIST
	#undef GX_EVENT_PARAMS

	#define GX_EVENT_TEMPLATE_ARGLIST typename Type1, typename Type2, typename Type3, typename Type4, typename Type5, typename Type6, typename Type7, typename Type8 
	#define GX_EVENT_TEMPLATE_PARAMS Type1,Type2,Type3,Type4,Type5,Type6,Type7,Type8
	#define GX_EVENT_TEMPLATE_FULL_PARAMS Type1,Type2,Type3,Type4,Type5,Type6,Type7,Type8,NullClass,NullClass
	#define GX_EVENT_ARGLIST Type1 param1, Type2 param2, Type3 param3, Type4 param4, Type5 param5, Type6 param6, Type7 param7, Type8 param8
	#define GX_EVENT_PARAMS	param1, param2, param3, param4, param5, param6, param7, param8

	#include "Events_Element.h"

	/***************************************************************************
					------------------   9   -------------------

					这是对具有9个参数的回调函数的相关模板类定义

					------------------   9   -------------------
	****************************************************************************/
	#undef GX_EVENT_TEMPLATE_ARGLIST
	#undef GX_EVENT_TEMPLATE_FULL_PARAMS
	#undef GX_EVENT_TEMPLATE_PARAMS
	#undef GX_EVENT_ARGLIST
	#undef GX_EVENT_PARAMS

	#define GX_EVENT_TEMPLATE_ARGLIST typename Type1, typename Type2, typename Type3, typename Type4, typename Type5, typename Type6, typename Type7, typename Type8, typename Type9
	#define GX_EVENT_TEMPLATE_PARAMS Type1,Type2,Type3,Type4,Type5,Type6,Type7,Type8,Type9
	#define GX_EVENT_TEMPLATE_FULL_PARAMS Type1,Type2,Type3,Type4,Type5,Type6,Type7,Type8,Type9,NullClass
	#define GX_EVENT_ARGLIST Type1 param1, Type2 param2, Type3 param3, Type4 param4, Type5 param5, Type6 param6, Type7 param7, Type8 param8, Type8 param9
	#define GX_EVENT_PARAMS	param1, param2, param3, param4, param5, param6, param7, param8, param9

	#include "Events_Element.h"

	}
}

#endif