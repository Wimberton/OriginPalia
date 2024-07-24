#include "HookManager.h"
#include "BaseHook.h"

HookManager& HookManager::Instance()
{
	static HookManager Instance;
	return Instance;
}

void HookManager::AddHook(BaseHook* InHook)
{
	_hooks.insert(InHook);
}

void HookManager::RemoveHook(BaseHook* InHook)
{
	auto it = _hooks.find(InHook);
	if (it != _hooks.end()) {
		delete InHook;
		_hooks.erase(it);
	}
}

void HookManager::UnhookAll() {
	for (auto hook : _hooks) {
		hook->UnhookAll();
	}
	_hooks.clear();
}

HookManager::HookManager()
{
}

HookManager::~HookManager()
{
	for (auto& h : _hooks)
		delete h;
}