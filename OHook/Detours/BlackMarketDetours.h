#pragma once

#include "PaliaOverlay.h"

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class BlackMarketDetours final {
public:
	static void SetupDetours();
	static bool bShouldOpenConfigStore;
	static std::string SelectedStoreConfig;
	
private:
	static void Func_OnClientInteract(void*, void*, void*);
	static void Func_SpinTheWheel(void*, void*, void*);
	static void Func_IsInteractable(void*, void*, void*);
	static inline UFunction::FNativeFuncPtr Orig_OnClientInteract;
	static inline UFunction::FNativeFuncPtr Orig_SpinTheWheel;
	static inline UFunction::FNativeFuncPtr Orig_IsInteractable;
	

	static inline UVAL_PrizeWheelComponent* WheelComponentUsed;
	static inline TSoftObjectPtr<class UVAL_ItemTypeDefinitionAsset> PrizeWheelCoin;
	static inline FBagSlotLocation CoinLocation;
	static inline bool bCoinsStored;
	static inline float TimeSpun;
};

inline bool BlackMarketDetours::bShouldOpenConfigStore = false;
inline std::string BlackMarketDetours::SelectedStoreConfig = "/Game/Configs/RegisterStores/DA_Store_BlackMarket.DA_Store_BlackMarket";
