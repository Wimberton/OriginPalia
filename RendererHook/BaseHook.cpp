#include "BaseHook.h"
#include <Windows.h>
#include <detours.h>
#include <algorithm>
#include <Windows/WindowsHook.h>

BaseHook::BaseHook() : _library(nullptr)
{
}

BaseHook::~BaseHook()
{
    UnhookAll();
}

void BaseHook::BeginHook()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
}

void BaseHook::EndHook()
{
    DetourTransactionCommit();
}

void BaseHook::UnhookAll()
{
    if (_hooked_funcs.size())
    {
        BeginHook();
        std::for_each(_hooked_funcs.begin(), _hooked_funcs.end(), [](std::pair<void**, void*>& hook) {
            DetourDetach(hook.first, hook.second);
            });
        EndHook();
        _hooked_funcs.clear();
    }

    WindowsHook::Instance()->ResetRenderState();
}

const char* BaseHook::GetLibName() const
{
    return "<no name>";
}

void BaseHook::HookFunc(std::pair<void**, void*> hook)
{
    if (DetourAttach(hook.first, hook.second) == 0)
        _hooked_funcs.emplace_back(hook);
}