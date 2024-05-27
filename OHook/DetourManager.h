#ifndef DETOUR_MANAGER_H
#define DETOUR_MANAGER_H

#pragma once

#include "PaliaOverlay.h"
#include <unordered_set>

inline static void (__fastcall *OriginalProcessEvent)(const UObject*, const UFunction*, void*);
inline static std::unordered_set<std::string> invocations;
inline static void* HookedClient;
inline static UFont* Roboto;

class DetourManager final {

public:
    DetourManager() = default;

    static inline void ProcessEventDetourCallback(const UObject* Class, const UFunction* Function, void* Params, const DetourManager* manager);
    static inline void ProcessEventDetour(const UObject* Class, const UFunction* Function, void* Params);
    static void SetupDetour(void* Instance, void (*DetourFunc)(const UObject*, const UFunction*, void*));
    static void SetupDetour(void* Instance);
};

void SetupDetour(void* Instance);

#endif