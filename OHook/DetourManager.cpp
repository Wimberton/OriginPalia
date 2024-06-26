#include "DetourManager.h"
#include "Configuration.h"
#include "PaliaOverlay.h"
#include "SDKExt.h"
#include "Utils.h"

#include <algorithm>
#include "format"
#include "functional"
#include <sstream>
#include <fstream>

#include "Detours/AimDetours.h"
#include "Detours/FishingDetours.h"
#include "Detours/HUDDetours.h"
#include "Detours/MovementDetours.h"
#include "Detours/TeleportDetours.h"
#include "Detours/WidgetDetours.h"

#include <SDK/Palia_parameters.hpp>

using namespace SDK;

// Cache

void DetourManager::ClearActorCache(PaliaOverlay* Overlay) {
    const UWorld* World = GetWorld();

    const auto GameplayStatics = static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject);
    if (!GameplayStatics)
        return;

    // Clear cache on level change
    if (Overlay->CurrentLevel != World->PersistentLevel) {
        Overlay->CachedActors.clear();
        Overlay->CurrentLevel = World->PersistentLevel;
        Overlay->CurrentMap = GameplayStatics->GetCurrentLevelName(World, false).ToString();
    }
}

void DetourManager::ManageActorCache(PaliaOverlay* Overlay) {
    const UWorld* World = GetWorld();

    const auto GameplayStatics = static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject);
    if (!GameplayStatics)
        return;

    if (const double WorldTime = GameplayStatics->GetTimeSeconds(World); abs(WorldTime - Overlay->LastCachedTime) > 0.1) {
        Overlay->LastCachedTime = WorldTime;
        Overlay->ProcessActors(Overlay->ActorStep);

        Overlay->ActorStep++;
        if (Overlay->ActorStep >= static_cast<int>(EType::MAX)) {
            Overlay->ActorStep = 0;
        }
    }
}

// [Fun]

std::chrono::steady_clock::time_point lastAFKPing = std::chrono::steady_clock::now();
inline void Func_DoAntiAfk() {
    if (!Configuration::bEnableAntiAfk)
        return;

    const auto ValeriaController = GetValeriaController();
    if (!ValeriaController)
        return;

    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::minutes>(currentTime - lastAFKPing);

    if (elapsedTime.count() >= 3) { // minutes
        ValeriaController->Client_InactivityHeartbeat();
        ValeriaController->RpcServer_NotifyInactivityInterrupted();
        lastAFKPing = currentTime;
    }
}

// [Placement]

inline void Func_DoPlaceAnywhere() {
    if (!Configuration::bPlaceAnywhere)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    UPlacementComponent* PlacementComponent = ValeriaCharacter->GetPlacement();
    if (!PlacementComponent || !PlacementComponent->IsValidLowLevel() || PlacementComponent->IsDefaultObject())
        return;

    PlacementComponent->CanPlaceHere = true;
}

// [Minigames]

inline void Func_DoCompleteMinigame() {
    if (!Configuration::bEnableMinigameSkip)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    const auto MinigameComponent = ValeriaCharacter->MinigameQTE;
    if (!MinigameComponent || !MinigameComponent->IsValidLowLevel() || MinigameComponent->IsDefaultObject())
        return;

    if (MinigameComponent->IsPlaying()) {
        MinigameComponent->RpcServer_ChangeState(EMinigameState::Success);
    }
}

// [Misc Mods]

void Func_DoGatherActors(const PaliaOverlay* Overlay) {
    if (!Configuration::bEnableAutoGather)
        return;

    for (auto& cachedActors : Overlay->CachedActors) {
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
                    FRotator CharacterRotation = gatherableActor->K2_GetActorRotation();
                    FVector aGatherableLocation = gatherableActor->K2_GetActorLocation();
                    FRotator aGatherableRotation = gatherableActor->K2_GetActorRotation();
                    
                    float DistanceToaGatherableActor = CharacterLocation.GetDistanceToInMeters(aGatherableLocation);

                    if (!Overlay->TeleportToGatherables && DistanceToaGatherableActor > 0.75f && DistanceToaGatherableActor < 8.0f) {

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
                if (!Overlay->TeleportToGatherables && gatherableInteractable && gatherableInteractable->IsValidLowLevel() && !gatherableInteractable->IsDefaultObject()) {
                   
                    FVector CharacterLocation = ValeriaCharacter->K2_GetActorLocation();
                    FRotator CharacterRotation = lootActor->K2_GetActorRotation();
                    FVector aLootLocation = lootActor->K2_GetActorLocation();
                    FRotator aLootRotation = lootActor->K2_GetActorRotation();
                   
                    float DistanceToaLootActor = CharacterLocation.GetDistanceToInMeters(aLootLocation);

                    if (DistanceToaLootActor > 0.75f && DistanceToaLootActor < 8.0f) {
                        
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

void Func_DoPControllerMods() {
    const auto World = GetWorld();
    if (!World)
        return;

    static bool lastState = Configuration::bEnableInteractionMods;
    static ULevel* lastLevel = nullptr;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    auto PController = ValeriaCharacter->GetValeriaPlayerController();
    if (!PController || !PController->IsValidLowLevel() || PController->IsDefaultObject())
        return;

    if (Configuration::bEnableInteractionMods != lastState || World->PersistentLevel != lastLevel) {
        if (Configuration::bEnableInteractionMods) {
            PController->InteractableSearchRadius = Configuration::InteractionRadius;
            PController->InteractableSearchDistanceAhead = Configuration::InteractionRadius;
        }
        else {
            PController->InteractableSearchRadius = 250.0f;
            PController->InteractableSearchDistanceAhead = 250.0f;
        }

        lastState = Configuration::bEnableInteractionMods;
        lastLevel = World->PersistentLevel;
    }
}

void Func_DoMakeVanish() {
    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    UMeshComponent* MeshComponent = ValeriaCharacter->Mesh;
    if (MeshComponent) {
        if (Configuration::bEnableVanishMode) {
            MeshComponent->SetVisibility(false, true);
            MeshComponent->SetIsReplicated(true);
        }
        else {
            MeshComponent->SetVisibility(true, true);
            MeshComponent->SetIsReplicated(true);
        }

        ValeriaCharacter->ForceNetUpdate();
    }
}

inline void Func_NoSnitches() {
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

void DetourManager::Func_DoOpenGuildStore(PaliaOverlay* Overlay, const AValeriaCharacter* ValeriaCharacter, int storeId) {
    if (!IsActorValid(ValeriaCharacter))
        return;

    if (Overlay->GameplayUIManager == nullptr) {
        WidgetDetours::Func_DoGrabGameplayUIWidget(Overlay);
        return;
    }

    auto StoreComponent = ValeriaCharacter->StoreComponent;
    if (!StoreComponent || !StoreComponent->IsValidLowLevel())
        return;

    StoreComponent->Client_SetVillagerStore(storeId);
    StoreComponent->Client_OpenStore();
    Overlay->GameplayUIManager->Implementation_OpenVillagerStore(storeId);
}

void DetourManager::Func_DoOpenConfigStore(PaliaOverlay* Overlay, const AValeriaCharacter* ValeriaCharacter, std::string storePath) {
    if (!IsActorValid(ValeriaCharacter))
        return;

    if (Overlay->GameplayUIManager == nullptr) {
        WidgetDetours::Func_DoGrabGameplayUIWidget(Overlay);
        return;
    }

    auto StoreComponent = ValeriaCharacter->StoreComponent;
    if (!StoreComponent || !StoreComponent->IsValidLowLevel())
        return;

    auto StoreConfig = static_cast<UVillagerStoreConfig*>(LoadAssetPtrFromPath(storePath));
    if (!StoreConfig) return;

    StoreComponent->Client_SetStoreByConfig(StoreConfig);
    StoreComponent->Client_OpenStore();
    Overlay->GameplayUIManager->Implementation_OpenStore(StoreConfig);
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
    const auto Overlay = dynamic_cast<PaliaOverlay*>(OverlayBase::Instance);
    const auto fn = Function->GetFullName();
    invocations.insert(fn);

    // PlayerTick
    if (fn == "Function Engine.Actor.ReceiveTick") {
        FishingDetours::Func_DoFastAutoFishing(Overlay);
        MovementDetours::Func_DoNoClip(Overlay);
        MovementDetours::Func_DoPersistentMovement();
        
        Func_DoAntiAfk();

        // GameplayUIWidget
        WidgetDetours::Func_DoGrabGameplayUIWidget(Overlay);
    }
    else if (fn == "Function Engine.Actor.ReceiveBeginPlay") {
        WidgetDetours::RefreshWidgets(Overlay);
    }
    // HUD
    else if (fn == "Function Engine.HUD.ReceiveDrawHUD") {
        HUDDetours::Func_DoESP(Overlay, reinterpret_cast<const AHUD*>(Class));
        HUDDetours::Func_DoFOVCircle(Overlay, reinterpret_cast<const AHUD*>(Class));
        AimDetours::Func_DoInteliAim(Overlay);
        AimDetours::Func_DoLegacyAim(Overlay);
        TeleportDetours::Func_DoTeleportToTargeted(Overlay);

        Func_NoSnitches();
        Func_DoGatherActors(Overlay);
        Func_DoMakeVanish();
        Func_DoPControllerMods();
        Func_DoPlaceAnywhere();
        Func_DoCompleteMinigame();
    }
    // Fishing Capture/Override
    else if (fn == "Function Palia.FishingComponent.RpcServer_SelectLoot") {
        FishingDetours::Func_DoFishingCaptureOverride(Overlay, Params);
    }
    // Fishing Instant Catch
    else if (fn == "Function Palia.FishingComponent.RpcClient_StartFishingAt_Deprecated") {
        FishingDetours::Func_DoInstantCatch();
    }
    // Fishing End (Perfect/Durability/PlayerHelp)
    else if (fn == "Function Palia.FishingComponent.RpcServer_EndFishing") {
        FishingDetours::EndFishingDetoured(Overlay, Params);
    }
    // Fishing Cleanup (Sell/Discard/Move)
    else if (fn == "Function Palia.FishingComponent.RpcClient_FishCaught") {
        FishingDetours::Func_DoFishingCleanup();
    }
    // Waypoint Teleport (InGame)
    else if (fn == "Function Palia.TrackingComponent.RpcServer_SetUserMarker") {
        TeleportDetours::Func_DoTeleportToWaypoint(Params);
    }
    // Waypoint Teleport (WorldMap)
    else if (fn == "Function Palia.TrackingComponent.RpcClient_SetUserMarkerViaWorldMap") {
        TeleportDetours::Func_DoTeleportToWaypoint(Params);
    }
    // Silent Aim
    else if (fn == "Function Palia.ProjectileFiringComponent.RpcServer_FireProjectile") {
        AimDetours::Func_DoSilentAim(Overlay, Params);
    }
    // ??
    else if (fn == "Function Palia.ValeriaClientPriMovementComponent.RpcServer_SendMovement") {
        static_cast<Params::ValeriaClientPriMovementComponent_RpcServer_SendMovement*>(Params)->MoveInfo.TargetVelocity.X = 0;
        static_cast<Params::ValeriaClientPriMovementComponent_RpcServer_SendMovement*>(Params)->MoveInfo.TargetVelocity.Y = 0;
    }
    
    if (OriginalProcessEvent) {
        OriginalProcessEvent(Class, Function, Params);
    }
}

// Essentials

void DetourManager::SetupDetour(void* Instance, void (*DetourFunc)(const UObject*, const UFunction*, void*)) {
    if(Instance == nullptr) return;
    
    const void** Vtable = *static_cast<const void***>(Instance);

    DWORD OldProtection;
    VirtualProtect(Vtable, sizeof(DWORD) * 1024, PAGE_EXECUTE_READWRITE, &OldProtection);

    const int32_t Idx = Offsets::ProcessEventIdx;
    OriginalProcessEvent = reinterpret_cast<void(*)(const UObject*, const UFunction*, void*)>(reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr)) + Offsets::ProcessEvent);
    Vtable[Idx] = DetourFunc;

    HookedClient = Instance;
    VirtualProtect(Vtable, sizeof(DWORD) * 1024, OldProtection, &OldProtection);
}

void DetourManager::SetupDetour(void* Instance) {
    SetupDetour(Instance, &DetourManager::ProcessEventDetour);
}

void DetourManager::ProcessEventDetourCallback(const UObject* Class, const UFunction* Function, void* Params, const DetourManager* manager) {
    manager->ProcessEventDetour(Class, Function, Params);
}