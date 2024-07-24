#pragma once

#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class FishingDetours final {
public:
	FishingDetours() = default;

	static void Func_DoFastAutoFishing();
	static void Func_DoFishingCaptureOverride(void* Params);
	static void Func_DoInstantCatch();
	static Params::FishingComponent_RpcServer_EndFishing* EndFishingDetoured(void* Params);
	static void Func_DoFishingCleanup();

	static FName sOverrideFishingSpot;
	static bool bCaptureFishingSpot;
	static bool bOverrideFishingSpot;
	static bool bEnableAutoFishing;
	static int bSelectedFishingSpot;
	static const char* bFishingSpots[14];
	static const wchar_t* bFishingSpotsFString[13];
};

inline FName FishingDetours::sOverrideFishingSpot;
inline bool FishingDetours::bCaptureFishingSpot = false;
inline bool FishingDetours::bOverrideFishingSpot = false;
inline bool FishingDetours::bEnableAutoFishing = false;
inline int FishingDetours::bSelectedFishingSpot = 0;
inline const char* FishingDetours::bFishingSpots[14] = { "- none -","River Village","River Village *","Pond Village","Pond Village *","Lake Village","Lake Village *","Ocean AZ1","Ocean AZ1 *","River AZ1","River AZ1 *","Cave AZ1","Cave AZ1 *","100G Pool" };
inline const wchar_t* FishingDetours::bFishingSpotsFString[13]{ L"RiverVillage",L"RiverVillage_SQ",L"PondVillage",L"PondVillage_SQ",L"LakeVillage",L"LakeVillage_SQ",L"OceanAZ1",L"OceanAZ1_SQ",L"RiverAZ1",L"RiverAZ1_SQ",L"CaveAZ1",L"CaveAZ1_SQ",L"test" };
