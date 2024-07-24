#pragma once
#include "DetourManager.h"
#include "ActorHandler.h"
#include "menu/menu.hpp"
#include <chrono>
#include <map>
#include <mutex>

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

class PaliaOverlay final : public MenuBase {
protected:
    void DrawHUD() override;
    void DrawOverlay() override;
    void DrawGuiESP() override;
    void DrawGuiFOVCircle() override;

public:
    static void SetupHooks();

    //static std::map<int, std::string> CreatureQualityNames;
    //static std::map<int, std::string> BugQualityNames;
    //static std::map<int, std::string> GatherableSizeNames;
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

    // ==================================== //

    int Delay = 500;
	const char* themes[4] = { "Origin", "Spectrum Light", "Magma", "Custom Theme" };
};