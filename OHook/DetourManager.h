#ifndef DETOUR_MANAGER_H
#define DETOUR_MANAGER_H

#pragma once

#include <unordered_set>
#include <SDK.hpp>

class PaliaOverlay;

using namespace SDK;

inline static void (__fastcall *OriginalProcessEvent)(const UObject*, const UFunction*, void*);
inline static std::unordered_set<std::string> invocations;
inline static void* HookedClient;
inline static UFont* Roboto;

struct GatherableActorInfo {
    std::string Name;
    FVector Location;
    bool IsGatherable;
};

class DetourManager final {

public:
    DetourManager() = default;

    static void ClearActorCache(PaliaOverlay* Overlay);
    static void ManageActorCache(PaliaOverlay* Overlay);

    static inline void ProcessEventDetourCallback(const UObject* Class, const UFunction* Function, void* Params, const DetourManager* manager);
    static inline void ProcessEventDetour(const UObject* Class, const UFunction* Function, void* Params);
    static void SetupDetour(void* Instance, void (*DetourFunc)(const UObject*, const UFunction*, void*));
    static void SetupDetour(void* Instance);

    static void Func_DoOpenGuildStore(PaliaOverlay* Overlay, const AValeriaCharacter* ValeriaCharacter, int storeId);
    static void Func_DoOpenConfigStore(PaliaOverlay* Overlay, const AValeriaCharacter* ValeriaCharacter, std::string storeConfig);
    static void Func_SellItem(FBagSlotLocation bag, int quantityToSell, UVillagerStoreComponent* StoreComponent, const AValeriaCharacter* ValeriaCharacter, bool *cannotSell);
};

#endif