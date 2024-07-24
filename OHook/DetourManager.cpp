#include "DetourManager.h"

#include <thread>

#include "Configuration.h"
#include "PaliaOverlay.h"
#include "ActorHandler.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"

#include "functional"

#include "Detours/AimDetours.h"
#include "Detours/FishingDetours.h"
#include "Detours/HUDDetours.h"
#include "Detours/MovementDetours.h"
#include "Detours/TeleportDetours.h"
#include "Detours/WidgetDetours.h"
#include "Detours/HousingDetours.h"
#include "Detours/CookingDetours.h"
#include "Detours/PlayerTweaksDetours.h"
#include "Detours/BlackMarketDetours.h"

// Supporter Features 
#ifdef ENABLE_SUPPORTER_FEATURES
    #include "Detours/Supporters/OutfitsDetours.h"
    #include "Detours/Supporters/FastToolsDetours.h"
#endif

#include <SDK/Palia_parameters.hpp>

using namespace SDK;

#define FUNCTION_DETOUR(functionPtr, functionName)              \
if (functionPtr == nullptr) {                                   \
    functionPtr = UObject::FindObject<UFunction>(functionName); \
}

// Init Function Detours
void DetourManager::InitFunctions() {
    // Current player stuff
    FUNCTION_DETOUR(Actor_ReceiveTick, "Function Engine.Actor.ReceiveTick")
    FUNCTION_DETOUR(Actor_ReceiveBeginPlay, "Function Engine.Actor.ReceiveBeginPlay")
    FUNCTION_DETOUR(Actor_ReceiveEndPlay, "Function Engine.Actor.ReceiveEndPlay")
    FUNCTION_DETOUR(PlayerController_ClientTravelInternal, "Function Engine.PlayerController.ClientTravelInternal")
    FUNCTION_DETOUR(PlayerController_ClientRestart, "Function Engine.PlayerController.ClientRestart")
    FUNCTION_DETOUR(HUD_ReceiveDrawHUD, "Function Engine.HUD.ReceiveDrawHUD")

    // Fishing component
    FUNCTION_DETOUR(FishingComponent_RpcServer_SelectLoot, "Function Palia.FishingComponent.RpcServer_SelectLoot")
    FUNCTION_DETOUR(FishingComponent_RpcClient_StartFishingAt_Deprecated, "Function Palia.FishingComponent.RpcClient_StartFishingAt_Deprecated")
    FUNCTION_DETOUR(FishingComponent_RpcServer_EndFishing, "Function Palia.FishingComponent.RpcServer_EndFishing")
    FUNCTION_DETOUR(FishingComponent_RpcClient_FishCaught, "Function Palia.FishingComponent.RpcClient_FishCaught")

    // Tracking component
    FUNCTION_DETOUR(TrackingComponent_RpcServer_SetUserMarker, "Function Palia.TrackingComponent.RpcServer_SetUserMarker")
    FUNCTION_DETOUR(TrackingComponent_RpcClient_SetUserMarkerViaWorldMap, "Function Palia.TrackingComponent.RpcClient_SetUserMarkerViaWorldMap")

    // Projectile firing component
    FUNCTION_DETOUR(ProjectileFiringComponent_RpcServer_FireProjectile, "Function Palia.ProjectileFiringComponent.RpcServer_FireProjectile")

    // Valeria client movement component
    FUNCTION_DETOUR(ValeriaClientPriMovementComponent_RpcServer_SendMovement, "Function Palia.ValeriaClientPriMovementComponent.RpcServer_SendMovement")
}
#undef FUNCTION_DETOUR

// [Requests Mods]

inline void Func_DoRequestMods() {
    if (!Configuration::bEnableRequestMods)
        return;

    auto World = GetWorld();
    if (!World)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    //Gift BTN Force
    TArray<UUserWidget*> FoundWidgets;
    TSubclassOf<UUserWidget> WidgetClass = UWBP_ListEntry_Request_CM_C::StaticClass();
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(World, &FoundWidgets, WidgetClass, false);

    for (auto Widget : FoundWidgets) {
        auto OPRequestModal = static_cast<UWBP_ListEntry_Request_CM_C*>(Widget);

        if (!OPRequestModal || !OPRequestModal->IsValidLowLevel() || OPRequestModal->IsDefaultObject() || !OPRequestModal->IsVisible())
            continue;

        bool Return = true;
        OPRequestModal->CanFulfillRequestNow(&Return);

        auto ConfirmButton = OPRequestModal->GiftButton;
        if (ConfirmButton && ConfirmButton->IsValidLowLevel() && !ConfirmButton->IsDefaultObject()) {
            ConfirmButton->SetIsEnabled(true);
        }
    }
}

// [Misc Mods]

bool TeleportToGatherables = false;
void Func_DoGatherActors() {
    if (!Configuration::bEnableAutoGather)
        return;

    for (auto& cachedActors : ActorHandler::GetActors()) {
        if (cachedActors.ActorType != EType::Forage && cachedActors.ActorType != EType::Loot)
            continue;

        const auto ValeriaCharacter = GetValeriaCharacter();
        if (!ValeriaCharacter)
            return;

        const auto ValeriaAnimComponent = ValeriaCharacter->GetAbilityAnimation();
        if (!ValeriaAnimComponent)
            return;

        auto ValeriaMovementComponent = ValeriaCharacter->GetValeriaCharacterMovementComponent();
        if (!ValeriaMovementComponent)
            return;

        switch (cachedActors.ActorType) {
        case EType::Forage: {
            if (!cachedActors.Actor || !IsActorValid(cachedActors.Actor))
                continue;

            auto gatherableActor = static_cast<ABP_ValeriaGatherable_C*>(cachedActors.Actor);
            if (gatherableActor && gatherableActor->IsValidLowLevel() && !gatherableActor->IsDefaultObject()) {
                gatherableActor->bForceNetAddressable = true;

                auto* gatherableInteractable = gatherableActor->Interactable;
                if (gatherableInteractable && gatherableInteractable->IsValidLowLevel() && !gatherableInteractable->IsDefaultObject()) {
                    
                    FVector CharacterLocation = ValeriaCharacter->K2_GetActorLocation();
                    FVector aGatherableLocation = gatherableActor->K2_GetActorLocation();
                    
                    float DistanceToaGatherableActor = CharacterLocation.GetDistanceToInMeters(aGatherableLocation);

                    if (!TeleportToGatherables && DistanceToaGatherableActor > 0.75f && DistanceToaGatherableActor < 8.0f) {
                        gatherableInteractable->bSendTelemetryEventOnServerInteract = false;
                        
                        ValeriaCharacter->Interactor->HighlightInteractable(gatherableInteractable);
                        
                        FInteractEventParams Params{
                            .Type = EInteractEventType::Pressed,
                            .Index = EInteractEventIndex::Primary,
                            .InputType = EInputType::Keyboard,
                            .Handled = true
                        };

                        ValeriaCharacter->Interactor->InteractWithHighlighted(Params);
                        ValeriaAnimComponent->StopAll(0.0f);
                    }
                }
            }
        }
        break;
        default:
            if (!cachedActors.Actor || !IsActorValid(cachedActors.Actor))
                continue;

            auto lootActor = static_cast<ABP_Loot_C*>(cachedActors.Actor);
            if (lootActor && lootActor->IsValidLowLevel() && !lootActor->IsDefaultObject()) {
                lootActor->bForceNetAddressable = true;

                auto* gatherableInteractable = lootActor->Interactable;
                if (!TeleportToGatherables && gatherableInteractable && gatherableInteractable->IsValidLowLevel() && !gatherableInteractable->IsDefaultObject()) {
                   
                    FVector CharacterLocation = ValeriaCharacter->K2_GetActorLocation();
                    FVector aLootLocation = lootActor->K2_GetActorLocation();
                   
                    float DistanceToaLootActor = CharacterLocation.GetDistanceToInMeters(aLootLocation);

                    if (DistanceToaLootActor > 0.75f && DistanceToaLootActor < 8.0f) {
                        gatherableInteractable->bSendTelemetryEventOnServerInteract = false;
                        
                        ValeriaCharacter->Interactor->HighlightInteractable(gatherableInteractable);
                        
                        FInteractEventParams Params{
                            .Type = EInteractEventType::Pressed,
                            .Index = EInteractEventIndex::Primary,
                            .InputType = EInputType::Keyboard,
                            .Handled = true
                        };

                        ValeriaCharacter->Interactor->InteractWithHighlighted(Params);
                        ValeriaAnimComponent->StopAll(0.0f);
                    }
                }
            }
            break;
        }
    }
}

void Func_NoSnitches() {
    UUserWidget* Widget = nullptr;
    WidgetDetours::FindWidget(UWBP_ReportPlayerModalDialog_CM_C::StaticClass(), &Widget);

    auto ReportWindow = static_cast<UWBP_ReportPlayerModalDialog_CM_C*>(Widget);
    if (!ReportWindow || !ReportWindow->IsValidLowLevel() || ReportWindow->IsDefaultObject() || !ReportWindow->IsVisible())
        return;

    auto ReportBtn = ReportWindow->Button_Submit;
    if (ReportBtn && ReportBtn->IsValidLowLevel() && !ReportBtn->IsDefaultObject()) {
        ReportBtn->SetText(UKismetTextLibrary::Conv_StringToText(L"No Snitching :)"));
        ReportBtn->SetEnabled(false);
    }
}

void Func_ActorTick() {
    auto World = GetWorld();
    if (DetourManager::CurrentLevel != World->PersistentLevel) {
        DetourManager::CurrentLevel = World->PersistentLevel;
        ActorHandler::LevelChange(DetourManager::CurrentLevel);
        DetourManager::CurrentMap = UGameplayStatics::GetCurrentLevelName(World, false).ToString();

        //OutfitsDetours::AppliedOutfit = false; 
        //OutfitsDetours::AppliedVariants = false;
        PlayerTweaksDetours::ResetTweaks();

        if (DetourManager::CurrentMap == "Village_Black_Market_Root")
            BlackMarketDetours::SetupDetours();
    }
}

void DetourManager::Func_DoOpenGuildStore(const AValeriaCharacter* ValeriaCharacter, int storeId) {
    if (!IsActorValid(ValeriaCharacter))
        return;

    if (WidgetDetours::GameplayUIManager == nullptr) {
        WidgetDetours::Func_DoGrabGameplayUIWidget();
        return;
    }

    auto StoreComponent = ValeriaCharacter->StoreComponent;
    if (!StoreComponent || !StoreComponent->IsValidLowLevel())
        return;

    StoreComponent->Client_SetVillagerStore(storeId);
    StoreComponent->Client_OpenStore();
    WidgetDetours::GameplayUIManager->Implementation_OpenVillagerStore(storeId);
}

void DetourManager::Func_DoOpenConfigStore() {
    auto ValeriaCharacter = GetValeriaCharacter();

    if (WidgetDetours::GameplayUIManager == nullptr) {
        WidgetDetours::Func_DoGrabGameplayUIWidget();
        return;
    }

    auto StoreComponent = ValeriaCharacter->StoreComponent;
    if (!StoreComponent || !StoreComponent->IsValidLowLevel())
        return;

    auto StoreConfig = static_cast<UVillagerStoreConfig*>(LoadAssetPtrFromPath(BlackMarketDetours::SelectedStoreConfig));
    if (!StoreConfig) return;

    // attempt to change time rules
    StoreConfig->Requirements.TimeOfDay.Periods = 15;
    // doesn't work

    StoreComponent->Client_SetStoreByConfig(StoreConfig);
    StoreComponent->Client_OpenStore();
    WidgetDetours::GameplayUIManager->Implementation_OpenStore(StoreConfig);
}

void DetourManager::Func_SellItem(FBagSlotLocation bag, int quantityToSell, UVillagerStoreComponent* StoreComponent, const AValeriaCharacter* ValeriaCharacter, bool *cannotSell) {
    static int stores[] = { 2, 3, 6, 7, 10, 12, 8806, 19928 };

    if (StoreComponent->StoreCanBuyItem(bag)) {
        ValeriaCharacter->StoreComponent->RpcServer_SellItem(bag, quantityToSell);
    }
    else {
        int i = 0;
        while (!StoreComponent->StoreCanBuyItem(bag) && (i < IM_ARRAYSIZE(stores))) {
            StoreComponent->Client_SetVillagerStore(stores[i]);
            StoreComponent->Client_OpenStore();
            if (auto StoreConfig = ValeriaCharacter->StoreComponent->StoreConfig; StoreConfig != nullptr)
                ValeriaCharacter->StoreComponent->RpcServer_OpenStore(stores[i], StoreConfig);
            ValeriaCharacter->StoreComponent->RpcServer_SellItem(bag, quantityToSell);
            i += 1;
            if (i == 8) {
                *cannotSell = true;
                break;
            }
        }
    }
}

// Detouring

void DetourManager::ProcessEventDetour(const UObject* Class, const UFunction* Function, void* Params) {
    const auto Overlay = dynamic_cast<PaliaOverlay*>(MenuBase::Instance);
    //const auto fn = Function->GetFullName();
    //invocations.insert(fn);
    
    // PlayerTick
    if (Function == Actor_ReceiveTick) {
        // GameplayUIWidget
        WidgetDetours::Func_DoGrabGameplayUIWidget();
        ActorHandler::Tick();
        
        // Player Tweaks
        PlayerTweaksDetours::Func_DoCharacterTweaks();
        MovementDetours::Func_DoNoClip();
        MovementDetours::Func_DoPersistentMovement();
        FishingDetours::Func_DoFastAutoFishing();
        
        Func_ActorTick();
        
//#ifdef ENABLE_SUPPORTER_FEATURES // Future-proofing
//        //OutfitsDetours::Func_ReapplyLoadouts(); For v3 outfits ~ Manual whitelisting of Entitlements
//#endif
    }
    else if (Function == Actor_ReceiveBeginPlay) {
        WidgetDetours::RefreshWidgets();
    }
    else if (Function == Actor_ReceiveEndPlay) {
    }

    // Level Change (Leave Level)
    else if (Function == PlayerController_ClientTravelInternal) {
        ActorHandler::ClientTravelInternal();
    }
    // Level Change (Join Level)
    else if (Function == PlayerController_ClientRestart) {
    }
    
    // HUD
    else if (Function == HUD_ReceiveDrawHUD) {
        HUDDetours::Func_DoESP(reinterpret_cast<const AHUD*>(Class));

        AimDetours::Func_DoInteliAim();
        AimDetours::Func_DoLegacyAim();
        TeleportDetours::Func_DoTeleportToTargeted();
        HousingDetours::Func_DoPlaceAnywhere();

//#ifdef ENABLE_SUPPORTER_FEATURES // Future-proofing
//        OutfitsDetours::Func_DoCosmetics(Overlay); // For v3 outfits ~ Manual whitelisting of Entitlements
//#endif

        Func_NoSnitches();
        Func_DoGatherActors();
        Func_DoRequestMods();

#ifdef ENABLE_SUPPORTER_FEATURES
        FastToolsDetours::Func_DoFastTools();
#endif
    }
    
    // Fishing Capture/Override
    else if (Function == FishingComponent_RpcServer_SelectLoot) {
        FishingDetours::Func_DoFishingCaptureOverride(Params);
    }
    // Fishing Instant Catch
    else if (Function == FishingComponent_RpcClient_StartFishingAt_Deprecated) {
        FishingDetours::Func_DoInstantCatch();
    }
    // Fishing End (Perfect/Durability/PlayerHelp)
    else if (Function == FishingComponent_RpcServer_EndFishing) {
        FishingDetours::EndFishingDetoured(Params);
    }
    // Fishing Cleanup (Sell/Discard/Move)
    else if (Function == FishingComponent_RpcClient_FishCaught) {
        FishingDetours::Func_DoFishingCleanup();
    }
    
    // Waypoint Teleport (InGame)
    else if (Function == TrackingComponent_RpcServer_SetUserMarker) {
        TeleportDetours::Func_DoTeleportToWaypoint(Params);
    }
    // Waypoint Teleport (WorldMap)
    else if (Function == TrackingComponent_RpcClient_SetUserMarkerViaWorldMap) {
        TeleportDetours::Func_DoTeleportToWaypoint(Params);
    }
    
    // Silent Aim
    else if (Function == ProjectileFiringComponent_RpcServer_FireProjectile) {
        AimDetours::Func_DoSilentAim(Params);
    }
    // Movement Bypass
    else if (Function == ValeriaClientPriMovementComponent_RpcServer_SendMovement) {
        auto sendMovement = static_cast<Params::ValeriaClientPriMovementComponent_RpcServer_SendMovement*>(Params);
        sendMovement->MoveInfo.TargetVelocity.X = 0;
        sendMovement->MoveInfo.TargetVelocity.Y = 0;
    }

	// Outfits, Confirm Button Handle(For v3 outfits ~Manual whitelisting of Entitlements)
//#ifdef ENABLE_SUPPORTER_FEATURES // Future-proofing
//	else if (Function == WBP_CharacterCustomizationScreen_CM_Button_Primary_Confirm) {
//		OutfitsDetours::Func_GetSpoofedOutfit(Overlay);
//	}
//#endif

    if (BlackMarketDetours::bShouldOpenConfigStore) {
        BlackMarketDetours::bShouldOpenConfigStore = false;
        Func_DoOpenConfigStore();
    }
    
    if (OriginalProcessEvent) {
        OriginalProcessEvent(Class, Function, Params);
    }
}

// Essentials

void DetourManager::SetupHooks() {
    if (!ActorHandler::bListenersAdded) {
        ActorHandler::SetupListeners();
    }
    // Small delay per attempt (Needed)
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    auto* ValeriaController = GetValeriaController();
    auto* ValeriaCharacter = GetValeriaCharacter();

    // Get Components
    std::unordered_map<std::string, std::function<void*()>> componentMap = {
        { "PlayerController", [&] { return ValeriaCharacter ? ValeriaCharacter->GetValeriaPlayerController() : nullptr; } },
        { "CharacterMovement", [&] { return ValeriaCharacter ? ValeriaCharacter->GetValeriaCharacterMovementComponent() : nullptr; } },
        { "Store", [&] { return ValeriaCharacter ? ValeriaCharacter->StoreComponent : nullptr; } },
        { "ItemUpgrade", [&] { return ValeriaCharacter ? ValeriaCharacter->GetItemUpgradeComponent() : nullptr; } },
        { "Inventory", [&] { return ValeriaCharacter ? ValeriaCharacter->GetInventory() : nullptr; } },
        { "Equipment", [&] { return ValeriaCharacter ? ValeriaCharacter->GetEquipment() : nullptr; } },
        { "MinigameQTE", [&] { return ValeriaCharacter ? ValeriaCharacter->MinigameQTE : nullptr; } },
        { "Firing", [&] { return ValeriaCharacter ? ValeriaCharacter->GetFiringComponent() : nullptr; } },
        { "Fishing", [&] { return ValeriaCharacter ? ValeriaCharacter->GetFishing() : nullptr; } },
        { "Placement", [&] { return ValeriaCharacter ? ValeriaCharacter->GetPlacement() : nullptr; } },
        { "Tracking", [&] { return ValeriaController ? ValeriaController->GetTrackingComponent() : nullptr; } },
        { "MyHUD", [&] { return ValeriaController ? ValeriaController->MyHUD : nullptr; } }
    };
    
    // Hook Components
    for (auto& [name, hook_info] : components) {
        if (hook_info.hooked)
            continue;

        auto it = componentMap.find(name);
        if (it != componentMap.end()) {
            void* hookComponent = it->second();
            
            if (hookComponent != nullptr) {
                hook_info.hooked = true;
                hook_info.component = hookComponent;
                SetupDetour(hookComponent);
            }
        }
    }

    // Retry hooking missed components
    for (auto& [name, hook_info] : components) {
        if (!hook_info.hooked) {
            SetupHooks();
            return;
        }
    }

    // Black Magic Detours

    // Supporter Detours
#ifdef ENABLE_SUPPORTER_FEATURES
    HousingDetours::IsBiomeUnlockedDetour();
    OutfitsDetours::HasEntitlementDetour();

    if (!HousingDetours::orig_IsBiomeUnlocked ||
        !OutfitsDetours::orig_HasEntitlement) {
        SetupHooks();
        return;
    }
#endif
    
    // Detours
    PlayerTweaksDetours::OnRep_LastCharacterRespecTimeDetour();
    CookingDetours::HasCookingDetour();
    if (!PlayerTweaksDetours::orig_OnRep_LastCharacterRespecTime ||
        !CookingDetours::orig_MinigameIsPlaying) {
        SetupHooks();
        return;
    }

    InitFunctions();
}

void DetourManager::SetupDetour(void* Instance, void (*DetourFunc)(const UObject*, const UFunction*, void*)) {
    if(!Instance || !DetourFunc) return;
    
    const void** Vtable = *static_cast<const void***>(Instance);

    DWORD OldProtection;

    const int32_t Idx = Offsets::ProcessEventIdx;
    VirtualProtect(&Vtable[Idx], sizeof(void*), PAGE_READWRITE, &OldProtection);

    OriginalProcessEvent = reinterpret_cast<void(*)(const UObject*, const UFunction*, void*)>(reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr)) + Offsets::ProcessEvent);
    Vtable[Idx] = DetourFunc;

    HookedClient = Instance;
    VirtualProtect(&Vtable[Idx], sizeof(void*), OldProtection, &OldProtection);
}

void DetourManager::SetupDetour(void* Instance) {
    SetupDetour(Instance, &DetourManager::ProcessEventDetour);
}

void DetourManager::ProcessEventDetourCallback(const UObject* Class, const UFunction* Function, void* Params, const DetourManager* manager) {
    manager->ProcessEventDetour(Class, Function, Params);
}


