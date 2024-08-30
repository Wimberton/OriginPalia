#include "CookingDetours.h"
#include "Core/Configuration.h"
#include "Core/DetourManager.h"
#include "Misc/Utils.h"

#include <SDK/Palia_parameters.hpp>

using namespace SDK;

void CookingDetours::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
    DetourManager::AddFunctionDetour("Function Palia.MinigameComponentBase.IsPlaying", nullptr, &Func_MinigameIsPlaying);
}

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

void CookingDetours::Func_MinigameIsPlaying(void* Context, void* TheStack, void* Result) {
    if (*static_cast<bool*>(Result) && Configuration::bEnableCookingMinigameSkip) {
        FinishCooking();
    }
}
