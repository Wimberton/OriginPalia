#ifndef DETOUR_MANAGER_H
#define DETOUR_MANAGER_H

#pragma once

#include <map>
#include <unordered_set>
#include <SDK.hpp>

// Comment out this line to disable Supporter Features
//#define ENABLE_SUPPORTER_FEATURES

class PaliaOverlay;

using namespace SDK;

inline static void (__fastcall *OriginalProcessEvent)(const UObject*, const UFunction*, void*);
inline static std::unordered_set<std::string> invocations;
inline static void* HookedClient;
inline static UFont* Roboto;

static UFunction* Actor_ReceiveTick = nullptr;
static UFunction* Actor_ReceiveBeginPlay = nullptr;
static UFunction* Actor_ReceiveEndPlay = nullptr;
static UFunction* PlayerController_ClientRestart = nullptr;
static UFunction* PlayerController_ClientTravelInternal = nullptr;
static UFunction* HUD_ReceiveDrawHUD = nullptr;

static UFunction* FishingComponent_RpcServer_SelectLoot = nullptr;
static UFunction* FishingComponent_RpcClient_StartFishingAt_Deprecated = nullptr;
static UFunction* FishingComponent_RpcServer_EndFishing = nullptr;
static UFunction* FishingComponent_RpcClient_FishCaught = nullptr;

static UFunction* TrackingComponent_RpcServer_SetUserMarker = nullptr;
static UFunction* TrackingComponent_RpcClient_SetUserMarkerViaWorldMap = nullptr;

static UFunction* ProjectileFiringComponent_RpcServer_FireProjectile = nullptr;

static UFunction* ValeriaClientPriMovementComponent_RpcServer_SendMovement = nullptr;

struct GatherableActorInfo {
    std::string Name;
    FVector Location;
    bool IsGatherable;
};

struct HookInfo {
    void* component;
    bool hooked;
};

class DetourManager final {

public:
    DetourManager() = default;

    static void InitFunctions();

    static inline void ProcessEventDetourCallback(const UObject* Class, const UFunction* Function, void* Params, const DetourManager* manager);
    static inline void ProcessEventDetour(const UObject* Class, const UFunction* Function, void* Params);
    static void SetupHooks();
    static void SetupDetour(void* Instance, void (*DetourFunc)(const UObject*, const UFunction*, void*));
    static void SetupDetour(void* Instance);

    static void Func_DoOpenGuildStore(const AValeriaCharacter* ValeriaCharacter, int storeId);
    static void Func_DoOpenConfigStore();
    static void Func_SellItem(FBagSlotLocation bag, int quantityToSell, UVillagerStoreComponent* StoreComponent, const AValeriaCharacter* ValeriaCharacter, bool *cannotSell);

    static UFunction* WBP_CharacterCustomizationScreen_CM_Button_Primary_Confirm;
    static ULevel* CurrentLevel;
    static std::string CurrentMap;
    
private:
    static std::map<std::string, HookInfo> components;
    
};

inline std::map<std::string, HookInfo> DetourManager::components = {
    {"PlayerController", {nullptr, false}},
    {"CharacterMovement", {nullptr, false}},
    {"Store", {nullptr, false}},
    {"ItemUpgrade", {nullptr, false}},
    {"Inventory", {nullptr, false}},
    {"Equipment", {nullptr, false}},
    {"MinigameQTE", {nullptr, false}},
    {"Firing", {nullptr, false}},
    {"Fishing", {nullptr, false}},
    {"Placement", {nullptr, false}},
    {"Tracking", {nullptr, false}},
    {"MyHUD", {nullptr, false}}
};

inline ULevel* DetourManager::CurrentLevel = nullptr;
inline std::string DetourManager::CurrentMap = "Unknown";

inline UFunction* DetourManager::WBP_CharacterCustomizationScreen_CM_Button_Primary_Confirm = nullptr;

#endif