#include "PlayerTweaksDetours.h"

#include "Configuration.h"
#include "Misc/Utils.h"

void PlayerTweaksDetours::Func_DoCharacterTweaks() {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::minutes>(currentTime - lastAFKPing);
    
    if (elapsedTime.count() < antiAFKInterval &&
        PlayerModsToggle.bUnlimitedWardrobeRespec == Configuration::bEnableUnlimitedWardrobeRespec &&
        PlayerModsToggle.bInteractionsIncrease == Configuration::bEnableInteractionMods &&
        PlayerModsToggle.bInteractionRadius == Configuration::InteractionRadius &&
        PlayerModsToggle.bAntiAFK == Configuration::bEnableAntiAfk) {
        return;
    }
    
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
    if (PlayerModsToggle.bInteractionsIncrease != Configuration::bEnableInteractionMods || PlayerModsToggle.bInteractionRadius != Configuration::InteractionRadius) {
        ValeriaController->InteractableSearchRadius = Configuration::bEnableInteractionMods ? Configuration::InteractionRadius : 200.0f;
        ValeriaController->InteractableSearchDistanceAhead = Configuration::bEnableInteractionMods ? Configuration::InteractionRadius : 100.0f;

        PlayerModsToggle.bInteractionsIncrease = Configuration::bEnableInteractionMods;
        PlayerModsToggle.bInteractionRadius = Configuration::InteractionRadius;
    }
}

void PlayerTweaksDetours::ResetTweaks() {
    PlayerModsToggle = {};
}

void PlayerTweaksDetours::OnRep_LastCharacterRespecTimeDetour() {
    if (!orig_OnRep_LastCharacterRespecTime) {
        auto CustomizationComponent = UVAL_CharacterCustomizationComponent::StaticClass();
        auto OnRep_LastCharacterRespecTime = CustomizationComponent->GetFunction("VAL_CharacterCustomizationComponent", "OnRep_LastCharacterRespecTime");
        if (OnRep_LastCharacterRespecTime) {
            orig_OnRep_LastCharacterRespecTime = OnRep_LastCharacterRespecTime->ExecFunction;
            OnRep_LastCharacterRespecTime->ExecFunction = &Func_OnRep_LastCharacterRespecTime;
        }
    }
}

void PlayerTweaksDetours::Func_OnRep_LastCharacterRespecTime(void* Context, void* TheStack, void* Result) {
    orig_OnRep_LastCharacterRespecTime(Context, TheStack, Result);
    if(Configuration::bEnableUnlimitedWardrobeRespec) {
        const auto ValeriaController = GetValeriaController();
        if (ValeriaController) {
            ValeriaController->ValeriaCharacter->CharacterCustomizationComponent->LastCharacterRespecTime = 0;
        }
    }
}