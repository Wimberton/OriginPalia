#pragma once

#include "Overlay/PaliaOverlay.h"

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class BlackMarketDetours final {
public:
	static void SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC);
	
	static void Func_OnClientInteract(void*, void*, void*);
	static void Func_SpinTheWheel(void*, void*, void*);
	static void Func_IsInteractable(void*, void*, void*);

private:

	static inline UVAL_PrizeWheelComponent* WheelComponentUsed;
	static inline TSoftObjectPtr<class UVAL_ItemTypeDefinitionAsset> PrizeWheelCoin;
	static inline FBagSlotLocation CoinLocation;
	static inline bool bCoinsStored;
	static inline float TimeSpun;
};