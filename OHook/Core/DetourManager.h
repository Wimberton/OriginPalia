#pragma once

#include <SDK.hpp>

// Comment out this line to disable Supporter Features
// #define ENABLE_SUPPORTER_FEATURES

using namespace SDK;

class DetourManager final {
public:
    using ProcessEventCallback = void(const UObject* Context, UFunction* Function, void* Parms);
    using FunctionCallback = void(void* Context, void* TheStack, void* Result);

    static void ProcessEventDetour(const UObject* Context, UFunction* Function, void* Parms);
    static void FunctionDetour(void* Context, void* TheStack, void* Result);

    static void AddInstance(UObject* Instance);
    static void AddFunctionListener(UFunction* Function, ProcessEventCallback* PreCallback, ProcessEventCallback* PostCallback);
    static void AddFunctionListener(const std::string& FunctionName, ProcessEventCallback* PreCallback, ProcessEventCallback* PostCallback);

    static void AddFunctionDetour(UFunction* Function, FunctionCallback* PreCallback, FunctionCallback* PostCallback);
    static void AddFunctionDetour(const std::string& FunctionName, FunctionCallback* PreCallback, FunctionCallback* PostCallback);

    static void RemoveInstances();
    static void RemoveFunctionDetours();

private:
    static inline std::unordered_map<UFunction*, UFunction::FNativeFuncPtr>FunctionDetours = {};
    static inline std::unordered_map<UObject*, ProcessEventCallback*>InstanceProcessEventDetours = {};
    static inline std::unordered_map<UFunction*, std::vector<ProcessEventCallback*>>PreFunctionListeners = {};
    static inline std::unordered_map<UFunction*, std::vector<ProcessEventCallback*>>PostFunctionListeners = {};
    static inline std::unordered_map<UFunction*, std::vector<FunctionCallback*>>PreFunctionCallbacks = {};
    static inline std::unordered_map<UFunction*, std::vector<FunctionCallback*>>PostFunctionCallbacks = {};

    static const void* VTableHook(const void** vtable, const int index, const void* hook);
    static ProcessEventCallback* MainProcessEvent;
};

inline DetourManager::ProcessEventCallback* DetourManager::MainProcessEvent = nullptr;