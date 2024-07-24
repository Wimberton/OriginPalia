#pragma once

#include <SDK.hpp>

class PaliaOverlay;

using namespace SDK;

class CookingDetours {
public:
    static void FinishCooking();
    static void HasCookingDetour();
    static void Func_MinigameIsPlaying(void* Context, void* TheStack, void* Result);

    static UFunction::FNativeFuncPtr orig_MinigameIsPlaying;
};

inline UFunction::FNativeFuncPtr CookingDetours::orig_MinigameIsPlaying;