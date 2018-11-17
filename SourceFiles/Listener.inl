
template<typename ReturnType, typename... Args>
ListenerElement<ReturnType, Args...>::ListenerElement(void* obj, Func function)
	:_obj(obj), _callbackFunc(function)
{}

template<typename returnType, typename ... Args>
returnType ListenerElement<returnType, Args...>::operator()(Args... args) const
{
	return (*_callbackFunc)(_obj, args...);
}

template<typename ReturnType, typename ...Args>
inline bool ListenerElement<ReturnType, Args...>::operator==(const ListenerElement & other) const
{
	return (_callbackFunc == other._callbackFunc) && (_obj == other._obj);
}

template<typename Obj, typename ReturnType, typename ... Args>
template<ReturnType(Obj::* func)(Args...)>
inline ListenerElement<ReturnType, Args...> ListenerMaker<Obj, ReturnType, Args...>::Bind(Obj * obj)
{
	return ListenerElement<ReturnType, Args...>(obj, &ListenerMaker::Invoke<func>);
}

template<typename Obj, typename ReturnType, typename ...Args>
template<ReturnType(Obj::* func)(Args...)>
inline ReturnType ListenerMaker<Obj, ReturnType, Args...>::Invoke(void * obj, Args ...args)
{
	return (static_cast<Obj*>(obj)->*func)(args...);
}

template<typename ReturnType, typename... Args>
inline Listener<ReturnType, Args...>::Listener()
{
}

template<typename ReturnType, typename ...Args>
inline void Listener<ReturnType, Args...>::Add(listenerElement element)
{
	_functions.push_back(element);
}

template<typename ReturnType, typename ...Args>
inline void Listener<ReturnType, Args...>::Remove(listenerElement element)
{
	for (unsigned int i = 0; i < _functions.size(); ++i)
	{
		if (_functions[i] == element)
		{
			_functions.erase(_functions.begin() + i);
			return;
		}
	}
}

template<typename ReturnType, typename ...Args>
inline void Listener<ReturnType, Args...>::operator()(Args... args)
{
	for (unsigned int i = 0; i < _functions.size(); ++i)
	{
		_functions[i](args...);
	}
}
