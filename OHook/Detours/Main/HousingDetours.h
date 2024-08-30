#pragma once

#include <SDK.hpp>

class PaliaOverlay;

using namespace SDK;

class HousingDetours final {
public:
    HousingDetours() = default;

    static void SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC);
    static void Func_DoPlaceAnywhere(const UObject* Context, UFunction* Function, void* Parms);

    static void Func_PE_RpcServer_PlaceItem(const UObject* Context, UFunction* Function, void* Parms);
    static void Func_PE_RpcServer_UpdateLockedItemToPlace(const UObject* Context, UFunction* Function, void* Parms);
    static void Func_PE_RpcServer_LockItemToPlace(const UObject* Context, UFunction* Function, void* Parms);

    static void ChangeAxis(int Axis, double Change, FRotator* Rotation);

    static FRotator PlacementActorRotation;
    static FVector PlacementPositionOffset;
    static FRotator LastRotation;
    static int ModifiedAxis;
    static bool NewItem;
};

inline FRotator HousingDetours::PlacementActorRotation = {0,0,0};
inline FVector HousingDetours::PlacementPositionOffset = {0,0,0};
inline FRotator HousingDetours::LastRotation = {0,0,0};
inline int HousingDetours::ModifiedAxis = 0;
inline bool HousingDetours::NewItem = false;
