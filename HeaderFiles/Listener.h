#pragma once

#include <vector>
#include <map>
#include <functional>
#include <iostream>

template<typename ReturnType, typename... Args>
class ListenerElement
{
	typedef ReturnType(*Func)(void* obj, Args... args);
public:
	ListenerElement(void* obj, Func function);
	ReturnType	operator()(Args... args) const;
	bool		operator==(const ListenerElement& other) const;

private:
	void*	_obj;
	Func	_callbackFunc;
};


template<typename ReturnType, typename... Args>
class Listener
{
	typedef ListenerElement<ReturnType, Args...> listenerElement;
public:
	Listener();
	void	Add(listenerElement element);
	void	Remove(listenerElement element);
	void	operator()(Args... args);
private:
	std::vector<listenerElement>	_functions;
};

template<typename Obj, typename ReturnType, typename... Args>
struct ListenerMaker
{
	template<ReturnType(Obj::*func)(Args...)>
	inline static ListenerElement<ReturnType, Args...>		Bind(Obj* obj);

	template<ReturnType(Obj::*func)(Args...)>
	static ReturnType								Invoke(void* obj, Args... args);
};

template<typename Obj, typename ReturnType, typename... Args>
ListenerMaker<Obj, ReturnType, Args...>	makeListener(ReturnType(Obj::*func)(Args... args))
{
	return ListenerMaker<Obj, ReturnType, Args...>();
}
#include "..\..\Source\Core\Listener.inl"

#define LISTENER(func, obj) (makeListener(func).Bind<func>(obj))


