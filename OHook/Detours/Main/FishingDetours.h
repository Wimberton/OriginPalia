#pragma once

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class FishingDetours final {
public:
	FishingDetours() = default;

	static void SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC);

	static void Func_DoFastAutoFishing(const UObject* Context, UFunction* Function, void* Parms);
	static void Func_DoFishingCaptureOverride(const UObject* Context, UFunction* Function, void* Parms);
	static void Func_DoInstantCatch(const UObject* Context, UFunction* Function, void* Parms);
	static void Func_EndFishingDetoured(const UObject* Context, UFunction* Function, void* Parms);
	static void Func_DoFishingCleanup(const UObject* Context, UFunction* Function, void* Parms);

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
inline const char* FishingDetours::bFishingSpots[14] = { "- none -","Kilima River","Kilima River (Hotspot)","Pond","Pond (Hotspot)","Kilima Lake","Kilima Lake (Hotspot)","Bahari Coast","Bahari Coast (Hotspot)","Bahari River","Bahari River (Hotspot)","Cave","Cave (Hotspot)","100G Pool" };
inline const wchar_t* FishingDetours::bFishingSpotsFString[13]{ L"RiverVillage",L"RiverVillage_SQ",L"PondVillage",L"PondVillage_SQ",L"LakeVillage",L"LakeVillage_SQ",L"OceanAZ1",L"OceanAZ1_SQ",L"RiverAZ1",L"RiverAZ1_SQ",L"CaveAZ1",L"CaveAZ1_SQ",L"test" };
