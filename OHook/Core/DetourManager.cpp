#include "DetourManager.h"
#include "Misc/SDKExt.h"

#include <bit>

void DetourManager::ProcessEventDetour(const UObject* Context, UFunction* Function, void* Parms) {
    for (const auto& [first, second] : PreFunctionListeners) {
        if (Function == first) {
            for (const auto Callback : second) {
                Callback(Context, Function, Parms);
            }
        }
    }
    MainProcessEvent(Context, Function, Parms);
    for (const auto& [first, second] : PostFunctionListeners) {
        if (Function == first) {
            for (const auto Callback : second) {
                Callback(Context, Function, Parms);
            }
        }
    }
};

void DetourManager::AddInstance(UObject* Instance) {
    if (!Instance) return;
    if (InstanceProcessEventDetours.contains(Instance)) return;
    if (!MainProcessEvent) {
        MainProcessEvent = reinterpret_cast<ProcessEventCallback*>(reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr)) + Offsets::ProcessEvent);
    }
    std::cout << "~~~ Adding PE Detour: " << Instance->GetName() << " ~~~\n";
    const void** Vtable = *static_cast<const void***>(static_cast<void*>(Instance));
    auto OrigPtr = reinterpret_cast<ProcessEventCallback*>(VTableHook(Vtable, Offsets::ProcessEventIdx, static_cast<void*>(&ProcessEventDetour)));
    InstanceProcessEventDetours.emplace(Instance, OrigPtr);
};

const void* DetourManager::VTableHook(const void** vtable, const int index, const void* hook) {
	DWORD old_protect;
	VirtualProtect(&vtable[index], sizeof(void*), PAGE_READWRITE, &old_protect);
	const auto* orig = vtable[index];
	vtable[index] = hook;
	VirtualProtect(&vtable[index], sizeof(void*), old_protect, &old_protect);
	return orig;
};

void DetourManager::RemoveInstances() {
    for (auto [fst, snd] : InstanceProcessEventDetours) {
        const void** Vtable = *static_cast<const void***>(static_cast<void*>(fst));
        VTableHook(Vtable, Offsets::ProcessEventIdx, snd);
    }
    InstanceProcessEventDetours.clear();
}

void DetourManager::AddFunctionListener(UFunction* Function, ProcessEventCallback* PreCallback, ProcessEventCallback* PostCallback) {
    std::cout << "~~~ Adding Function Listener: " << Function->GetFullName() << "\n";
    if (PreCallback) {
        if (const auto Entry = PreFunctionListeners.find(Function); Entry != PreFunctionListeners.end()) {
            Entry->second.push_back(PreCallback); 
        } else {
            PreFunctionListeners.emplace(Function, std::vector<ProcessEventCallback*>{PreCallback});
        }
    }
    if (PostCallback) {
        if (const auto Entry = PostFunctionListeners.find(Function); Entry != PostFunctionListeners.end()) {
            Entry->second.push_back(PostCallback);
        } else {
            PostFunctionListeners.emplace(Function, std::vector<ProcessEventCallback*>{PostCallback});
        }
    }
};

void DetourManager::AddFunctionListener(const std::string& FunctionName, ProcessEventCallback* PreCallback, ProcessEventCallback* PostCallback) {
    if (UFunction* Function = UObject::FindObject<UFunction>(FunctionName, EClassCastFlags::Function)) {
        AddFunctionListener(Function, PreCallback, PostCallback);
    }
};

void DetourManager::FunctionDetour(void* Context, void* TheStack, void* Result) {
    const auto Stack = static_cast<SDKExt::FFrame*>(TheStack);
	const auto Function = Stack->Code ? *std::bit_cast<UFunction**>(&Stack->Code[0-sizeof(uint64)]) : Stack->CurrentNativeFunction;
    //std::cout << "~~~ Function Called: " << Function->GetName() << "\n";
    for (const auto& [fst, snd] : PreFunctionCallbacks) {
        if (Function == fst) {
            for (const auto Callback : snd) {
                //std::cout << "  Calling PreCallback\n";
                Callback(Context, TheStack, Result);
            }
        }
    }
    if (const auto FoundEntry = FunctionDetours.find(Function); FoundEntry != FunctionDetours.end()) {
        //std::cout << "    Running Original Function\n";
		FoundEntry->second(Context, TheStack, Result);
	}
	else {
        const auto UberFunc = UObject::FindObject<UFunction>("Function CoreUObject.Object.ExecuteUbergraph", EClassCastFlags::Function);
		UberFunc->ExecFunction(Context, TheStack, Result);
        //std::cout << "    Running UObject Ubergraph\n";
	}
    for (const auto& [fst, snd] : PostFunctionCallbacks) {
        if (Function == fst) {
            for (const auto Callback : snd) {
                //std::cout << "  Calling PostCallback\n";
                Callback(Context, TheStack, Result);
            }
        }
    }

}

void DetourManager::AddFunctionDetour(UFunction* Function, FunctionCallback* PreCallback, FunctionCallback* PostCallback) {
    if (!Function) return;
    if (Function->ExecFunction != &FunctionDetour) {
        std::cout << "~~~ Adding Function Detour: " << Function->GetName() << "\n";
        FunctionDetours.emplace(Function, Function->ExecFunction);
        Function->ExecFunction = &FunctionDetour;
    }
    if (PreCallback) {
        if (const auto Entry = PreFunctionCallbacks.find(Function); Entry != PreFunctionCallbacks.end()) {
            bool ShouldAdd = true;
            for (const auto ptr : Entry->second) {
                if (ptr == PreCallback) {
                    ShouldAdd = false;
                }
            }
            if (ShouldAdd)
                Entry->second.push_back(PreCallback); 
        } else {
            PreFunctionCallbacks.emplace(Function, std::vector<FunctionCallback*>{PreCallback});
        }
    }
    if (PostCallback) {
        if (const auto Entry = PostFunctionCallbacks.find(Function); Entry != PostFunctionCallbacks.end()) {
            bool ShouldAdd = true;
            for (const auto ptr : Entry->second) {
                if (ptr == PostCallback) {
                    ShouldAdd = false;
                }
            }
            if (ShouldAdd)
            Entry->second.push_back(PostCallback);
        } else {
            PostFunctionCallbacks.emplace(Function, std::vector<FunctionCallback*>{PostCallback});
        }
    }
};

void DetourManager::AddFunctionDetour(const std::string& FunctionName, FunctionCallback* PreCallback, FunctionCallback* PostCallback) {
    if (UFunction* Function = UObject::FindObject<UFunction>(FunctionName, EClassCastFlags::Function)) {
        AddFunctionDetour(Function, PreCallback, PostCallback);
    }
};

void DetourManager::RemoveFunctionDetours() {
    for (auto [fst, snd] : FunctionDetours) {
        fst->ExecFunction = snd;
    }
    FunctionDetours.clear();
}