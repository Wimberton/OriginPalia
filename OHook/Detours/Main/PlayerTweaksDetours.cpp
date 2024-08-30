#include "Detours/Main/PlayerTweaksDetours.h"
#include "Core/Configuration.h"
#include "Core/DetourManager.h"
#include "Misc/Utils.h"

void PlayerTweaksDetours::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
    DetourManager::AddInstance(VPC);
	DetourManager::AddFunctionListener("Function Engine.PlayerController.ClientRestart", nullptr, &ResetTweaks);
	DetourManager::AddInstance(VC);
	DetourManager::AddFunctionListener("Function Engine.Actor.ReceiveTick", &Func_DoCharacterTweaks, nullptr);
	DetourManager::AddFunctionListener("Function Engine.Actor.ReceiveTick", &Func_DoGatherActors, nullptr);
    DetourManager::AddFunctionDetour("Function Palia.VAL_CharacterCustomizationComponent.OnRep_LastCharacterRespecTime", nullptr, &Func_OnRep_LastCharacterRespecTime);
}

void PlayerTweaksDetours::Func_DoCharacterTweaks(const UObject* Context, UFunction* Function, void* Parms) {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::minutes>(currentTime - lastAFKPing);
    
    if (elapsedTime.count() < antiAFKInterval &&
        PlayerModsToggle.bUnlimitedWardrobeRespec == Configuration::bEnableUnlimitedWardrobeRespec &&
        PlayerModsToggle.bInteractionsIncrease == Configuration::bEnableInteractionMods &&
        !AreFloatsEqual(PlayerModsToggle.bInteractionRadius, Configuration::InteractionRadius) &&
        PlayerModsToggle.bAntiAFK == Configuration::bEnableAntiAfk &&
        PlayerModsToggle.maxJumpModValue == Configuration::CustomMaxJumps) {
        return;
    }
    
    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    const auto ValeriaController = GetValeriaController();
    if (!ValeriaController)
        return;

    // AntiAFK
    if (Configuration::bEnableAntiAfk && elapsedTime.count() >= antiAFKInterval) {
        ValeriaController->Client_InactivityHeartbeat();
        ValeriaController->RpcServer_NotifyInactivityInterrupted();
        lastAFKPing = currentTime;
        antiAFKInterval = RNG(randomEngine);
    }
    
    // Unlimited Wardrobe Respec
    if (PlayerModsToggle.bUnlimitedWardrobeRespec != Configuration::bEnableUnlimitedWardrobeRespec) {
        if(Configuration::bEnableUnlimitedWardrobeRespec) {
            ValeriaController->ValeriaCharacter->CharacterCustomizationComponent->LastCharacterRespecTime = 0;
        }
        PlayerModsToggle.bUnlimitedWardrobeRespec = Configuration::bEnableUnlimitedWardrobeRespec;
    }

    // InteractionMod
    if (PlayerModsToggle.bInteractionsIncrease != Configuration::bEnableInteractionMods || !AreFloatsEqual(PlayerModsToggle.bInteractionRadius, Configuration::InteractionRadius)) {
        ValeriaController->InteractableSearchRadius = Configuration::bEnableInteractionMods ? Configuration::InteractionRadius : 200.0f;
        ValeriaController->InteractableSearchDistanceAhead = Configuration::bEnableInteractionMods ? Configuration::InteractionRadius : 100.0f;

        PlayerModsToggle.bInteractionsIncrease = Configuration::bEnableInteractionMods;
        PlayerModsToggle.bInteractionRadius = Configuration::InteractionRadius;
    }

    // Jump Mod
    if (PlayerModsToggle.maxJumpModValue != Configuration::CustomMaxJumps) {
        ValeriaCharacter->JumpMaxCount = Configuration::CustomMaxJumps ? Configuration::CustomMaxJumps : 1;
        
        PlayerModsToggle.maxJumpModValue = Configuration::CustomMaxJumps;
    }
}

void PlayerTweaksDetours::ResetTweaks(const UObject* Context, UFunction* Function, void* Parms) {
    PlayerModsToggle = {};
}

void PlayerTweaksDetours::Func_OnRep_LastCharacterRespecTime(void* Context, void* TheStack, void* Result) {
    if(Configuration::bEnableUnlimitedWardrobeRespec) {
        const auto ValeriaController = GetValeriaController();
        if (ValeriaController) {
            ValeriaController->ValeriaCharacter->CharacterCustomizationComponent->LastCharacterRespecTime = 0;
        }
    }
}

bool TeleportToGatherables = false;
void PlayerTweaksDetours::Func_DoGatherActors(const UObject* Context, UFunction* Function, void* Parms) {
    if (!Configuration::bEnableAutoGather)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;
    
    for (auto& cachedActors : ActorHandler::GetActorsOfType(EType::Forage | EType::Loot)) {
        // Do nothing for unselected ESP animals
        if(!cachedActors.ShouldShow(ValeriaCharacter)) continue;        
        
        const auto ValeriaAnimComponent = ValeriaCharacter->GetAbilityAnimation();
        if (!IsComponentValid(ValeriaAnimComponent))
            return;

        auto ValeriaMovementComponent = ValeriaCharacter->GetValeriaCharacterMovementComponent();
        if (!ValeriaMovementComponent)
            return;

        if (!cachedActors.Actor)
            continue;

        switch (cachedActors.ActorType) {
        case EType::Forage: {
            auto gatherableActor = static_cast<ABP_ValeriaGatherable_C*>(cachedActors.Actor);
            if(!IsActorValid(gatherableActor))
                continue;
            
            auto* gatherableInteractable = gatherableActor->Interactable;
            if(!IsComponentValid(gatherableInteractable))
                continue;

            gatherableActor->bForceNetAddressable = true;

            if (!TeleportToGatherables && cachedActors.Distance > 0.75f && cachedActors.Distance < 8.0f) {
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
        break;
        default:

            auto lootActor = static_cast<ABP_Loot_C*>(cachedActors.Actor);
            if(!IsActorValid(lootActor))
                continue;
            
            auto* gatherableInteractable = lootActor->Interactable;
            if (TeleportToGatherables || !IsComponentValid(gatherableInteractable))
                continue;

            lootActor->bForceNetAddressable = true;

            if (cachedActors.Distance > 0.75f && cachedActors.Distance < 8.0f) {
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
            break;
        }
    }
}