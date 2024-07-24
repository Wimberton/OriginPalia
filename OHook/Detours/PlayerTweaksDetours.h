#pragma once
#include "PaliaOverlay.h"
#include <random>

#include <SDK.hpp>

class PaliaOverlay;

using namespace SDK;

class PlayerTweaksDetours final {
public:
    PlayerTweaksDetours() = default;

    static void Func_OnRep_LastCharacterRespecTime(void* Context, void* TheStack, void* Result);
    static void OnRep_LastCharacterRespecTimeDetour();
    
    struct PlayerModsToggles {
        bool bUnlimitedWardrobeRespec;
        bool bInteractionsIncrease;
        float bInteractionRadius;
        bool bAntiAFK;

        PlayerModsToggles() : bUnlimitedWardrobeRespec(false), bInteractionsIncrease(false), bInteractionRadius(200.f), bAntiAFK(false) {}
    };
    static PlayerModsToggles PlayerModsToggle;
    static UFunction::FNativeFuncPtr orig_OnRep_LastCharacterRespecTime;
    
    static void Func_DoCharacterTweaks();
    static void ResetTweaks();
};

inline PlayerTweaksDetours::PlayerModsToggles PlayerTweaksDetours::PlayerModsToggle;
inline UFunction::FNativeFuncPtr PlayerTweaksDetours::orig_OnRep_LastCharacterRespecTime;

// Anti-AFK Randomizer
static std::default_random_engine randomEngine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
static std::uniform_int_distribution RNG(10, 20); // RNG 10~20 minutes
static int antiAFKInterval = RNG(randomEngine);

inline std::chrono::steady_clock::time_point lastAFKPing = std::chrono::steady_clock::now();


