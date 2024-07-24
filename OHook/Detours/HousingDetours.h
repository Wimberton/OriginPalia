#pragma once

#include <SDK.hpp>

class PaliaOverlay;

using namespace SDK;

class HousingDetours final {
public:
    HousingDetours() = default;

    static void Func_DoPlaceAnywhere();
    static void IsBiomeUnlockedDetour();
    static void Func_IsBiomeUnlocked(void* Context, void* TheStack, void* Result);
	
    static UFunction::FNativeFuncPtr orig_IsBiomeUnlocked;
};

inline UFunction::FNativeFuncPtr HousingDetours::orig_IsBiomeUnlocked;
