#include "FishingDetours.h"
#include "Core/Configuration.h"
#include "Core/DetourManager.h"
#include "Overlay/PaliaOverlay.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"

#include "functional"
#include <sstream>
#include <fstream>

#include <SDK/Palia_parameters.hpp>

using namespace SDK;

void FishingDetours::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
    DetourManager::AddInstance(VC);
	DetourManager::AddFunctionListener("Function Engine.Actor.ReceiveTick", &Func_DoFastAutoFishing, nullptr);
    DetourManager::AddInstance(VC->GetFishing());
    DetourManager::AddFunctionListener("Function Palia.FishingComponent.RpcServer_SelectLoot", &Func_DoFishingCaptureOverride, nullptr);
    DetourManager::AddFunctionListener("Function Palia.FishingComponent.RpcClient_StartFishingAt_Deprecated", &Func_DoInstantCatch, nullptr);
    DetourManager::AddFunctionListener("Function Palia.FishingComponent.RpcServer_EndFishing", &Func_EndFishingDetoured, nullptr);
    DetourManager::AddFunctionListener("Function Palia.FishingComponent.RpcClient_FishCaught", &Func_DoFishingCleanup, nullptr);
}

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


void FishingDetours::Func_DoFastAutoFishing(const UObject* Context, UFunction* Function, void* Parms) {
    // Toggle values (Safe to leave looped)
    ToggleFishingDelays(bEnableAutoFishing);

    if (!bEnableAutoFishing)
        return;

    if (Configuration::bRequireClickFishing ? !Menu::bShowMenu && IsGameWindowActive() && IsKeyHeld(VK_LBUTTON) : true) {
        auto ValeriaCharacter = GetValeriaCharacter();
        if (!ValeriaCharacter)
            return;

        // Instant Catch
        auto FishingComponent = ValeriaCharacter->GetFishing();
        if (!IsComponentValid(FishingComponent))
            return;
        
        if (static_cast<EFishingState_NEW>(FishingComponent->GetFishingState()) == EFishingState_NEW::Bite) {
            FFishingEndContext Context;
            FishingComponent->RpcServer_EndFishing(Context);
            FishingComponent->SetFishingState(EFishingState_OLD::None);
        }

        if (!ValeriaCharacter->GetEquippedItem().ItemType->IsFishingRod())
            return;

        // Cast the rod
        ValeriaCharacter->ToolPrimaryActionPressed();
        ValeriaCharacter->ToolPrimaryActionReleased();
    }
}

void FishingDetours::Func_DoInstantCatch(const UObject* Context, UFunction* Function, void* Parms) {
    if (!Configuration::bFishingInstantCatch)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    UFishingComponent* FishingComponent = ValeriaCharacter->GetFishing();
    if (!IsComponentValid(FishingComponent))
        return;

    if (static_cast<EFishingState_NEW>(FishingComponent->GetFishingState()) == EFishingState_NEW::Bite) {
        FFishingEndContext Context;
        FishingComponent->RpcServer_EndFishing(Context);
        FishingComponent->SetFishingState(EFishingState_OLD::None);
    }
}

int fishingFlushCounter = 0;

void FishingDetours::Func_DoFishingCleanup(const UObject* Context, UFunction* Function, void* Parms) {
    // Avoid doing extra work
    fishingFlushCounter++;
    if (fishingFlushCounter >= 25) {
        if (APlayerController* PlayerController = GetPlayerController()) {
            PlayerController->ClientFlushLevelStreaming();
            PlayerController->ClientForceGarbageCollection();

            fishingFlushCounter = 0;
        }
    }
}

void FishingDetours::Func_DoFishingCaptureOverride(const UObject* Context, UFunction* Function, void* Parms) {
    auto SelectLoot = static_cast<Params::FishingComponent_RpcServer_SelectLoot*>(Parms);

    if (bCaptureFishingSpot) {
        memcpy(&sOverrideFishingSpot, &SelectLoot->RPCLootParams.WaterType_Deprecated, sizeof(FName));
        bCaptureFishingSpot = false;
    }
    if (bOverrideFishingSpot) {
        memcpy(&SelectLoot->RPCLootParams.WaterType_Deprecated, &sOverrideFishingSpot, sizeof(FName));
    }
}

void FishingDetours::Func_EndFishingDetoured(const UObject* Context, UFunction* Function, void* Parms) {
    auto EndFishing = static_cast<Params::FishingComponent_RpcServer_EndFishing*>(Parms);

    if (Configuration::bFishingInstantCatch || bEnableAutoFishing) {
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
}