#pragma once

#include <SDK.hpp>

class PaliaOverlay;

using namespace SDK;

class CookingDetours {
public:
    static void SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC);

    static void FinishCooking();
    static void Func_MinigameIsPlaying(void* Context, void* TheStack, void* Result);
};