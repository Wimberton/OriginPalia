#pragma once
#include <utility>
#include <vector>
class BaseHook
{
protected:
	BaseHook(const BaseHook&) = delete;
	BaseHook(BaseHook&&) = delete;
	BaseHook& operator =(const BaseHook&) = delete;
	BaseHook& operator =(BaseHook&&) = delete;

public:
	BaseHook();
	virtual ~BaseHook();

	void BeginHook();
	void EndHook();
	void UnhookAll();

	virtual const char* GetLibName() const;
	void HookFunc(std::pair<void**, void*> hook);

	template<typename T>
	void HookFuncs(std::pair<T*, T> funcs)
	{
		HookFunc(funcs);
	}

	template<typename T, typename ...Args>
	void HookFuncs(std::pair<T*, T> funcs, Args... args)
	{
		HookFunc(funcs);
		HookFuncs(args...);
	}
protected:
	std::vector<std::pair<void**, void*>> _hooked_funcs;

	void* _library;


};