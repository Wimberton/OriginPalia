#include "FishingDetours.h"
#include "Configuration.h"
#include "PaliaOverlay.h"
#include "SDKExt.h"
#include "Utils.h"

#include <algorithm>
#include "functional"
#include <sstream>
#include <fstream>

#include <SDK/Palia_parameters.hpp>

using namespace SDK;

inline void ToggleFishingDelays(const bool RemoveDelays) {
    auto ValeriaController = GetValeriaController();
    if (!ValeriaController)
        return;
    
    UValeriaGameInstance* ValeriaGameInstance = ValeriaController->GameInst;
    if (!ValeriaGameInstance || !ValeriaGameInstance->IsValidLowLevel() || ValeriaGameInstance->IsDefaultObject()) {
        return;
    }

    auto& CastSettings = ValeriaGameInstance->Configs.Globals.Fishing->CastSettings;

    // Avoid continuously setting values if already set properly
    float newCastDelay = RemoveDelays ? 0.0f : 0.150f;
    if (std::abs(CastSettings.CastDelay - newCastDelay) < 0.0001f) {
        return;
    }

    auto& FishingSettings = ValeriaGameInstance->Configs.Globals.Fishing;
    auto& EndSettings = ValeriaGameInstance->Configs.Globals.Fishing->EndSettings;

    CastSettings.CastDelay = newCastDelay;
    CastSettings.MaxDistanceToCast = 1500.0f;
    CastSettings.MinDistanceToCast = RemoveDelays ? 1500.0f : 500.0f;
    CastSettings.LaunchOffset = RemoveDelays ? FVector{ 1500, 0, -300 } : FVector{};
    CastSettings.WindupSpeed = RemoveDelays ? FLT_MAX : 0.350f;

    FishingSettings->AfterFinishDestroyBobberWhenAtDistanceToRod = RemoveDelays ? FLT_MAX : 50.0;
    FishingSettings->FishingFinishReelInSpeed = RemoveDelays ? FLT_MAX : 1600.0f;
    FishingSettings->TotalCelebrationDuration = RemoveDelays ? 0.0f : 4.0f;
    FishingSettings->OnBeginReelingInitialCooldown = RemoveDelays ? 0.0f : 1.0f;

    EndSettings.MaxTimeOfEndFishingCelebrate = RemoveDelays ? 0.0f : 4.5f;
    EndSettings.MaxTimeOfEndFishingDefault = RemoveDelays ? 0.0f : 2.0f;
    EndSettings.MaxTimeOfEndFishingEmptyHanded = RemoveDelays ? 0.0f : 2.2f;
    EndSettings.MaxTimeOfEndFishingFailure = RemoveDelays ? 0.0f : 1.75f;
}

void FishingDetours::Func_DoFastAutoFishing(const PaliaOverlay* Overlay) {
    // Toggle values (Safe to leave looped)
    ToggleFishingDelays(Overlay->bEnableAutoFishing);

    if (!Overlay->bEnableAutoFishing)
        return;

    if (Configuration::bRequireClickFishing ? !Overlay->ShowOverlay() && IsGameWindowActive() && IsKeyHeld(VK_LBUTTON) : true) {
        auto ValeriaCharacter = GetValeriaCharacter();
        if (!ValeriaCharacter)
            return;

        // Instant Catch
        auto FishingComponent = ValeriaCharacter->GetFishing();
        if (FishingComponent && FishingComponent->IsValidLowLevel() && !FishingComponent->IsDefaultObject()) {
            if (static_cast<EFishingState_NEW>(FishingComponent->GetFishingState()) == EFishingState_NEW::Bite) {
                FFishingEndContext Context;
                FishingComponent->RpcServer_EndFishing(Context);
                FishingComponent->SetFishingState(EFishingState_OLD::None);
            }
        }

        if (!ValeriaCharacter->GetEquippedItem().ItemType->IsFishingRod())
            return;

        // Cast the rod
        ValeriaCharacter->ToolPrimaryActionPressed();
        ValeriaCharacter->ToolPrimaryActionReleased();
    }
}

void FishingDetours::Func_DoInstantCatch() {
    if (!Configuration::bFishingInstantCatch)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    UFishingComponent* FishingComponent = ValeriaCharacter->GetFishing();
    if (!FishingComponent || !FishingComponent->IsValidLowLevel() || FishingComponent->IsDefaultObject())
        return;

    if (static_cast<EFishingState_NEW>(FishingComponent->GetFishingState()) == EFishingState_NEW::Bite) {
        FFishingEndContext Context;
        FishingComponent->RpcServer_EndFishing(Context);
        FishingComponent->SetFishingState(EFishingState_OLD::None);
    }
}

int fishingFlushCounter = 0;

void FishingDetours::Func_DoFishingCleanup() {
    // Avoid doing extra work
    if (!Configuration::bFishingSell && !Configuration::bFishingDiscard && !Configuration::bFishingOpenStoreWaterlogged) {
        return;
    }

    const auto ValeriaController = GetValeriaController();
    if (!ValeriaController)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    UVillagerStoreComponent* StoreComponent = ValeriaCharacter->StoreComponent;
    if (!StoreComponent || !StoreComponent->IsValidLowLevel() || StoreComponent->IsDefaultObject()) {
        return;
    }

    const UInventoryComponent* InventoryComponent = ValeriaCharacter->GetInventory();
    if (!InventoryComponent || !InventoryComponent->IsValidLowLevel() || InventoryComponent->IsDefaultObject()) {
        return;
    }

    // Sell / Discard / Storage
    for (int BagIndex = 0; BagIndex < InventoryComponent->Bags.Num(); BagIndex++) {
        for (int SlotIndex = 0; SlotIndex < 8; SlotIndex++) {
            FBagSlotLocation Slot{ BagIndex, SlotIndex };
            FValeriaItem Item = InventoryComponent->GetItemAt(Slot);

            if (Configuration::bFishingSell && Item.ItemType->Category == EItemCategory::Fish && StoreComponent) {
                if (!StoreComponent->StoreCanBuyItem(Slot)) {
                    StoreComponent->Client_SetVillagerStore(2);
                    StoreComponent->Client_OpenStore();
                }

                StoreComponent->RpcServer_SellItem(Slot, 10);
            }
            else if (Configuration::bFishingDiscard && Item.ItemType->Category == EItemCategory::Junk) {
                // Don't ever discard more than the amount of the stack
                ValeriaController->DiscardItem(Slot, Item.Amount);
            }
            else if (Item.ItemType->PersistId == 2810) { // Waterlogged Chest
                if (!Configuration::bFishingOpenStoreWaterlogged) {
                    // Don't ever discard more than the amount of the stack
                    ValeriaController->DiscardItem(Slot, Item.Amount);
                }
                else {
                    ValeriaController->ConsumeItem(Slot);
                }
            }
            else if (Configuration::bFishingOpenStoreWaterlogged && Item.ItemType->Name.ToString().find("DA_ItemType_Decor_Makeshift_") != std::string::npos) {
                ValeriaController->MoveItemSlotToStorage(Slot, 1, EStoragePoolType::Primary);
            }
        }
    }

    fishingFlushCounter++;
    if (fishingFlushCounter >= 25) {
        if (APlayerController* PlayerController = GetPlayerController()) {
            PlayerController->ClientFlushLevelStreaming();
            PlayerController->ClientForceGarbageCollection();

            fishingFlushCounter = 0;
        }
    }
}

void FishingDetours::Func_DoFishingCaptureOverride(PaliaOverlay* Overlay, void* Params) {
    auto SelectLoot = static_cast<Params::FishingComponent_RpcServer_SelectLoot*>(Params);

    if (Overlay->bCaptureFishingSpot) {
        memcpy(&Overlay->sOverrideFishingSpot, &SelectLoot->RPCLootParams.WaterType_Deprecated, sizeof(FName));
        Overlay->bCaptureFishingSpot = false;
    }
    if (Overlay->bOverrideFishingSpot) {
        memcpy(&SelectLoot->RPCLootParams.WaterType_Deprecated, &Overlay->sOverrideFishingSpot, sizeof(FName));
    }
}

Params::FishingComponent_RpcServer_EndFishing* FishingDetours::EndFishingDetoured(const PaliaOverlay* Overlay, void* Params) {
    auto EndFishing = static_cast<Params::FishingComponent_RpcServer_EndFishing*>(Params);

    if (Configuration::bFishingInstantCatch || Overlay->bEnableAutoFishing) {
        EndFishing->Context.Result = EFishingMiniGameResult::Success;
    }

    if (Configuration::bFishingNoDurability) {
        EndFishing->Context.DurabilityReduction = 0;
    }

    EndFishing->Context.Perfect = Configuration::bFishingPerfectCatch ? true : Configuration::bFishingInstantCatch ? false : EndFishing->Context.Perfect;
    EndFishing->Context.SourceWaterBody = nullptr;
    EndFishing->Context.bUsedMultiplayerHelp = Configuration::bFishingMultiplayerHelp;
    EndFishing->Context.StartRodHealth = 100.0f;
    EndFishing->Context.EndRodHealth = 100.0f;
    EndFishing->Context.StartFishHealth = 100.0f;
    EndFishing->Context.EndFishHealth = 100.0f;
    return EndFishing;
}