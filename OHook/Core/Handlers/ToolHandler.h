#pragma once

#include <SDK.hpp>

enum class ETools {
    None,
    Axe,
    Belt,
    Bow,
    FishingRod,
    Hoe,
    Pick,
    WateringCan,
    MAX
};
static inline std::vector<std::string> EToolsString = {
    "None",
    "Axe",
    "Belt",
    "Bow",
    "Fishing Rod",
    "Hoe",
    "Pick",
    "Watering Can",
};

enum class EToolTier {
    None,
    Simple,
    Standard,
    Fine,
    Exquisite,
    MAX
};
static inline std::vector<std::string> EToolTierString = {
    "None",
    "Simple",
    "Standard",
    "Fine",
    "Exquisite",
};

using namespace SDK;

class ToolHandler
{
public:
    static void SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC);

    static void Func_PE_EquipmentChanged(const UObject* Context, UFunction* Function, void* Parms);
    static void Func_PE_ClientTravelInternal(const UObject* Context, UFunction* Function, void* Parms);

    static std::pair<ETools, EToolTier> ParseItem(FValeriaItem);
    static void SetEquippedTool(ETools);

    static ETools EquippedTool;
    static EToolTier EquippedToolTier;
    static FBagSlotLocation EquippedToolBagSlot;

private:
    static inline std::unordered_map<ETools, UInputAction*> ToolActionMap = {};
    static void PopulateToolActionMap();
};

inline ETools ToolHandler::EquippedTool = ETools::None;
inline EToolTier ToolHandler::EquippedToolTier = EToolTier::None;
inline FBagSlotLocation ToolHandler::EquippedToolBagSlot = FBagSlotLocation{-1,-1};