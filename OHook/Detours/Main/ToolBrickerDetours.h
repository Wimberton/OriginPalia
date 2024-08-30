#pragma once

#include "SDK.hpp"
#include "SDK/WBP_RepairStation_v3_CM_classes.hpp"

class PaliaOverlay;

using namespace SDK;

class ToolBrickerDetours {
public:
    ToolBrickerDetours() = default;

    static void SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC);

    static void Func_DoToolBricker(const UObject* Context, UFunction* Function, void* Parms);
    static void Func_PurchaseRepair(const UObject* Context, UFunction* Function, void* Parms);

    static void SetupRepairStation();
    static void SetRepairStation(UWBP_RepairStation_v3_CM_C*);
    static void ClearRepairStation();
    
private:
    static void SetGoldButton(UObject*, std::wstring, bool);
    static void InvalidateFunctionPtrs();

    static UWBP_RepairStation_v3_CM_C* RepairStation;
    static UClass* RepairStationClass;

    static UFunction* Button_SetText;
    static UFunction* Button_SetEnabled;
    static UFunction* Checkmark_SetText;
    static UFunction* RepairStation_EnableGoldRepair;
    static UFunction* RepairStation_RefreshGoldRepairButton;
    static UFunction* RepairStation_SetAllowRepairKitsFromStorage;
    static UFunction* RepairStation_RefreshRepairKitRepairButton;
    static UFunction* RepairStation_GetPlayerGoldAmount;
    static UFunction* RepairStation_GetGoldCostForAllTools;
    static UFunction* RepairStation_FindMostDamagedTool;
    static UFunction* RepairStation_GetToolDurability;

    static int32 RepairAmount;

    static constexpr int32 MaxGold = 999999;
};

inline int32 ToolBrickerDetours::RepairAmount = 0;

inline UWBP_RepairStation_v3_CM_C* ToolBrickerDetours::RepairStation = nullptr;

inline UClass* ToolBrickerDetours::RepairStationClass = nullptr;
inline UFunction* ToolBrickerDetours::Button_SetText = nullptr;
inline UFunction* ToolBrickerDetours::Button_SetEnabled = nullptr;
inline UFunction* ToolBrickerDetours::Checkmark_SetText = nullptr;
inline UFunction* ToolBrickerDetours::RepairStation_EnableGoldRepair = nullptr;
inline UFunction* ToolBrickerDetours::RepairStation_RefreshGoldRepairButton = nullptr;
inline UFunction* ToolBrickerDetours::RepairStation_SetAllowRepairKitsFromStorage = nullptr;
inline UFunction* ToolBrickerDetours::RepairStation_RefreshRepairKitRepairButton = nullptr;
inline UFunction* ToolBrickerDetours::RepairStation_GetPlayerGoldAmount = nullptr;
inline UFunction* ToolBrickerDetours::RepairStation_GetGoldCostForAllTools = nullptr;
inline UFunction* ToolBrickerDetours::RepairStation_FindMostDamagedTool = nullptr;
inline UFunction* ToolBrickerDetours::RepairStation_GetToolDurability = nullptr;