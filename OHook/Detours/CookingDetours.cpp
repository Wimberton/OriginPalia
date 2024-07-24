#include "CookingDetours.h"
#include "Configuration.h"
#include "Misc/Utils.h"

#include <SDK/Palia_parameters.hpp>

using namespace SDK;

void CookingDetours::FinishCooking() {
    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    const auto CookingQTE = ValeriaCharacter->MinigameQTE;
    if (CookingQTE->ServerMinigameState == EMinigameState::Success) {
        ValeriaCharacter->AbilityAnimation->StopAll(0.f);
        return;
    }
    
    CookingQTE->ServerMinigameState = EMinigameState::Success;
    CookingQTE->RpcServer_ChangeState(EMinigameState::Success);
}

void CookingDetours::HasCookingDetour() {
    if (!orig_MinigameIsPlaying) {
        auto cookingMinigame = UMinigameComponentBase::StaticClass();
        auto cookingStarted = cookingMinigame->GetFunction("MinigameComponentBase", "IsPlaying"); // IsPaused has less calls than IsPlaying
        if (cookingStarted) {
            orig_MinigameIsPlaying = cookingStarted->ExecFunction;
            cookingStarted->ExecFunction = &Func_MinigameIsPlaying;
        }
    }
}

void CookingDetours::Func_MinigameIsPlaying(void* Context, void* TheStack, void* Result) {
    orig_MinigameIsPlaying(Context, TheStack, Result);
    if (*static_cast<bool*>(Result) && Configuration::bEnableMinigameSkip) {
        FinishCooking();
    }
}
