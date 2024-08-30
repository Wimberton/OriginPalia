#pragma once
#include "Overlay/PaliaOverlay.h"
#include <random>

#include <SDK.hpp>

class PaliaOverlay;

using namespace SDK;

class PlayerTweaksDetours final {
public:
    PlayerTweaksDetours() = default;

    static void SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC);

    static void Func_OnRep_LastCharacterRespecTime(void* Context, void* TheStack, void* Result);
    
    struct PlayerModsToggles {
        bool bUnlimitedWardrobeRespec;
        bool bInteractionsIncrease;
        float bInteractionRadius;
        bool bAntiAFK;
        int maxJumpModValue;

        PlayerModsToggles() : bUnlimitedWardrobeRespec(false), bInteractionsIncrease(false), bInteractionRadius(200.f), bAntiAFK(false), maxJumpModValue(1) {
        }
    };
    static PlayerModsToggles PlayerModsToggle;
    
    static void Func_DoCharacterTweaks(const UObject* Context, UFunction* Function, void* Parms);
    static void ResetTweaks(const UObject* Context, UFunction* Function, void* Parms);
    static void Func_DoGatherActors(const UObject* Context, UFunction* Function, void* Parms);
};

inline PlayerTweaksDetours::PlayerModsToggles PlayerTweaksDetours::PlayerModsToggle;

// Anti-AFK Randomizer
static std::default_random_engine randomEngine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
static std::uniform_int_distribution RNG(10, 20); // RNG 10~20 minutes
static int antiAFKInterval = RNG(randomEngine);

inline std::chrono::steady_clock::time_point lastAFKPing = std::chrono::steady_clock::now();


