#pragma once
#include <set>

class BaseHook;

class HookManager
{
	friend class BaseHook;

public:
	static HookManager& Instance();

	void AddHook(BaseHook* InHook);
	void RemoveHook(BaseHook* InHook);
	void UnhookAll();

protected:
	std::set<BaseHook*> _hooks;

	HookManager();
	virtual ~HookManager();
};