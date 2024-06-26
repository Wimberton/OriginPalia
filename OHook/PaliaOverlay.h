#pragma once

#include <OverlayBase.h>
#include "DetourManager.h"

#include <map>
#include <imgui.h>

#include <SDK.hpp>

using namespace SDK;

#define NO_HOTKEY -1

inline std::vector<std::string> debugger;
inline DetourManager gDetourManager;

inline std::map<int, std::string> CreatureQualityNames = {
    {0, "Unknown"},
    {1, "T1"},
    {2, "T2"},
    {3, "T3"},
    {4, "Chase"}
};

inline std::map<int, std::string> BugQualityNames = {
    {0, "Unknown"},
    {1, "Common"},
    {2, "Uncommon"},
    {3, "Rare"},
    {4, "Rare2"},
    {5, "Epic"}
};

inline std::map<int, std::string> GatherableSizeNames = {
    {0, "Unknown"},
    {1, "S"},
    {2, "M"},
    {3, "L"},
    {4, "Bush"}
};

enum class EType {
    Unknown,
    Tree,
    Ore,
    Bug,
    Animal,
    Forage,
    Fish,
    Loot,
    Players,
    NPCs,
    Quest,
    RummagePiles,
    Stables,
    Gates,
    Treasure,
    MAX
};

enum class ECreatureKind {
    Unknown,
    Chapaa,
    Cearnuk,
    TreeClimber,
    MAX
};

enum class ECreatureQuality {
    Unknown,
    Tier1,
    Tier2,
    Tier3,
    Chase,
    MAX
};

enum class EBugKind {
    Unknown,
    Bee,
    Beetle,
    Butterfly,
    Cicada,
    Crab,
    Cricket,
    Dragonfly,
    Glowbug,
    Ladybug,
    Mantis,
    Moth,
    Pede,
    Snail,
    MAX
};

enum class EBugQuality {
    Unknown,
    Common,
    Uncommon,
    Rare,
    Rare2,
    Epic,
    MAX
};

enum class EGatherableSize {
    Unknown,
    Small,
    Medium,
    Large,
    Bush,
    MAX
};

enum class ETreeType {
    Unknown,
    Flow,
    Heartwood,
    Sapwood,
    Bush,
    MAX
};

enum class EOreType {
    Unknown,
    Stone,
    Copper,
    Clay,
    Iron,
    Silver,
    Gold,
    Palium,
    MAX
};

enum class EFishType {
    Unknown,
    Node,
    Hook,
    MAX
};

enum class EForageableType {
    Unknown,
    Oyster,
    Coral,
    MushroomBlue,
    MushroomRed,
    Heartdrop,
    DragonsBeard,
    EmeraldCarpet,
    PoisonFlower,
    Shell,
    DariCloves,
    HeatRoot,
    SpicedSprouts,
    Sundrop,
    SweetLeaves,
    WaterFlower,
    Garlic,
    Ginger,
    GreenOnion,
    MAX
};

enum class EOneOffs {
    Player,
    NPC,
    FishHook,
    FishPool,
    Loot,
    Quest,
    RummagePiles,
    Stables,
    Others,
    Gates,
    Treasure,
    MAX
};

struct FEntry {
    AActor* Actor;
    FVector WorldPosition;
    std::wstring DisplayName;
    EType ActorType;
    int Type;
    int Quality;
    int Variant;
    double Distance;
    std::string TexturePath;
};

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

inline const char* STools[] = {
    "None",
    "Axe",
    "Belt",
    "Bow",
    "FishingRod",
    "Hoe",
    "Pick",
    "WateringCan",
    "Unknown"
};

enum class ELocation {
    // Home
    Global_Home,

    // Kilima Entrances
    Kilima_HousingPlot,
    Kilima_ToBahariBay,

    // Kilima Stables
    Kilima_CentralStables,
    Kilima_MirrorFields,
    Kilima_LeafhopperHills,
    Kilima_WhisperingBanks,

    // Kilima Others
    Kilima_Fairgrounds_MajiMarket,
    Kilima_GeneralStore,
    Kilima_RepairStation,
    Kilima_TailorStore,
    Kilima_FurnitureStore,
    Kilima_CityHall,
    Kilima_Tavern,
    Kilima_PhoenixFalls,
    Kilima_DragonShrine,
    Kilima_FishermansLagoon,
    Kilima_WimsFishingSpot,

    // Bahari Entrances
    Bahari_HousingPlot,
    Bahari_ToKilimaValley,

    // Bahari Stables
    Bahari_CentralStables,
    Bahari_TheOutskirts,
    Bahari_CoralShores,
    Bahari_PulsewaterPlains,
    Bahari_HideawayBluffs,
    Bahari_BeachcomberCove,

    // Others

    UserDefined
};

struct FLocation {
    std::string MapName;
    ELocation Type;
    std::string Name;
    FVector Location;
    FRotator Rotate;
};

class PaliaOverlay final : public OverlayBase {
protected:
    void DrawHUD() override;
    void DrawOverlay() override;

private:
    bool showHotkeyPopup = false;
    int* currentHotkeyTarget = nullptr;
    bool prevKeyStates[30] = { false };

public:
    void SetupColors();
    void ProcessActors(int);

    static std::map<int, std::string> CreatureQualityNames;
    static std::map<int, std::string> BugQualityNames;
    static std::map<int, std::string> GatherableSizeNames;
    static std::map<int, std::string> TypeIcons;

    static std::wstring GetQualityName(const int quality, const EType type) {
        switch (type) {
            // case EType::Animal:
            //     return CreatureQualityNames[quality];
            // case EType::Bug:
            //     return BugQualityNames[quality];
            // case EType::Forage:
            //     return GatherableSizeNames[quality];
        default:
            return L"";
        }
    }

    // Last time ESPs were cached
    double LastCachedTime;
    int ActorStep;

    double BestScore;
    AActor* BestTargetActorAnimal = nullptr;
    AActor* BestTargetActor = nullptr;
    EType BestTargetActorType;
    FVector BestTargetLocation;
    FRotator BestTargetRotation;

    // Cached actors
    std::vector<FEntry> CachedActors;

    ULevel* CurrentLevel;
    std::string CurrentMap = "Unknown";
    std::vector<FLocation> TeleportLocations = {
        // Global Home
        {"UserDefined", ELocation::Global_Home, "[Global] Housing Plot", {00000.000f, 00000.000f, 00000.000}, {0.000f, 0.00f, 0.000f}},
        // Kilima City Spots
        {"Village_Root", ELocation::Kilima_GeneralStore, "[Kilima] General Store", {156.193f, -1509.144f, 305.576}, {0.000f, -122.07f, 0.000f}},
        {"Village_Root", ELocation::Kilima_TailorStore, "[Kilima] Tailor Store", {-1715.341f, 2052.674f, 163.455}, {0.000f, -123.346f, 0.000f}},
        {"Village_Root", ELocation::Kilima_FurnitureStore, "[Kilima] Furniture Store", {3734.435f, -563.303f, 161.163}, {0.000f, -122.006f, 0.000f}},
        {"Village_Root", ELocation::Kilima_CityHall, "[Kilima] City Hall", {6344.635f, 2519.250f, 511.920}, {0.000f, 0.451f, 0.000f}},
        {"Village_Root", ELocation::Kilima_Tavern, "[Kilima] Tavern", {-755.001f, 4102.823f, 330.888}, {0.000f, 120.090f, 0.000f}},
        {"Village_Root", ELocation::Kilima_RepairStation, "[Kilima] Repair Station", {2465.246f, 6734.343f, 323.050}, {0.000f, 120.89f, 0.000f}},
        // Kilima Entrances
        {"Village_Root", ELocation::Kilima_HousingPlot, "[Kilima] Housing Plot Entrance", {20204.810f, -15375.620f, 2272.460}, {0.000f, -6.24f, 0.000f}},
        {"Village_Root", ELocation::Kilima_ToBahariBay, "[Kilima] Bahari Bay Entrance", {50623.753f, -5403.530f, 1332.610}, {0.000f, -24.72f, 0.000f}},
        // Kilima Stables
        {"Village_Root", ELocation::Kilima_CentralStables, "[Kilima] Central Stables", {9746.436f, 11380.740f, -119.406}, {0.000f, 145.62f, -0.000f}},
        {"Village_Root", ELocation::Kilima_MirrorFields, "[Kilima] Mirror Fields", {-6586.737f, -24412.872f, 1647.018}, {0.000f, 259.87f, 0.000f}},
        {"Village_Root", ELocation::Kilima_LeafhopperHills, "[Kilima] Leafhopper Hills", {-21589.245f, 3920.281f, 326.105}, {0.000f, 117.04f, 0.000f}},
        {"Village_Root", ELocation::Kilima_WhisperingBanks, "[Kilima] Whispering Banks", {38958.433f, 7574.109f, -657.983}, {0.000f, 214.71f, 0.000f}},
        // Kilima Others
        {"Village_Root", ELocation::Kilima_Fairgrounds_MajiMarket, "[Kilima] Fairgrounds Entrance (Maji Market)", {54696.812f, 21802.943f, -94.603}, {0.000f, -7.65f, 0.000f}},
        {"Village_Root", ELocation::Kilima_PhoenixFalls, "[Kilima] Phoenix Falls", {7596.145f, -51226.853f, 4026.341}, {0.000f, -68.64f, 0.000f}},
        {"Village_Root", ELocation::Kilima_DragonShrine, "[Kilima] Dragon Shrine", {33743.980f, -22253.369f, 1323.870}, {0.000f, -90.211f, 0.000f}},
        {"Village_Root", ELocation::Kilima_FishermansLagoon, "[Kilima] Fisherman's Lagoon", {25325.044f, 19246.651f, -439.559}, {0.000f, 25.76f, 0.000f}},
        {"Village_Root", ELocation::Kilima_WimsFishingSpot, "[Kilima] Wims Secret Fishing Spot", {58217.884f, 41798.154f, -432.593}, {0.000f, 94.089f, 0.000f}},
        // Bahari Entrances 
        {"AZ1_01_Root", ELocation::Bahari_HousingPlot, "[Bahari] Housing Plot Entrance", {54451.777f, -81603.203f, 962.742}, {0.000f, 2.45f, 0.000f}},
        {"AZ1_01_Root", ELocation::Bahari_ToKilimaValley, "[Bahari] Kilima Valley Entrance", {59457.644f, -10932.726f, 917.334}, {0.000f, -157.46f, 0.000f}},
        // Bahari Stables
        {"AZ1_01_Root", ELocation::Bahari_CentralStables, "[Bahari] Central Stables", {103331.330f, -63125.339f, -1928.051}, {0.000f, 148.09f, -0.000f}},
        {"AZ1_01_Root", ELocation::Bahari_TheOutskirts, "[Bahari] The Outskirts", {55743.927f, -81821.534f, 922.205}, {0.000f, 226.06f, 0.000f}},
        {"AZ1_01_Root", ELocation::Bahari_CoralShores, "[Bahari] Coral Shores", {143344.052f, -71851.374f, -4353.302}, {0.000f, 352.16f, 0.000f}},
        {"AZ1_01_Root", ELocation::Bahari_PulsewaterPlains, "[Bahari] Pulsewater Plains", {101257.267f, -23873.064f, -1635.604}, {0.000f, 287.12f, 0.000f}},
        {"AZ1_01_Root", ELocation::Bahari_HideawayBluffs, "[Bahari] Hideaway Bluffs", {82490.721f, 2994.478f, -694.785}, {0.000f, 14.61f, 0.000f}},
        {"AZ1_01_Root", ELocation::Bahari_BeachcomberCove, "[Bahari] Beachcomber Cove", {128767.154f, 4163.842f, -4386.759}, {0.000f, 312.00f, 0.000f}}
    };

    // Class name -> Display name map
    std::map<std::string, std::string> CLASS_NAME_ALIAS = {
        // Trees //

        // Birch
        {"BP_TreeChoppable_Birch_Sapling_C", "Birch XS"},
        {"BP_TreeChoppable_Birch_Small_C", "Birch S"},
        {"BP_TreeChoppable_Birch_Medium_C", "Birch M"},
        {"BP_TreeChoppable_Birch_Large_C", "Birch L"},
        {"BP_TreeChoppable_Birch_Sapling_CoOp_C", "Flow-Infused Birch XS"},
        {"BP_TreeChoppable_Birch_Small_CoOp_C", "Flow-Infused Birch S"},
        {"BP_TreeChoppable_Birch_Medium_CoOp_C", "Flow-Infused Birch M"},
        {"BP_TreeChoppable_Birch_Large_CoOp_C", "Flow-Infused Birch L"},

        // Birch (Housing)
        {"BP_TreeGrowable_Birch_Sapling_C", "Birch XS"},
        {"BP_TreeGrowable_Birch_Small_C", "Birch S"},
        {"BP_TreeGrowable_Birch_Medium_C", "Birch M"},
        {"BP_TreeGrowable_Birch_Large_C", "Birch L"},
        {"BP_TreeGrowable_Birch_Sapling_CoOp_C", "Flow-Infused Birch XS"},
        {"BP_TreeGrowable_Birch_Small_CoOp_C", "Flow-Infused Birch S"},
        {"BP_TreeGrowable_Birch_Medium_CoOp_C", "Flow-Infused Birch M"},
        {"BP_TreeGrowable_Birch_Large_CoOp_C", "Flow-Infused Birch L"},

        // Birch (Foliage On Plot)
        {"BP_FoliageOnPlot_Tree_Birch_Sapling_C", "Birch XS"},
        {"BP_FoliageOnPlot_Tree_Birch_Small_C", "Birch S"},
        {"BP_FoliageOnPlot_TreeBirch_Medium_C", "Birch M"},
        {"BP_FoliageOnPlot_Tree_Birch_Large_C", "Birch L"},

        // Juniper
        {"BP_TreeChoppable_Juniper_Sapling_C", "Juniper XS"},
        {"BP_TreeChoppable_Juniper_Small_C", "Juniper S"},
        {"BP_TreeChoppable_Juniper_Medium_C", "Juniper M"},
        {"BP_TreeChoppable_Juniper_Large_C", "Juniper L"},
        {"BP_TreeChoppable_Juniper_Sapling_CoOp_C", "Flow-Infused Juniper XS"},
        {"BP_TreeChoppable_Juniper_Small_CoOp_C", "Flow-Infused Juniper S"},
        {"BP_TreeChoppable_Juniper_Medium_CoOp_C", "Flow-Infused Juniper M"},
        {"BP_TreeChoppable_Juniper_Large_CoOp_C", "Flow-Infused Juniper L"},

        // Juniper (Housing)
        {"BP_TreeGrowable_Juniper_Sapling_C", "Juniper XS"},
        {"BP_TreeGrowable_Juniper_Small_C", "Juniper S"},
        {"BP_TreeGrowable_Juniper_Medium_C", "Juniper M"},
        {"BP_TreeGrowable_Juniper_Large_C", "Juniper L"},
        {"BP_TreeGrowable_Juniper_Sapling_CoOp_C", "Flow-Infused Juniper XS"},
        {"BP_TreeGrowable_Juniper_Small_CoOp_C", "Flow-Infused Juniper S"},
        {"BP_TreeGrowable_Juniper_Medium_CoOp_C", "Flow-Infused Juniper M"},
        {"BP_TreeGrowable_Juniper_Large_CoOp_C", "Flow-Infused Juniper L"},

        // Oak
        {"BP_TreeChoppable_Oak_Sapling_C", "Oak XS"},
        {"BP_TreeChoppable_Oak_Small_C", "Oak S"},
        {"BP_TreeChoppable_Oak_Medium_C", "Oak M"},
        {"BP_TreeChoppable_Oak_Large_C", "Oak L"},
        {"BP_TreeChoppable_Oak_Sapling_CoOp_C", "Flow-Infused Oak XS"},
        {"BP_TreeChoppable_Oak_Small_CoOp_C", "Flow-Infused Oak S"},
        {"BP_TreeChoppable_Oak_Medium_CoOp_C", "Flow-Infused Oak M"},
        {"BP_TreeChoppable_Oak_Large_CoOp_C", "Flow-Infused Oak L"},

        // Oak (Housing)
        {"BP_TreeGrowable_Oak_Sapling_C", "Oak XS"},
        {"BP_TreeGrowable_Oak_Small_C", "Oak S"},
        {"BP_TreeGrowable_Oak_Medium_C", "Oak M"},
        {"BP_TreeGrowable_Oak_Large_C", "Oak L"},
        {"BP_TreeGrowable_Oak_Sapling_CoOp_C", "Flow-Infused Oak XS"},
        {"BP_TreeGrowable_Oak_Small_CoOp_C", "Flow-Infused Oak S"},
        {"BP_TreeGrowable_Oak_Medium_CoOp_C", "Flow-Infused Oak M"},
        {"BP_TreeGrowable_Oak_Large_CoOp_C", "Flow-Infused Oak L"},

        // Oak (Foliage On Plot)
        {"BP_FoliageOnPlot_Tree_Oak_Sapling_C", "Oak XS"},
        {"BP_FoliageOnPlot_Tree_Oak_Small_C", "Oak S"},
        {"BP_FoliageOnPlot_TreeOak_Medium_C", "Oak M"},
        {"BP_FoliageOnPlot_Tree_Oak_Large_C", "Oak L"},


        // Pine
        {"BP_TreeChoppable_Pine_Sapling_C", "Pine XS"},
        {"BP_TreeChoppable_Pine_Small_C", "Pine S"},
        {"BP_TreeChoppable_Pine_Medium_C", "Pine M"},
        {"BP_TreeChoppable_Pine_Large_C", "Pine L"},
        {"BP_TreeChoppable_Pine_Large2_C", "Pine L"},
        {"BP_TreeChoppable_Pine_Sapling_CoOp_C", "Flow-Infused Pine XS"},
        {"BP_TreeChoppable_Pine_Small_CoOp_C", "Flow-Infused Pine S"},
        {"BP_TreeChoppable_Pine_Medium_CoOp_C", "Flow-Infused Pine M"},
        {"BP_TreeChoppable_Pine_Large_CoOp_C", "Flow-Infused Pine L"},
        {"BP_TreeChoppable_Pine_Large2_CoOp_C", "Flow-Infused Pine L"},

        // Pine (Housing)
        {"BP_TreeGrowable_Pine_Sapling_C", "Pine XS"},
        {"BP_TreeGrowable_Pine_Small_C", "Pine S"},
        {"BP_TreeGrowable_Pine_Medium_C", "Pine M"},
        {"BP_TreeGrowable_Pine_Large_C", "Pine L"},
        {"BP_TreeGrowable_Pine_Large2_C", "Pine L"},
        {"BP_TreeGrowable_Pine_Sapling_CoOp_C", "Flow-Infused Pine XS"},
        {"BP_TreeGrowable_Pine_Small_CoOp_C", "Flow-Infused Pine S"},
        {"BP_TreeGrowable_Pine_Medium_CoOp_C", "Flow-Infused Pine M"},
        {"BP_TreeGrowable_Pine_Large_CoOp_C", "Flow-Infused Pine L"},
        {"BP_TreeGrowable_Pine_Large2_CoOp_C", "Flow-Infused Pine L"},

        // Shrubs
        {"BP_ShrubChoppable_Bush_C", "Bush"},

        // Shrubs (Foliage On Plot)
        {"BP_FoliageOnPlot_Bush_C", "Bush"},

        // Ores //

        // Stone
        { "BP_Mining_Stone_Small_C", "Stone S" },
        { "BP_Mining_Stone_Medium_C", "Stone M" },
        { "BP_Mining_Stone_Large_C", "Stone L" },
        { "BP_Mining_Stone_Small_SingleHarvest_C", "Stone S" },
        { "BP_Mining_Stone_Medium_SingleHarvest_C", "Stone M" },
        { "BP_Mining_Stone_Large_SingleHarvest_C", "Stone L" },
        { "BP_Mining_Stone_Small_MultiHarvest_C", "Stone S" },
        { "BP_Mining_Stone_Medium_MultiHarvest_C", "Stone M" },
        { "BP_Mining_Stone_Large_MultiHarvest_C", "Stone L" },

        // Stone (Foliage On Plot)
        { "BP_FoliageOnPlot_Stone_C", "Stone S" },
        { "BP_FoliageOnPlot_RockL_C", "Stone L" },

        // Copper
        { "BP_Mining_Copper_Small_C", "Copper S" },
        { "BP_Mining_Copper_Medium_C", "Copper M" },
        { "BP_Mining_Copper_Large_C", "Copper L" },
        { "BP_Mining_Copper_Small_SingleHarvest_C", "Copper S" },
        { "BP_Mining_Copper_Medium_SingleHarvest_C", "Copper M" },
        { "BP_Mining_Copper_Large_SingleHarvest_C", "Copper L" },
        { "BP_Mining_Copper_Small_MultiHarvest_C", "Copper S" },
        { "BP_Mining_Copper_Medium_MultiHarvest_C", "Copper M" },
        { "BP_Mining_Copper_Large_MultiHarvest_C", "Copper L" },

        // Iron
        { "BP_Mining_Iron_Small_C", "Iron S" },
        { "BP_Mining_Iron_Medium_C", "Iron M" },
        { "BP_Mining_Iron_Large_C", "Iron L" },
        { "BP_Mining_Iron_Small_SingleHarvest_C", "Iron S" },
        { "BP_Mining_Iron_Medium_SingleHarvest_C", "Iron M" },
        { "BP_Mining_Iron_Large_SingleHarvest_C", "Iron L" },
        { "BP_Mining_Iron_Small_MultiHarvest_C", "Iron S" },
        { "BP_Mining_Iron_Medium_MultiHarvest_C", "Iron M" },
        { "BP_Mining_Iron_Large_MultiHarvest_C", "Iron L" },

        // Palium
        { "BP_Mining_Palium_C", "Palium" },
        { "BP_Mining_Palium_Small_C", "Palium S" },
        { "BP_Mining_Palium_Medium_C", "Palium M" },
        { "BP_Mining_Palium_Large_C", "Palium L" },
        { "BP_Mining_Palium_SingleHarvest_C", "Palium" },
        { "BP_Mining_Palium_Small_SingleHarvest_C", "Palium S" },
        { "BP_Mining_Palium_Medium_SingleHarvest_C", "Palium M" },
        { "BP_Mining_Palium_Large_SingleHarvest_C", "Palium L" },
        { "BP_Mining_Palium_MultiHarvest_C", "Palium" },
        { "BP_Mining_Palium_Small_MultiHarvest_C", "Palium S" },
        { "BP_Mining_Palium_Medium_MultiHarvest_C", "Palium M" },
        { "BP_Mining_Palium_Large_MultiHarvest_C", "Palium L" },

        // Clay
        {"BP_Mining_Clay_C", "Clay"},
        {"BP_Mining_Clay_SingleHarvest_C", "Clay"},
        {"BP_Mining_Clay_MultiHarvest_C", "Clay"},

        // Silver
        {"BP_Mining_Silver_C", "Silver"},
        {"BP_Mining_Silver_SingleHarvest_C", "Silver"},
        {"BP_Mining_Silver_MultiHarvest_C", "Silver"},

        // Gold
        {"BP_Mining_Gold_C", "Gold"},
        {"BP_Mining_Gold_SingleHarvest_C", "Gold"},
        {"BP_Mining_Gold_MultiHarvest_C", "Gold"},

        // Creatures //

        // Sernuk
        { "BP_ValeriaHuntingCreature_Cearnuk_T1_C", "Sernuk" },
        { "BP_ValeriaHuntingCreature_Cearnuk_T2_C", "Elder Sernuk" },
        { "BP_ValeriaHuntingCreature_Cearnuk_T3_C", "Proudhorn Sernuk" },

        // Chaapa
        { "BP_ValeriaHuntingCreature_Chapaa_T1_C", "Chapaa" },
        { "BP_ValeriaHuntingCreature_Chapaa_T2_C", "Striped Chapaa" },
        { "BP_ValeriaHuntingCreature_Chapaa_T3_C", "Azure Chapaa" },
        { "BP_ValeriaHuntingCreature_Chapaa_T3_MirrorImage_C", "(Fake) Chapaa" },

        // Chaapa (Minigame Event)
        { "BP_ValeriaHuntingCreature_ChapaaChase_Base_C", "Chapaa Chase - Base" },
        { "BP_ValeriaHuntingCreature_ChapaaChase_Fast_C", "Chapaa Chase - Fast" },

        // Muujin
        { "BP_ValeriaHuntingCreature_TreeClimber_T1_C", "Muujin" },
        { "BP_ValeriaHuntingCreature_TreeClimber_T2_C", "Banded Muujin" },
        { "BP_ValeriaHuntingCreature_TreeClimber_T3_C", "Bluebristle Muujin" },

        // Bugs //

        // Bee
        {"BP_Bug_BeeU_C", "Bahari Bee"},
        {"BP_Bug_BeeU+_C", "Bahari Bee *"},
        {"BP_Bug_BeeR_C", "Golden Glory Bee"},
        {"BP_Bug_BeeR+_C", "Golden Glory Bee *"},

        // Beetle
        {"BP_Bug_BeetleC_C", "Spotted Stink Bug"},
        {"BP_Bug_BeetleC+_C", "Spotted Stink Bug *"},
        {"BP_Bug_BeetleU_C", "Proudhorned Stag Beetle"},
        {"BP_Bug_BeetleU+_C", "Proudhorned Stag Beetle *"},
        {"BP_Bug_BeetleR_C", "Raspberry Beetle"},
        {"BP_Bug_BeetleR+_C", "Raspberry Beetle *"},
        {"BP_Bug_BeetleE_C", "Ancient Amber Beetle"},
        {"BP_Bug_BeetleE+_C", "Ancient Amber Beetle *"},

        // Butterfly
        {"BP_Bug_ButterflyC_C", "Common Blue Butterfly"},
        {"BP_Bug_ButterflyC+_C", "Common Blue Butterfly *"},
        {"BP_Bug_ButterflyU_C", "Duskwing Butterfly"},
        {"BP_Bug_ButterflyU+_C", "Duskwing Butterfly *"},
        {"BP_Bug_ButterflyR_C", "Brighteye Butterfly"},
        {"BP_Bug_ButterflyR+_C", "Brighteye Butterfly *"},
        {"BP_Bug_ButterflyE_C", "Rainbow-Tipped Butterfly"},
        {"BP_Bug_ButterflyE+_C", "Rainbow-Tipped Butterfly *"},

        // Cicada
        {"BP_Bug_CicadaC_C", "Common Bark Cicada"},
        {"BP_Bug_CicadaC+_C", "Common Bark Cicada *"},
        {"BP_Bug_CicadaU_C", "Cerulean Cicada"},
        {"BP_Bug_CicadaU+_C", "Cerulean Cicada *"},
        {"BP_Bug_CicadaR_C", "Spitfire Cicada"},
        {"BP_Bug_CicadaR+_C", "Spitfire Cicada *"},

        // Crab
        {"BP_Bug_CrabC_C", "Bahari Crab"},
        {"BP_Bug_CrabC+_C", "Bahari Crab *"},
        {"BP_Bug_CrabU_C", "Spineshell Crab"},
        {"BP_Bug_CrabU+_C", "Spineshell Crab *"},
        {"BP_Bug_CrabR_C", "Vampire Crab"},
        {"BP_Bug_CrabR+_C", "Vampire Crab *"},

        // Cricket
        {"BP_Bug_CricketC_C", "Common Field Cricket"},
        {"BP_Bug_CricketC+_C", "Common Field Cricket *"},
        {"BP_Bug_CricketU_C", "Garden Leafhopper"},
        {"BP_Bug_CricketU+_C", "Garden Leafhopper *"},
        {"BP_Bug_CricketR_C", "Azure Stonehopper"},
        {"BP_Bug_CricketR+_C", "Azure Stonehopper *"},

        // Dragonfly
        {"BP_Bug_DragonflyC_C", "Brushtail Dragonfly"},
        {"BP_Bug_DragonflyC+_C", "Brushtail Dragonfly *"},
        {"BP_Bug_DragonflyU_C", "Inky Dragonfly"},
        {"BP_Bug_DragonflyU+_C", "Inky Dragonfly *"},
        {"BP_Bug_DragonflyR_C", "Firebreathing Dragonfly"},
        {"BP_Bug_DragonflyR+_C", "Firebreathing Dragonfly *"},
        {"BP_Bug_DragonflyE_C", "Jewelwing Dragonfly"},
        {"BP_Bug_DragonflyE+_C", "Jewelwing Dragonfly *"},

        // Glowbug
        {"BP_Bug_GlowbugC_C", "Paper Lantern Bug"},
        {"BP_Bug_GlowbugC+_C", "Paper Lantern Bug *"},
        {"BP_Bug_GlowbugR_C", "Bahari Glowbug"},
        {"BP_Bug_GlowbugR+_C", "Bahari Glowbug *"},

        // Ladybug
        {"BP_Bug_LadybugU_C", "Garden Ladybug"},
        {"BP_Bug_LadybugU+_C", "Garden Ladybug *"},
        {"BP_Bug_LadybugR_C", "Princess Ladybug"},
        {"BP_Bug_LadybugR+_C", "Princess Ladybug *"},

        // Mantis
        {"BP_Bug_MantisU_C", "Garden Mantis"},
        {"BP_Bug_MantisU+_C", "Garden Mantis *"},
        {"BP_Bug_MantisR1_C", "Spotted Mantis"},
        {"BP_Bug_MantisR1+_C", "Spotted Mantis *"},
        {"BP_Bug_MantisR2_C", "Leafstalker Mantis"},
        {"BP_Bug_MantisR2+_C", "Leafstalker Mantis *"},
        {"BP_Bug_MantisE_C", "Fairy Mantis"},
        {"BP_Bug_MantisE+_C", "Fairy Mantis *"},

        // Moth
        {"BP_Bug_MothC_C", "Kilima Night Moth"},
        {"BP_Bug_MothC+_C", "Kilima Night Moth *"},
        {"BP_Bug_MothU_C", "Lunar Fairy Moth"},
        {"BP_Bug_MothU+_C", "Lunar Fairy Moth *"},
        {"BP_Bug_MothR_C", "Gossamer Veil Moth"},
        {"BP_Bug_MothR+_C", "Gossamer Veil Moth *"},

        // Pede
        {"BP_Bug_PedeU_C", "Garden Millipede"},
        {"BP_Bug_PedeU+_C", "Garden Millipede *"},
        {"BP_Bug_PedeR1_C", "Hairy Millipede"},
        {"BP_Bug_PedeR1+_C", "Hairy Millipede *"},
        {"BP_Bug_PedeR2_C", "Scintillating Centipede"},
        {"BP_Bug_PedeR2+_C", "Scintillating Centipede *"},

        // Snail
        {"BP_Bug_SnailU_C", "Garden Snail"},
        {"BP_Bug_SnailU+_C", "Garden Snail *"},
        {"BP_Bug_SnailR_C", "Stripeshell Snail"},
        {"BP_Bug_SnailR+_C", "Stripeshell Snail *"},

        // Forageables
        {"BP_Coral_C", "Coral"},
        {"BP_Gatherable_MushroomBlue+_C", "Brightshroom *"},
        {"BP_Gatherable_MushroomBlue_C", "Brightshroom"},
        {"BP_Gatherable_MushroomR+_C", "Mountain Morel *"},
        {"BP_Gatherable_MushroomR_C", "Mountain Morel"},
        {"BP_HeartdropLilly+_C", "Heartdrop Lily *"}, // have yet to see a SQ
        {"BP_HeartdropLilly_C", "Heartdrop Lily"},
        {"BP_Moss_DragonsBeard+_C", "Dragon's Beard Peat *"},
        {"BP_Moss_DragonsBeard_C", "Dragon's Beard Peat"},
        {"BP_Moss_EmeraldCarpet+_C", "Emerald Carpet Moss *"},
        {"BP_Moss_EmeraldCarpet_C", "Emerald Carpet Moss"},
        {"BP_Oyster_C", "Unopened Oyster"},
        {"BP_PoisonFlower+_C", "Briar Daisy *"},
        {"BP_PoisonFlower_C", "Briar Daisy"},
        {"BP_Seashell_C", "Shell"},
        {"BP_Spice_DariCloves+_C", "Dari Cloves *"},
        {"BP_Spice_DariCloves_C", "Dari Cloves"},
        {"BP_Spice_HeatRoot+_C", "Heat Root *"},
        {"BP_Spice_HeatRoot_C", "Heat Root"},
        {"BP_Spiced_Sprouts+_C", "Spice Sprouts *"},
        {"BP_Spiced_Sprouts_C", "Spice Sprouts"},
        {"BP_SundropLillies+_C", "Sundrop Lily *"},
        {"BP_SundropLillies_C", "Sundrop Lily"},
        {"BP_SweetLeaves+_C", "Sweet Leaf *"},
        {"BP_SweetLeaves_C", "Sweet Leaf"},
        {"BP_WaterFlower+_C", "Crystal Lake Lotus *"},
        {"BP_WaterFlower_C", "Crystal Lake Lotus"},
        {"BP_WildGarlic+_C", "Wild Garlic *"},
        {"BP_WildGarlic_C", "Wild Garlic"},
        {"BP_Gatherable_Ginger+_C", "Wild Ginger *"},
        {"BP_Gatherable_Ginger_C", "Wild Ginger"},
        {"BP_Gatherable_GreenOnion+_C", "Wild Green Onion *"},
        {"BP_Gatherable_GreenOnion_C", "Wild Green Onion"},

        // Fishing Nodes
        {"BP_WaterPlane_Fishing_OceanAZ1_SQ_C", "Coast"},
        {"BP_WaterPlane_Fishing_PondVillage_SQ_C", "Pond"},
        {"BP_WaterPlane_Fishing_LakeVillage_SQ_C", "Lake"},
        {"BP_WaterPlane_Fishing_RiverVillage_SQ_C", "Kilima River"},
        {"BP_WaterPlane_Fishing_RiverAZ1_SQ_C", "Bahari River"},
        {"BP_WaterPlane_Fishing_AZ1_Cave_SQ_C", "Cave"},

        // Fish
        {"BP_FishingJackpot", "Waterlogged Chest"},
        {"BP_FishingRecipeBook", "Recipe Book"},
        {"BP_Fish_AlbinoEeel", "Albino Eeel"},
        {"BP_Fish_Alligator_Gar", "Alligator Gar"},
        {"BP_Fish_AncientFish", "Ancient Fish"},
        {"BP_Fish_BahariBream", "Bahari Bream"},
        {"BP_Fish_Bahari_Pike", "Bahari Pike"},
        {"BP_Fish_Barracuda", "Barracuda"},
        {"BP_Fish_BatRay", "Bat Ray"},
        {"BP_Fish_Beluga_Sturgeon", "Beluga Sturgeon"},
        {"BP_Fish_Bigeye_Tuna", "Bigeye_Tuna"},
        {"BP_Fish_Black_Sea_Bass", "Black Sea Bass"},
        {"BP_Fish_Blobfish", "Blobfish"},
        {"BP_Fish_BlueMarlin", "Blue Marlin"},
        {"BP_Fish_Blue_Spotted_Ray", "Blue Spotted Ray"},
        {"BP_Fish_Cactus_Moray", "Cactus Moray"},
        {"BP_Fish_CalicoKoi", "Calico Koi"},
        {"BP_Fish_CantankerousKoi", "CantankerousKoi"},
        {"BP_Fish_Channel_Catfish", "Channel Catfish"},
        {"BP_Fish_Chub", "Chub"},
        {"BP_Fish_Cloudfish", "Cloudfish"},
        {"BP_Fish_Codfish", "Codfish"},
        {"BP_Fish_Crimson_Fangtooth", "Crimson Fangtooth"},
        {"BP_Fish_Crucian_Carp", "Crucian Carp"},
        {"BP_Fish_Cutthroat_Trout", "Cutthroat Trout"},
        {"BP_Fish_Dawnray", "Dawnray"},
        {"BP_Fish_Duskray", "Duskray"},
        {"BP_Fish_Enchanted_Pupfish", "Enchanted Pupfish"},
        {"BP_Fish_Energized_Piranha", "Energized Piranha"},
        {"BP_Fish_EyelessMinnow", "Eyeless Minnow"},
        {"BP_Fish_FairyCarp", "Fairy Carp"},
        {"BP_Fish_Fathead_Minnow", "Fathead Minnow"},
        {"BP_Fish_Flametongue_Ray", "Flametongue Ray"},
        {"BP_Fish_FreshwaterEel", "Freshwater Eel"},
        {"BP_Fish_Giant_Goldfish", "Giant Goldfish"},
        {"BP_Fish_Giant_Kilima_Stingray", "Giant Kilima Stingray"},
        {"BP_Fish_Gillyfin", "Gillyfin"},
        {"BP_Fish_GoldenSalmon", "Golden Salmon"},
        {"BP_Fish_Honey_Loach", "Honey Loach"},
        {"BP_Fish_Hypnotic_Moray", "Hypnotic Moray"},
        {"BP_Fish_Indigo_Lamprey", "Indigo Lamprey"},
        {"BP_Fish_Kenjis_Carp", "Kenli's Carp"},
        {"BP_Fish_KilimaCatFish", "Kilima Catfish"},
        {"BP_Fish_Kilima_Grayling", "Kilima Grayling"},
        {"BP_Fish_Kilima_Redfin", "Kilima Redfin"},
        {"BP_Fish_LargemouthBass", "Largemouth Bass"},
        {"BP_Fish_Midnight_Paddlefish", "Midnight Paddlefish"},
        {"BP_Fish_Mirror_Carp", "Mirror Carp"},
        {"BP_Fish_Mottled_Gobi", "Mottled Gobi"},
        {"BP_Fish_Mudminnow", "Mudminnow"},
        {"BP_Fish_MutatedAngler", "Mutated Angler"},
        {"BP_Fish_Oily_Anchovy", "Oily Anchovy"},
        {"BP_Fish_Orange_Bluegill", "Orange Bluegill"},
        {"BP_Fish_Paddlefish", "Paddlefish"},
        {"BP_Fish_PaintedPerch", "Painted Perch"},
        {"BP_Fish_Platinum_Chad", "Platinum Chad"},
        {"BP_Fish_PrismTrout", "Prism Trout"},
        {"BP_Fish_RadiantSunfish", "Radiant Sunfish"},
        {"BP_Fish_Rainbow_Trout", "Rainbow Trout"},
        {"BP_Fish_Red-bellied_piranha", "Red-bellied Piranha"},
        {"BP_Fish_RibbontailRay", "Ribbontail Ray"},
        {"BP_Fish_Rosy_Bitterling", "Rosy Bitterling"},
        {"BP_Fish_Sardine", "Sardine"},
        {"BP_Fish_Scarlet_Koi", "Scarlet Koi"},
        {"BP_Fish_Shimmerfin", "Shimmerfin"},
        {"BP_Fish_SilverSalmon", "Silver Salmon"},
        {"BP_Fish_Silvery_Minnow", "Silvery Minnow"},
        {"BP_Fish_SmallmouthBass", "Smallmouth Bass"},
        {"BP_Fish_SpottedBullhead", "Spotted Bullhead"},
        {"BP_Fish_Stalking_Catfish", "Stalking Catfish"},
        {"BP_Fish_Stickleback", "Stickleback"},
        {"BP_Fish_Stonefish", "Stonefish"},
        {"BP_Fish_Stormray", "Stormray"},
        {"BP_Fish_StripedSturgeon", "Striped Sturgeon"},
        {"BP_Fish_Striped_Dace", "Striped Dace"},
        {"BP_Fish_Swordfin_Eel", "Swordfin Eel"},
        {"BP_Fish_Thundering_Eel", "Thundering Eel"},
        {"BP_Fish_UmbranCarp", "Umbran Carp"},
        {"BP_Fish_UnicornFish", "Unicorn Fish"},
        {"BP_Fish_Voidray", "Voidray"},
        {"BP_Fish_Willow_Lamprey", "Willow Lamprey"},
        {"BP_Fish_Yellowfin_Tuna", "Yellowfin Tuna"},
        {"BP_Fish_Yellow_Perch", "Yellow Perch"},
        {"BP_Trash_Shipfragments", "Ship Fragments"},
        {"BP_Trash_Wagonwheel", "Wagon Wheel"},
        {"BP_Trash_WaterloggedBoot", "Waterlogged Boot"},

        // NPCs (Villagers)
        { "BP_Villager_Miner_C", "Hodari" },
        { "BP_VillagerTheArchaeologist_C", "Jina" },
        { "BP_Villager_Cook_C", "Reth" },
        { "BP_Villager_Tish_C", "Tish" },
        { "BP_VillagerTheDemolitionist_C", "Najuma" },
        { "BP_Villager_Healer_C", "Chayne" },
        { "BP_Villager_Elouisa_C", "Elouisa" },
        { "BP_Villager_TheMagistrate_C", "Eshe" },
        { "BP_Villager_TheLibrarian_C", "Caleri" },
        { "BP_Villager_Tamala_C", "Tamala" },
        { "BP_Villager_Blacksmith_C", "Sifuu" },
        { "BP_Villager_Farmer_C", "Badruu" },
        { "BP_VillagerDeliveryBoy_C", "Auni" },
        { "BP_Villager_Farmboy_C", "Nai'o" },
        { "BP_Villager_Tau_C", "Tau" },
        { "BP_VillagerTheInnKeeper_C", "Ashura" },
        { "BP_Villager_Hekla_C", "Hekla" },
        { "BP_Villager_Zeki_C", "Zeki" },
        { "BP_Villager_Rancher_C", "Delaila" },
        { "BP_VillagerFisherman_C", "Einar" },
        { "BP_Villager_Kenyatta_C", "Kenyatta" },
        { "BP_Villager_Mayor_C", "Kenli" },
        { "BP_Villager_Jel_C", "Jel" },
        { "BP_VillagerTheHunter_C", "Hassian" },
        { "BP_VillagerTheWatcher_C", "Subira" },

        // Loot
        { "BP_Hunting_LootBag_C", "Animal Loot" },
        { "BP_InsectBallLoot_C", "Bug Loot" },
        { "BP_LootChestRockPile_C", "Stone Loot" },
        { "BP_LootChestClayPile_C", "Clay Loot" },
        { "BP_LootChestCopperPile_C", "Copper Loot" },
        { "BP_LootChestIronPile_C", "Iron Loot" },
        { "BP_LootChestPaliumPile_C", "Palium Loot" },
        { "BP_LootChestWoodBundle_Oak_C", "Sapwood Loot" },
        { "BP_LootChestWoodBundle_Birch_C", "Sapwood Loot" },
        { "BP_LootChestWoodBundle_Juniper_C", "Sapwood Loot" },
        { "BP_LootChestWoodBundle_Pine_C", "Heartwood Loot" },
        { "BP_LootChestWoodBundle_Enchanted_C", "Flow Loot" },

        { "BP_ChapaaPile_C", "Rummage Pile" },
        { "BP_BeachPile_C", "Rummage Pile" },

        // Treasure
        { "BP_ShinyPebble_Orange_C", "Shiny Orange Pebble" },
        { "BP_ShinyPebble_Yellow_C", "Shiny Yellow Pebble" },
        { "BP_ShinyPebble_Blue_C", "Shiny Blue Pebble" },
        { "BP_ShinyPebble_Green_C", "Shiny Green Pebble" },
        { "BP_ShinyPebble_Purple_C", "Shiny Purple Pebble" },
        { "BP_WorldPersistGatherable_Base_Small_C", "Bronze Treasure Chest" },
        { "BP_WorldPersistGatherable_Base_Med_C", "Silver Treasure Chest" },
        { "BP_WorldPersistGatherable_Base_Large_C", "Gold Treasure Chest" },
        { "BP_WorldPersistGatherable_Winterlights_C", "Winterlights Treasure Chest" },

        {"BP_Stables_Sign_C", "Stables - Fast Travel"},
        {"BP_Stables_FrontGate_01_C", "_FrontGate_"}, // Generic name on purpose
        {"BP_Stables_FrontGate_02_C", "_FrontGate_"}, // Generic name on purpose
    };

    // Search map for assigning gatherable size
    std::map<EGatherableSize, std::vector<std::string>> GATHERABLE_SIZE_MAPPINGS = {
        {EGatherableSize::Small, {"_Small_", "_Sapling_"}},
        {EGatherableSize::Medium, {"_Medium_"}},
        {EGatherableSize::Large, {"_Large_", "_Large2_"}},
        {EGatherableSize::Bush, {"_Bush_"}},
    };

    // Search map for assigning tree type, assigned by loot type vs tree species
    std::map<ETreeType, std::vector<std::string>> TREE_TYPE_MAPPINGS = {
        {ETreeType::Flow, {"_CoOp_"}},
        {ETreeType::Heartwood, {"_Pine_"}},
        {ETreeType::Sapwood, {"_Juniper_", "_Oak_", "_Birch_"}},
        {ETreeType::Bush, {"_Bush_"}}
    };

    // Search map for assigning gatherable flags
    std::map<EOreType, std::vector<std::string>> MINING_TYPE_MAPPINGS = {
        {EOreType::Stone, {"_Stone_"}},
        {EOreType::Copper, {"_Copper_"}},
        {EOreType::Clay, {"_Clay_"}},
        {EOreType::Iron, {"_Iron_"}},
        {EOreType::Silver, {"_Silver_"}},
        {EOreType::Gold, {"_Gold_"}},
        {EOreType::Palium, {"_Palium_"}}
    };

    // Search map for assigning forageable type
    std::map<EForageableType, std::vector<std::string>> FORAGEABLE_TYPE_MAPPINGS = {
        {EForageableType::Oyster, {"_Oyster"}},
        {EForageableType::Coral, {"_Coral"}},
        {EForageableType::MushroomBlue, {"_MushroomBlue"}},
        {EForageableType::MushroomRed, {"_MushroomR"}},
        {EForageableType::Heartdrop, {"_HeartdropLilly"}},
        {EForageableType::DragonsBeard, {"_DragonsBeard"}},
        {EForageableType::EmeraldCarpet, {"_EmeraldCarpet"}},
        {EForageableType::PoisonFlower, {"_PoisonFlower"}},
        {EForageableType::Shell, {"_Seashell"}},
        {EForageableType::DariCloves, {"_DariCloves"}},
        {EForageableType::HeatRoot, {"_HeatRoot"}},
        {EForageableType::SpicedSprouts, {"_Spiced_Sprouts"}},
        {EForageableType::Sundrop, {"_SundropLillies"}},
        {EForageableType::SweetLeaves, {"_SweetLeaves"}},
        {EForageableType::WaterFlower, {"_WaterFlower"}},
        {EForageableType::Garlic, {"_WildGarlic"}},
        {EForageableType::Ginger, {"_Ginger"}},
        {EForageableType::GreenOnion, {"_GreenOnion"}}
    };

    // Search map for assigning creature kind type
    std::map<ECreatureKind, std::vector<std::string>> CREATURE_KIND_MAPPINGS = {
        {ECreatureKind::Chapaa, {"_Chapaa"}},
        {ECreatureKind::Cearnuk, {"_Cearnuk_"}},
        {ECreatureKind::TreeClimber, {"_TreeClimber"}}
    };

    // Search map for assiging creature quality type
    std::map<ECreatureQuality, std::vector<std::string>> CREATURE_KINDQUALITY_MAPPINGS = {
        {ECreatureQuality::Tier1, {"_T1_C"}},
        {ECreatureQuality::Tier2, {"_T2_C"}},
        {ECreatureQuality::Tier3, {"_T3_C"}},
        {ECreatureQuality::Chase, {"_Base_C", "_Fast_C"}}
    };

    // Search map for assigning bug kind type
    std::map<EBugKind, std::vector<std::string>> CREATURE_BUGKIND_MAPPINGS = {
        {EBugKind::Bee, {"_Bug_BeeU", "_Bug_BeeR"}},
        {EBugKind::Beetle, {"_Bug_Beetle"}},
        {EBugKind::Butterfly, {"_Bug_Butterfly"}},
        {EBugKind::Cicada, {"_Bug_Cicada"}},
        {EBugKind::Crab, {"_Bug_Crab"}},
        {EBugKind::Cricket, {"_Bug_Cricket"}},
        {EBugKind::Dragonfly, {"_Bug_Dragonfly"}},
        {EBugKind::Glowbug, {"_Bug_Glowbug"}},
        {EBugKind::Ladybug, {"_Bug_Ladybug"}},
        {EBugKind::Mantis, {"_Bug_Mantis"}},
        {EBugKind::Moth, {"_Bug_Moth"}},
        {EBugKind::Pede, {"_Bug_Pede"}},
        {EBugKind::Snail, {"_Bug_Snail"}},
    };

    // Search map for assigning bug quality type - NOTE, USE ENDS_WITH INSTEAD OF CONTAINS
    std::map<EBugQuality, std::vector<std::string>> CREATURE_BUGQUALITY_MAPPINGS = {
        {EBugQuality::Common, {"C_C", "C+_C"}},
        {EBugQuality::Uncommon, {"U_C", "U+_C"}},
        {EBugQuality::Rare, {"R_C", "R+_C", "R1_C", "R1+_C"}},
        {EBugQuality::Rare2, {"R2_C", "R2+_C"}},
        {EBugQuality::Epic, {"E_C", "E+_C"}}
    };

    // Search map for assigning fish kind type
    std::map<EFishType, std::vector<std::string>> FISH_TYPE_MAPPINGS = {
        {EFishType::Node, {"_WaterPlane_"}},
        {EFishType::Hook, {"_Fish_", "_Trash_", "_Fishing"}},
    };

    // Villager Textures
    std::map<std::string, std::string> VillagerTextures = {
        {"BP_Villager_Miner_C", "/Game/UI/Icons_Characters/Hodari_Icon.Hodari_Icon"},
        {"BP_VillagerTheArchaeologist_C", "/Game/UI/Icons_Characters/Jina_Icon.Jina_Icon"},
        {"BP_Villager_Cook_C", "/Game/UI/Icons_Characters/Reth_icon.Reth_icon"},
        {"BP_Villager_Tish_C", "/Game/UI/Icons_Characters/Tish_Icon.Tish_Icon"},
        {"BP_VillagerTheDemolitionist_C", "/Game/UI/Icons_Characters/Najuma_Icon.Najuma_Icon"},
        {"BP_Villager_Healer_C", "/Game/UI/Icons_Characters/Chane_Icon.Chane_Icon"},
        {"BP_Villager_Elouisa_C", "/Game/UI/Icons_Characters/Elouisa_Icon1.Elouisa_Icon1"},
        {"BP_Villager_TheMagistrate_C", "/Game/UI/Icons_Characters/Eshe_Icon.Eshe_Icon"},
        {"BP_Villager_TheLibrarian_C", "/Game/UI/Icons_Characters/Calere_Icon.Calere_Icon"},
        {"BP_Villager_Tamala_C", "/Game/UI/Icons_Characters/Tamala_Icon.Tamala_Icon"},
        {"BP_Villager_Blacksmith_C", "/Game/UI/Icons_Characters/Sefu_Icon.Sefu_Icon"},
        {"BP_Villager_Farmer_C", "/Game/UI/Icons_Characters/Badru_Icon.Badru_Icon"},
        {"BP_VillagerDeliveryBoy_C", "/Game/UI/Icons_Characters/Auni_Icon.Auni_Icon"},
        {"BP_Villager_Farmboy_C", "/Game/UI/Icons_Characters/Nyo_Icon.Nyo_Icon"},
        {"BP_Villager_Tau_C", "/Game/UI/Icons/WT_Tau_portrait.WT_Tau_portrait"},
        {"BP_VillagerTheInnKeeper_C", "/Game/UI/Icons_Characters/Ashura_Icon.Ashura_Icon"},
        {"BP_Villager_Hekla_C", "/Game/UI/Icons_Characters/Hekla_Icon.Hekla_Icon"},
        {"BP_Villager_Zeki_C", "/Game/UI/Icons_Characters/Zeki_Icon.Zeki_Icon"},
        {"BP_Villager_Rancher_C", "/Game/UI/Icons_Characters/Dalila_Icon.Dalila_Icon"},
        {"BP_VillagerFisherman_C", "/Game/UI/Icons_Characters/Einar_Icon.Einar_Icon"},
        {"BP_Villager_Kenyatta_C", "/Game/UI/Icons_Characters/Kenyatta_Icon.Kenyatta_Icon"},
        {"BP_Villager_Mayor_C", "/Game/UI/Icons_Characters/Kenji_Icon.Kenji_Icon"},
        {"BP_Villager_Jel_C", "/Game/UI/Icons_Characters/Jel_Icon.Jel_Icon"},
        {"BP_VillagerTheHunter_C", "/Game/UI/Icons_Characters/Hassian_Icon.Hassian_Icon"},
        {"BP_VillagerTheWatcher_C", "/Game/UI/Icons/WT_Subira_Portrait.WT_Subira_Portrait"},
    };

    // Treasure Textures
    std::map<std::string, std::string> TreasureTextures = {
        {"BP_ShinyPebble_Orange_C", "/Game/UI/Icons/Icon_ShinyPebble_orange.Icon_ShinyPebble_orange"},
        {"BP_ShinyPebble_Yellow_C", "/Game/UI/Icons/Icon_ShinyPebble_yellow.Icon_ShinyPebble_yellow"},
        {"BP_ShinyPebble_Blue_C", "/Game/UI/Icons/Icon_ShinyPebble_blue.Icon_ShinyPebble_blue"},
        {"BP_ShinyPebble_Green_C", "/Game/UI/Icons/Icon_ShinyPebble_green.Icon_ShinyPebble_green"},
        {"BP_ShinyPebble_Purple_C", "/Game/UI/Icons/Icon_ShinyPebble_purple.Icon_ShinyPebble_purple"},
        {"BP_WorldPersistGatherable_Base_Small_C", "/Game/UI/Icons/WT_Icon_Chest_Treasure_AncientHuman_Bronze.WT_Icon_Chest_Treasure_AncientHuman_Bronze"},
        {"BP_WorldPersistGatherable_Base_Med_C", "/Game/UI/Icons/WT_Icon_Chest_Treasure_AncientHuman_Silver.WT_Icon_Chest_Treasure_AncientHuman_Silver"},
        {"BP_WorldPersistGatherable_Base_Large_C", "/Game/UI/Icons/WT_Icon_Chest_Treasure_AncientHuman_Gold.WT_Icon_Chest_Treasure_AncientHuman_Gold"},
        {"BP_WorldPersistGatherable_Winterlights_C", "/Game/UI/Icons/WT_Icon_Chest_Treasure_Winterlights.WT_Icon_Chest_Treasure_Winterlights"},
    };

    // Forageables[Type][]
    bool Singles[static_cast<int>(EOneOffs::MAX)] = {};
    unsigned int SingleColors[static_cast<int>(EOneOffs::MAX)] = {
        IM_COL32(0xFF, 0xFF, 0x00, 0xFF),
        IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
        IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
        IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
        IM_COL32(0x00, 0xFF, 0xFF, 0xFF),
        IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
    };

    // Forageables[Type][Starred]
    bool Forageables[static_cast<int>(EForageableType::MAX)][2] = {};
    unsigned int ForageableColors[static_cast<int>(EForageableType::MAX)] = {};
    std::string ForageableTextures[static_cast<int>(EForageableType::MAX)] = {
        "", //Unknown
        "/Game/UI/Icons/Icon_Oyster.Icon_Oyster", //Oyster
        "/Game/UI/Icons/Icon_Coral.Icon_Coral", //Coral
        "/Game/UI/Icons/Icon_Crop_MushroomBlue.Icon_Crop_MushroomBlue", //MushroomBlue
        "/Game/UI/Icons/Icon_Crop_MushroomRed.Icon_Crop_MushroomRed", //MushroomRed
        "/Game/UI/Icons/Icon_Flower_Heartdrop.Icon_Flower_Heartdrop", //Heartdrop
        "/Game/UI/Icons/Icon_Moss_Rare.Icon_Moss_Rare", //DragonsBeard
        "/Game/UI/Icons/Icon_Moss_Common.Icon_Moss_Common", //EmeraldCarpet
        "/Game/UI/Icons/Icon_Flower_Daisy_Briar.Icon_Flower_Daisy_Briar", //PoisonFlower
        "/Game/UI/Icons/Icon_Shell.Icon_Shell", //Shell
        "/Game/UI/Icons/Icon_Flower_DariClove.Icon_Flower_DariClove", //DariCloves
        "/Game/UI/Icons/Icon_Spice_Heatroot.Icon_Spice_Heatroot", //HeatRoot
        "/Game/UI/Icons/Icon_Forage_SpiceSprouts.Icon_Forage_SpiceSprouts", //SpicedSprouts
        "/Game/UI/Icons/Icon_Flower_SundropLily.Icon_Flower_SundropLily", //Sundrop
        "/Game/UI/Icons/Icon_Spice_SweetLeaves.Icon_Spice_SweetLeaves", //SweetLeaves
        "/Game/UI/Icons/Icon_Flower_Lotus.Icon_Flower_Lotus", //WaterFlower
        "/Game/UI/Icons/Icon_Forage_WildGarlic.Icon_Forage_WildGarlic", //Garlic
        "/Game/UI/Icons/Icon_Foragable_Spice_Ginger.Icon_Foragable_Spice_Ginger", //Ginger
        "/Game/UI/Icons/Icon_Foragable_Spice_GreenOnion.Icon_Foragable_Spice_GreenOnion", //GreenOnion
    };

    int ForageableCommon[4] = {
        static_cast<int>(EForageableType::Oyster),
        static_cast<int>(EForageableType::Shell),
        static_cast<int>(EForageableType::Sundrop),
        static_cast<int>(EForageableType::MushroomRed)
    };

    int ForageableUncommon[9] = {
        static_cast<int>(EForageableType::Coral),
        static_cast<int>(EForageableType::PoisonFlower),
        static_cast<int>(EForageableType::WaterFlower),
        static_cast<int>(EForageableType::EmeraldCarpet),
        static_cast<int>(EForageableType::SpicedSprouts),
        static_cast<int>(EForageableType::SweetLeaves),
        static_cast<int>(EForageableType::Garlic),
        static_cast<int>(EForageableType::Ginger),
        static_cast<int>(EForageableType::GreenOnion)
    };

    int ForageableRare[3] = {
        static_cast<int>(EForageableType::DragonsBeard),
        static_cast<int>(EForageableType::MushroomBlue),
        static_cast<int>(EForageableType::HeatRoot)
    };

    int ForageableEpic[2] = {
        static_cast<int>(EForageableType::Heartdrop),
        static_cast<int>(EForageableType::DariCloves)
    };

    // Ores[Type][Size]
    bool Ores[static_cast<int>(EOreType::MAX)][static_cast<int>(EGatherableSize::MAX)] = {};

    unsigned int OreColors[static_cast<int>(EOreType::MAX)] = {
        IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
        IM_COL32(0x88, 0x8C, 0x8D, 0xFF), // Stone
        IM_COL32(0xB8, 0x73, 0x33, 0xFF), // Copper
        IM_COL32(0xAD, 0x50, 0x49, 0xFF), // Clay
        IM_COL32(0xA1, 0x9D, 0x94, 0xFF), // Iron
        IM_COL32(0xAA, 0xA9, 0xAD, 0xFF), // Silver
        IM_COL32(0xDB, 0xAC, 0x34, 0xFF), // Gold
        IM_COL32(0x94, 0xA0, 0xE2, 0xFF), // Palium
    };
    std::string OreTextures[static_cast<int>(EOreType::MAX)] = {
        "", // Unknown
        "/Game/UI/Icons/Icon_Stone.Icon_Stone", // Stone
        "/Game/UI/Icons/Icon_Ore_Copper.Icon_Ore_Copper", // Copper
        "/Game/UI/Icons/Icon_Ore_Clay.Icon_Ore_Clay", // Clay
        "/Game/UI/Icons/Icon_Ore_Iron.Icon_Ore_Iron", // Iron
        "/Game/UI/Icons/Icon_Ore_Silver.Icon_Ore_Silver", // Silver
        "/Game/UI/Icons/Icon_Ore_Gold.Icon_Ore_Gold", // Gold
        "/Game/UI/Icons/Icon_Ore_Palium.Icon_Ore_Palium", // Palium
    };

    // Animals[Type][Size]
    bool Animals[static_cast<int>(ECreatureKind::MAX)][static_cast<int>(ECreatureQuality::MAX)] = {};
    unsigned int AnimalColors[static_cast<int>(ECreatureKind::MAX)][static_cast<int>(ECreatureQuality::MAX)] = {};
    std::string AnimalTextures[static_cast<int>(ECreatureKind::MAX)][static_cast<int>(ECreatureQuality::MAX)] = {
        {}, // Unknown
        {   // Chapaa
            "", // Unknown
            "/Game/UI/Icons/Icon_Material_Chapaa_Tail_T1.Icon_Material_Chapaa_Tail_T1", // Tier1
            "/Game/UI/Icons/Icon_Material_Chapaa_Tail_T2.Icon_Material_Chapaa_Tail_T2", // Tier2
            "/Game/UI/Icons/Icon_Material_Chapaa_Tail_T3.Icon_Material_Chapaa_Tail_T3", // Tier3
        },
        {   // Cearnuk,
            "", // Unknown
            "/Game/UI/Icons/Icon_Material_Cearnuk_Antlers_T1.Icon_Material_Cearnuk_Antlers_T1", // Tier1
            "/Game/UI/Icons/Icon_Material_Cearnuk_Antlers_T2.Icon_Material_Cearnuk_Antlers_T2", // Tier2
            "/Game/UI/Icons/Icon_Material_Cearnuk_Antlers_T3.Icon_Material_Cearnuk_Antlers_T3", // Tier3
        },
        {   // TreeClimber,
            "", // Unknown
            "/Game/UI/Icons/WT_Icon_Material_Muujin_Fur_T1.WT_Icon_Material_Muujin_Fur_T1", // Tier1
            "/Game/UI/Icons/WT_Icon_Material_Muujin_Fur_T2.WT_Icon_Material_Muujin_Fur_T2", // Tier2
            "/Game/UI/Icons/WT_Icon_Material_Muujin_Fur_T3.WT_Icon_Material_Muujin_Fur_T3", // Tier3
        },
    };

    // Bugs[Type][Size][Starred]
    bool Bugs[static_cast<int>(EBugKind::MAX)][static_cast<int>(EBugQuality::MAX)][2] = {};
    unsigned int BugColors[static_cast<int>(EBugKind::MAX)][static_cast<int>(EBugQuality::MAX)] = {};
    std::string BugTextures[static_cast<int>(EBugKind::MAX)][static_cast<int>(EBugQuality::MAX)] = {
        {}, // Unknown
        {   // Bee
            "", // Unknown
            "", // Common
            "/Game/UI/Icons/Icon_Bug_Bee_Bahari.Icon_Bug_Bee_Bahari", // Uncommon
            "/Game/UI/Icons/Icon_Bug_Bee_GoldenGlory.Icon_Bug_Bee_GoldenGlory", // Rare
            "", // Rare2
            "", // Epic
        },
        {   // Beetle
            "", // Unknown
            "/Game/UI/Icons/Icon_Bug_Beetle_SpottedStinkbug.Icon_Bug_Beetle_SpottedStinkbug", // Common
            "/Game/UI/Icons/Icon_Bug_Beetle_ProudhornedStag.Icon_Bug_Beetle_ProudhornedStag", // Uncommon
            "/Game/UI/Icons/Icon_Bug_Beetle_Raspberry.Icon_Bug_Beetle_Raspberry", // Rare
            "", // Rare2
            "/Game/UI/Icons/Icon_Bug_Beetle_AncientAmber.Icon_Bug_Beetle_AncientAmber", // Epic
        },
        {   // Butterfly
            "", // Unknown
            "/Game/UI/Icons/Icon_Bug_Butterfly_CommonBlue.Icon_Bug_Butterfly_CommonBlue", // Common
            "/Game/UI/Icons/Icon_Bug_Butterfly_Duskwing.Icon_Bug_Butterfly_Duskwing", // Uncommon
            "/Game/UI/Icons/Icon_Bug_Butterfly_Brighteye.Icon_Bug_Butterfly_Brighteye", // Rare
            "", // Rare2
            "/Game/UI/Icons/Icon_Bug_Butterfly_RainbowTipped.Icon_Bug_Butterfly_RainbowTipped", // Epic
        },
        {   // Cicada
            "", // Unknown
            "/Game/UI/Icons/Icon_Bug_Cicada_CommonBark.Icon_Bug_Cicada_CommonBark", // Common
            "/Game/UI/Icons/Icon_Bug_Cicada_Cerulean.Icon_Bug_Cicada_Cerulean", // Uncommon
            "/Game/UI/Icons/Icon_Bug_Cicada_Spitfire.Icon_Bug_Cicada_Spitfire", // Rare
            "", // Rare2
            "", // Epic
        },
        {   // Crab
            "", // Unknown
            "/Game/UI/Icons/Icon_Bug_Crab_Bahari.Icon_Bug_Crab_Bahari", // Common
            "/Game/UI/Icons/Icon_Bug_Crab_Spineshell.Icon_Bug_Crab_Spineshell", // Uncommon
            "/Game/UI/Icons/Icon_Bug_Crab_Vampire.Icon_Bug_Crab_Vampire", // Rare
            "", // Rare2
            "", // Epic
        },
        {   // Cricket
            "", // Unknown
            "/Game/UI/Icons/Icon_Bug_Cricket_CommonField.Icon_Bug_Cricket_CommonField", // Common
            "/Game/UI/Icons/Icon_Bug_Cricket_GardenLeafhopper.Icon_Bug_Cricket_GardenLeafhopper", // Uncommon
            "/Game/UI/Icons/Icon_Bug_Cricket_AzureStonehopper.Icon_Bug_Cricket_AzureStonehopper", // Rare
            "", // Rare2
            "", // Epic
        },
        {   // Dragonfly
            "", // Unknown
            "/Game/UI/Icons/Icon_Bug_Dragonfly_Brushtail.Icon_Bug_Dragonfly_Brushtail", // Common
            "/Game/UI/Icons/Icon_Bug_Dragonfly_Inky.Icon_Bug_Dragonfly_Inky", // Uncommon
            "/Game/UI/Icons/Icon_Bug_Dragonfly_Firebreathing.Icon_Bug_Dragonfly_Firebreathing", // Rare
            "", // Rare2
            "/Game/UI/Icons/Icon_Bug_Dragonfly_Jewelwing.Icon_Bug_Dragonfly_Jewelwing", // Epic
        },
        {   // Glowbug
            "", // Unknown
            "/Game/UI/Icons/Icon_Bug_Glowbug_PaperLantern.Icon_Bug_Glowbug_PaperLantern", // Common
            "", // Uncommon
            "/Game/UI/Icons/Icon_Bug_Glowbug_Bahari.Icon_Bug_Glowbug_Bahari", // Rare
            "", // Rare2
            "", // Epic
        },
        {   // Ladybug
            "", // Unknown
            "", // Common
            "/Game/UI/Icons/Icon_Bug_Ladybug_Garden.Icon_Bug_Ladybug_Garden", // Uncommon
            "/Game/UI/Icons/Icon_Bug_Ladybug_Princess.Icon_Bug_Ladybug_Princess", // Rare
            "", // Rare2
            "", // Epic
        },
        {   // Mantis
            "", // Unknown
            "", // Common
            "/Game/UI/Icons/Icon_Bug_Mantis_Garden.Icon_Bug_Mantis_Garden", // Uncommon
            "/Game/UI/Icons/Icon_Bug_Mantis_Spotted.Icon_Bug_Mantis_Spotted", // Rare
            "/Game/UI/Icons/Icon_Bug_Mantis_Leafstalker.Icon_Bug_Mantis_Leafstalker", // Rare2
            "/Game/UI/Icons/Icon_Bug_Mantis_Fairy.Icon_Bug_Mantis_Fairy", // Epic
        },
        {   // Moth
            "", // Unknown
            "/Game/UI/Icons/Icon_Bug_Moth_KilimaNight.Icon_Bug_Moth_KilimaNight", // Common
            "/Game/UI/Icons/Icon_Bug_Moth_LunarFairy.Icon_Bug_Moth_LunarFairy", // Uncommon
            "/Game/UI/Icons/Icon_Bug_Moth_GossamerVeil.Icon_Bug_Moth_GossamerVeil", // Rare
            "", // Rare2
            "", // Epic
        },
        {   // Pede
            "", // Unknown
            "", // Common
            "/Game/UI/Icons/Icon_Bug_Millipede_Garden.Icon_Bug_Millipede_Garden", // Uncommon
            "/Game/UI/Icons/Icon_Bug_Millipede_Hairy.Icon_Bug_Millipede_Hairy", // Rare
            "/Game/UI/Icons/Icon_Bug_Millipede_Scintillating.Icon_Bug_Millipede_Scintillating", // Rare2
            "", // Epic
        },
        {   // Snail
            "", // Unknown
            "", // Common
            "/Game/UI/Icons/Icon_Bug_Snail_Garden.Icon_Bug_Snail_Garden", // Uncommon
            "/Game/UI/Icons/Icon_Bug_Snail_Stripeshell.Icon_Bug_Snail_Stripeshell", // Rare
            "", // Rare2
            "", // Epic
        },
    };

    // Trees[Type][Size]
    bool Trees[static_cast<int>(ETreeType::MAX)][static_cast<int>(EGatherableSize::MAX)] = {};
    unsigned int TreeColors[static_cast<int>(ETreeType::MAX)] = {
        IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
        IM_COL32(0x67, 0x00, 0xEA, 0xFF),
        IM_COL32(0x00, 0xFF, 0x00, 0xFF),
        IM_COL32(0x00, 0xFF, 0x00, 0xFF),
        IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
    };
    std::string TreeTextures[static_cast<int>(ETreeType::MAX)] = {
        "", // Unknown
        "/Game/UI/Icons/Icon_Wood_Magicwood.Icon_Wood_Magicwood", // Flow
        "/Game/UI/Icons/Icon_Wood_Hardwood.Icon_Wood_Hardwood", // Heartwood
        "/Game/UI/Icons/Icon_Wood_Softwood.Icon_Wood_Softwood", // Sapwood
        "", // Bush
    };

    // Trees[Type][Size]
    bool Fish[static_cast<int>(EFishType::MAX)] = {};
    unsigned int FishColors[static_cast<int>(EFishType::MAX)] = {
        IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
        IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
        IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
    };

    // ==================================== //

    // Widgets
    UWBP_GameplayUIManager_C* GameplayUIManager = nullptr;

    int TeleportHotkey = VK_XBUTTON1;
    int defaultTeleportHotkey = VK_XBUTTON1;
    bool bWaitingForKeyPress = false;

    float FOV;
    int screenWidth;
    int screenHeight;

    bool TeleportToGatherables = false;

    // ESP Numericals
    std::chrono::steady_clock::time_point LastTeleportToTargetTime;
    std::chrono::steady_clock::time_point LastTimeMapTeleport;

    float SmoothingFactor = 90.0f;
    FVector AimOffset = { -1.5, -1.35, 0 };

    float CurrentAimTime = 0.f;
    double SelectionThreshold = 50.0;

    // Movement Booleans
    bool bEnableNoclip = false;
    bool bPreviousNoclipState = false;

    // Movement Numericals
    int currentMovementModeIndex = 0;
    int globalGameSpeedIndex = 0;

    // Movement Floats
    float velocitySpeedMultiplier = 0.0f;
    float GlobalGameSpeed = 1.0f;
    float NoClipFlySpeed = 600.0f;

    // These will be used for resetting back to default values
    float WalkSpeed = 565.0f; // Default walk speed
    float GameSpeed = 1.0f; // Default game speed
    float SprintSpeedMultiplier = 1.65f; // Default sprint speed multiplier
    float ClimbingSpeed = 80.0f; // Default climbing speed
    float GlidingSpeed = 900.0f; // Default gliding speed
    float GlidingFallSpeed = 250.0f; // Default gliding fall speed
    float JumpVelocity = 700.0f; // Default jump velocity
    float MaxStepHeight = 45.0f; // Default maximum step height

    // These will be set using the configs, can be changed
    float CustomGameSpeed = 1.0f; // Custom, Dynamic game speed

    FName sOverrideFishingSpot;
    bool bCaptureFishingSpot = false;
    bool bOverrideFishingSpot = false;
    bool bEnableAutoFishing = false;
    int bSelectedFishingSpot = 0;
    const char* bFishingSpots[14] = { "- none -","River Village","River Village *","Pond Village","Pond Village *","Lake Village","Lake Village *","Ocean AZ1","Ocean AZ1 *","River AZ1","River AZ1 *","Cave AZ1","Cave AZ1 *","100G Pool" };
    const FString bFishingSpotsFString[13]{ L"RiverVillage",L"RiverVillage_SQ",L"PondVillage",L"PondVillage_SQ",L"LakeVillage",L"LakeVillage_SQ",L"OceanAZ1",L"OceanAZ1_SQ",L"RiverAZ1",L"RiverAZ1_SQ",L"CaveAZ1",L"CaveAZ1_SQ",L"test" };
};