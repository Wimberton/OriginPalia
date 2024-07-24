#include "HousingDetours.h"
#include "Configuration.h"
#include "Misc/Utils.h"

#include "DetourManager.h"

using namespace SDK;

void HousingDetours::Func_DoPlaceAnywhere() {
    if (!Configuration::bPlaceAnywhere)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    UPlacementComponent* PlacementComponent = ValeriaCharacter->GetPlacement();
    if (!PlacementComponent || !PlacementComponent->IsValidLowLevel() || PlacementComponent->IsDefaultObject())
        return;

    PlacementComponent->CanPlaceHere = true;
}

void HousingDetours::IsBiomeUnlockedDetour() {
    if (!orig_IsBiomeUnlocked) {
        auto biomeconfig = UHousingBiomeConfigAsset::StaticClass();
        auto IsUnlocked = biomeconfig->GetFunction("HousingBiomeConfigAsset", "IsBiomeUnlocked");
        if (IsUnlocked) {
            orig_IsBiomeUnlocked = IsUnlocked->ExecFunction;
            IsUnlocked->ExecFunction = &Func_IsBiomeUnlocked;
        }
    }
}

void HousingDetours::Func_IsBiomeUnlocked(void* Context, void* TheStack, void* Result) {
    orig_IsBiomeUnlocked(Context, TheStack, Result);
#ifdef ENABLE_SUPPORTER_FEATURES
    if(Configuration::bEnableFreeLandscape) {
        *static_cast<bool*>(Result) = true;
    }
#endif
}