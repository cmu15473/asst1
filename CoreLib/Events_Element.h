template <GX_EVENT_TEMPLATE_ARGLIST>
class EventHandler<GX_EVENT_TEMPLATE_FULL_PARAMS>
{
public:
	virtual void Invoke(GX_EVENT_ARGLIST) = 0;
	virtual void *GetOwner()=0;
	virtual void *GetHandler()=0;
	virtual EventHandler<GX_EVENT_TEMPLATE_FULL_PARAMS>* Clone()=0;
};

template <GX_EVENT_TEMPLATE_ARGLIST>
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
	EventHandler<GX_EVENT_TEMPLATE_PARAMS>* Clone()
	{
		return new EventFuncHandler<GX_EVENT_TEMPLATE_PARAMS>(Func);
	}
};

template <typename Class, GX_EVENT_TEMPLATE_ARGLIST>
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
		return (void *)(*(int *)&FHandler);
	}
	EventHandler<GX_EVENT_TEMPLATE_PARAMS> * Clone()
	{
		return new EventFuncOfObjHandler<Class,GX_EVENT_TEMPLATE_PARAMS>(FOwner, FHandler);
	}
};

template <GX_EVENT_TEMPLATE_ARGLIST>
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
			Handlers.Delete(id);
		}
	}
public:
	Event()
	{
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
	Event(Class * Owner, typename EventFuncOfObjHandler<Class,GX_EVENT_TEMPLATE_PARAMS>::FuncHandler handler)
	{
		Bind(Owner, handler);
	}
	Event(typename EventFuncHandler<GX_EVENT_TEMPLATE_PARAMS>::FuncHandler f)
	{
		Bind(f);
	}
	~Event()
	{
		for (int i=0; i<Handlers.Count(); i++)
			delete Handlers[i];
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
	bool Attached()
	{
		return (Handlers.Count()!=0);
	}
};