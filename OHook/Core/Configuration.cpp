#include "Core/Configuration.h"
#include "Core/HotkeysManager.h"
#include "Detours/Main/HUDDetours.h"
#include "Overlay/PaliaOverlay.h"
#include "Misc/ImGuiExt.h"

#include <imgui.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <json/json.h>
#include <wrl/internal.h>

#pragma region VarsInit

bool Configuration::ConfigLoaded = false;

// Path for the configuration file
static const std::string configDirectory = R"(C:\ProgramData\OriginPalia\config)";
static const std::string overlayConfigFilePath      = configDirectory + "\\Overlay_settings.json";
static const std::string espConfigFilePath          = configDirectory + "\\ESP_settings.json";
static const std::string aimConfigFilePath          = configDirectory + "\\Aim_settings.json";
static const std::string movementConfigFilePath     = configDirectory + "\\Movement_settings.json";
static const std::string toolskillsConfigFilePath   = configDirectory + "\\ToolSkills_settings.json";
static const std::string sellitemsConfigFilePath    = configDirectory + "\\SellItems_settings.json";
static const std::string modConfigFilePath          = configDirectory + "\\Mod_settings.json";
static const std::string customThemeConfigFilePath  = configDirectory + "\\Custom_theme.json";
static const std::string hotkeysConfigFilePath      = configDirectory + "\\Hotkey_settings.json";

// ESP-Tab
bool Configuration::bEnableESP = true;
float Configuration::ESPTextScale = 1.0f;
double Configuration::CullDistance = 300.0;
bool Configuration::bEnableESPDistance = true;
bool Configuration::bEnableESPDespawnTimer = false;
bool Configuration::bEnableMapESP = false;
bool Configuration::bEnableESPIcons = true;

#pragma region ESP

// ESP Colors
std::map<EOreType, ImU32> Configuration::OreColors = {
    {EOreType::Unknown, IM_COL32(0xFF, 0xFF, 0xFF, 0xFF)},
    {EOreType::Stone, IM_COL32(0x88, 0x8C, 0x8D, 0xFF)}, // Stone
    {EOreType::Copper, IM_COL32(0xB8, 0x73, 0x33, 0xFF)}, // Copper
    {EOreType::Clay, IM_COL32(0xAD, 0x50, 0x49, 0xFF)}, // Clay
    {EOreType::Iron, IM_COL32(0xA1, 0x9D, 0x94, 0xFF)}, // Iron
    {EOreType::Silver, IM_COL32(0xAA, 0xA9, 0xAD, 0xFF)}, // Silver
    {EOreType::Gold, IM_COL32(0xDB, 0xAC, 0x34, 0xFF)}, // Gold
    {EOreType::Palium, IM_COL32(0x94, 0xA0, 0xE2, 0xFF)} // Palium
};

std::map<EOneOffs, ImU32> Configuration::SingleColors = {
    {EOneOffs::Player, IM_COL32(0xFF, 0x63, 0x47, 0xFF)}, // Tomato Red
    {EOneOffs::NPC, IM_COL32(0xDE, 0xB8, 0x87, 0xFF)}, // Burly Wood
    {EOneOffs::FishHook, IM_COL32(0xC0, 0xC0, 0xC0, 0xFF)}, // Washed Gray
    {EOneOffs::FishPool, IM_COL32(0xC0, 0xC0, 0xC0, 0xFF)}, // Washed Gray
    {EOneOffs::Loot, IM_COL32(0xEE, 0x82, 0xEE, 0xFF)}, // Violet
    {EOneOffs::Quest, IM_COL32(0xFF, 0xA5, 0x00, 0xFF)}, // Orange
    {EOneOffs::RummagePiles, IM_COL32(0xFF, 0x45, 0x00, 0xFF)}, // Orange Red
    {EOneOffs::Treasure, IM_COL32(0xFF, 0xD7, 0x00, 0xFF)}, // Bright Gold
    {EOneOffs::Stables, IM_COL32(0x88, 0x45, 0x13, 0xFF)}, // Saddle Brown
    {EOneOffs::TimedDrop, IM_COL32(0xDB, 0xAC, 0x34, 0xFF)}, // Gold
    {EOneOffs::Relic, IM_COL32(0xC0, 0xC0, 0xC0, 0xFF)}, // Washed Gray
    {EOneOffs::Others, IM_COL32(0xC0, 0xC0, 0xC0, 0xFF)}, // Washed Gray
};

std::map<ETreeType, ImU32> Configuration::TreeColors = {
    {ETreeType::Unknown, IM_COL32(0xFF, 0xFF, 0xFF, 0xFF)},
    {ETreeType::Flow, IM_COL32(0x67, 0x00, 0xEA, 0xFF)},
    {ETreeType::Heartwood, IM_COL32(0x00, 0xFF, 0x00, 0xFF)},
    {ETreeType::Sapwood, IM_COL32(0x00, 0xFF, 0x00, 0xFF)},
    {ETreeType::Bush, IM_COL32(0xFF, 0xFF, 0xFF, 0xFF)},
};

std::map<EForageableType, ImU32> Configuration::ForageableColors = {
    // Uncommons
    { EForageableType::Oyster, IM_COL32(0xCD, 0xCD, 0xCD, 0xFF)}, // Light Gray
    { EForageableType::Shell, IM_COL32(0xCD, 0xCD, 0xCD, 0xFF)}, // Light Gray
    { EForageableType::Sundrop, IM_COL32(0xCD, 0xCD, 0xCD, 0xFF)}, // Light Gray
    { EForageableType::MushroomRed, IM_COL32(0xCD, 0xCD, 0xCD, 0xFF)}, // Light Gray
    // Commons
    { EForageableType::Coral, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    { EForageableType::PoisonFlower, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    { EForageableType::WaterFlower, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    { EForageableType::EmeraldCarpet, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    { EForageableType::SpicedSprouts, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    { EForageableType::SweetLeaves, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    { EForageableType::Garlic, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    { EForageableType::Ginger, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    { EForageableType::GreenOnion, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    // Rare
    { EForageableType::DragonsBeard, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    { EForageableType::MushroomBlue, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    { EForageableType::HeatRoot, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    // Epic
    { EForageableType::Heartdrop, IM_COL32(0xFF, 0xD7, 0x00, 0xFF)}, // Gold
    { EForageableType::DariCloves, IM_COL32(0xFF, 0xD7, 0x00, 0xFF)}, // Gold
};

std::map<FCreatureType, ImU32> Configuration::AnimalColors = {
    {{ ECreatureKind::Cearnuk, ECreatureQuality::Tier1 }, IM_COL32(0x80, 0x80, 0x80, 0xFF)}, // Light Gray
    {{ ECreatureKind::Cearnuk, ECreatureQuality::Tier2 }, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{ ECreatureKind::Cearnuk, ECreatureQuality::Tier3 }, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue

    {{ ECreatureKind::Chapaa, ECreatureQuality::Tier1 }, IM_COL32(0x80, 0x80, 0x80, 0xFF)}, // Light Gray
    {{ ECreatureKind::Chapaa, ECreatureQuality::Tier2 }, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{ ECreatureKind::Chapaa, ECreatureQuality::Tier3 }, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    {{ ECreatureKind::Chapaa, ECreatureQuality::Chase }, IM_COL32(0xFF, 0xD7, 0x00, 0xFF)}, // Gold

    {{ ECreatureKind::TreeClimber, ECreatureQuality::Tier1 }, IM_COL32(0x80, 0x80, 0x80, 0xFF)}, // Light Gray
    {{ ECreatureKind::TreeClimber, ECreatureQuality::Tier2 }, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{ ECreatureKind::TreeClimber, ECreatureQuality::Tier3 }, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
};

std::map<FBugType, ImU32> Configuration::BugColors = {
    //Bees
    {{EBugKind::Bee, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{EBugKind::Bee, EBugQuality::Rare}, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    //Beetles
    {{EBugKind::Beetle, EBugQuality::Common}, IM_COL32(0x80, 0x80, 0x80, 0xFF)}, // Gray
    {{EBugKind::Beetle, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{EBugKind::Beetle, EBugQuality::Rare}, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    {{EBugKind::Beetle, EBugQuality::Epic}, IM_COL32(0xFF, 0xD7, 0x00, 0xFF)}, // Gold
    //Butterfly
    {{EBugKind::Butterfly, EBugQuality::Common}, IM_COL32(0x80, 0x80, 0x80, 0xFF)}, // Gray
    {{EBugKind::Butterfly, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{EBugKind::Butterfly, EBugQuality::Rare}, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    {{EBugKind::Butterfly, EBugQuality::Epic}, IM_COL32(0xFF, 0xD7, 0x00, 0xFF)}, // Gold
    //Cicada
    {{EBugKind::Cicada, EBugQuality::Common}, IM_COL32(0x80, 0x80, 0x80, 0xFF)}, // Gray
    {{EBugKind::Cicada, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{EBugKind::Cicada, EBugQuality::Rare}, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    //Crab
    {{EBugKind::Crab, EBugQuality::Common}, IM_COL32(0x80, 0x80, 0x80, 0xFF)}, // Gray
    {{EBugKind::Crab, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{EBugKind::Crab, EBugQuality::Rare}, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    //Cricket
    {{EBugKind::Cricket, EBugQuality::Common}, IM_COL32(0x80, 0x80, 0x80, 0xFF)}, // Gray
    {{EBugKind::Cricket, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{EBugKind::Cricket, EBugQuality::Rare}, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    //Dragonfly
    {{EBugKind::Dragonfly, EBugQuality::Common}, IM_COL32(0x80, 0x80, 0x80, 0xFF)}, // Gray
    {{EBugKind::Dragonfly, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{EBugKind::Dragonfly, EBugQuality::Rare}, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    {{EBugKind::Dragonfly, EBugQuality::Epic}, IM_COL32(0xFF, 0xD7, 0x00, 0xFF)}, // Gold
    //Glowbug
    {{EBugKind::Glowbug, EBugQuality::Common}, IM_COL32(0x80, 0x80, 0x80, 0xFF)}, // Gray
    {{EBugKind::Glowbug, EBugQuality::Rare}, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    //Ladybug
    {{EBugKind::Ladybug, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{EBugKind::Ladybug, EBugQuality::Rare}, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    //Mantis
    {{EBugKind::Mantis, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{EBugKind::Mantis, EBugQuality::Rare}, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    {{EBugKind::Mantis, EBugQuality::Rare2}, IM_COL32(0x00, 0xBF, 0xFF, 0xFF)}, // Deep Sky Blue
    {{EBugKind::Mantis, EBugQuality::Epic}, IM_COL32(0xFF, 0xD7, 0x00, 0xFF)}, // Gold
    //Moth
    {{EBugKind::Moth, EBugQuality::Common}, IM_COL32(0x80, 0x80, 0x80, 0xFF)}, // Gray
    {{EBugKind::Moth, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{EBugKind::Moth, EBugQuality::Rare}, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    //Pede
    {{EBugKind::Pede, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{EBugKind::Pede, EBugQuality::Rare}, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
    {{EBugKind::Pede, EBugQuality::Rare2}, IM_COL32(0x00, 0xBF, 0xFF, 0xFF)}, // Deep Sky Blue
    //Snail
    {{EBugKind::Snail, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    {{EBugKind::Snail, EBugQuality::Rare}, IM_COL32(0x1E, 0x90, 0xFF, 0xFF)}, // Dodger Blue
};

#pragma endregion
#pragma endregion



// Aim-Tab

// InteliAim
bool Configuration::bEnableInteliAim = true;
float Configuration::FOVRadius = 120.0f;
bool Configuration::bHideFOVCircle = false;
float Configuration::CircleAlpha = 1.0f;
bool Configuration::bDrawCrosshair = true;

// InteliTarget
bool Configuration::bEnableSilentAimbot = false;
bool Configuration::bTeleportToTargeted = true;
bool Configuration::bAvoidTeleportingToPlayers = true;
bool Configuration::bDoRadiusPlayersAvoidance = true;
int Configuration::TeleportPlayerAvoidanceRadius = 30;



// Movement-Tab

float Configuration::CustomWalkSpeed = 565.0f;
float Configuration::CustomSprintSpeed = 1.65f;
float Configuration::CustomClimbingSpeed = 80.0f;
float Configuration::CustomGlidingSpeed = 900.0f;
float Configuration::CustomGlidingFallSpeed = 250.0f;
float Configuration::CustomJumpVelocity = 700.0f;
int Configuration::CustomMaxJumps = 1;
float Configuration::CustomMaxStepHeight = 45.0f;



// ToolSkills-Tab

// Auto Tool Swinging
bool Configuration::bEnableAutoSwinging = false;
bool Configuration::bResourceAutoEquipTool = false;
bool Configuration::bAvoidGroveChopping = false;

// Auto Bug Catching
bool Configuration::bEnableBugCatching = false;
bool Configuration::bBugAutoEquipTool = false;
bool Configuration::bBugUseRandomDelay = false;
float Configuration::BugCatchingDistance = 5.0f;
int Configuration::BugSpeedPreset = 1;
int Configuration::BugCatchingDelay = 750;

// Auto Animal Hunting
bool Configuration::bEnableAnimalHunting = false;
bool Configuration::bAnimalAutoEquipTool = false;
bool Configuration::bAnimalUseRandomDelay = false;
float Configuration::AnimalHuntingDistance = 5.0f;
int Configuration::AnimalSpeedPreset = 1;
int Configuration::AnimalHuntingDelay = 750;

// Fishing
bool Configuration::bFishingNoDurability = true;
bool Configuration::bFishingMultiplayerHelp = false;
bool Configuration::bFishingInstantCatch = false;
bool Configuration::bFishingPerfectCatch = true;
bool Configuration::bFishingDiscard = false;
bool Configuration::bFishingOpenStoreWaterlogged = false;
bool Configuration::bRequireClickFishing = true;

// Others
bool Configuration::bEnableAutoGather = false;
bool Configuration::bEnableCookingMinigameSkip = false;



// SellItems-Tab

// Buying
bool Configuration::bEnableBuyMultiplier = false;
bool Configuration::bEnableCtrlClickBuy = false;
int Configuration::buyMultiplierValue = 1;

// AutoSell
bool Configuration::bFishingSell = false;
bool Configuration::bFishingSellSQ = false;
bool Configuration::bFishingSellRarity[7] = {false, false, false, false, false, false, false};
bool Configuration::bBugSell = false;
bool Configuration::bBugSellSQ = false;
bool Configuration::bBugSellRarity[7] = {false, false, false, false, false, false, false};
bool Configuration::bHuntingSell = false;
bool Configuration::bHuntingSellSQ = false;
bool Configuration::bHuntingSellRarity[7] = {false, false, false, false, false, false, false};
bool Configuration::bFlowerSell = false;
bool Configuration::bForageableSell = false;
bool Configuration::bForageableSellSQ = false;
bool Configuration::bForageableSellRarity[7] = {false, false, false, false, false, false, false};
bool Configuration::bTreeSell;
bool Configuration::bTreeSellRarity[4] = {false, false, false, false};



// Mods-Tab

// Character
bool Configuration::bEnableAntiAfk = false;
bool Configuration::bEnableUnlimitedWardrobeRespec = false;
bool Configuration::bEnableInteractionMods = false;
float Configuration::InteractionRadius = 500.0f;
bool Configuration::bEnableWaypointTeleport = false;
bool Configuration::bEasyModeActive = false;

// Fun
bool Configuration::bPlaceAnywhere = false;
double Configuration::PlacementRotation = 15.0;
bool Configuration::bEnableOutfitCustomization = false;
bool Configuration::bEnableContentUnlocker = false;
bool Configuration::bEnableFreeLandscape = false;
bool Configuration::bEnableShowWeeklyWants = false;
bool Configuration::bEnableToolBricker = false;
bool Configuration::bEnableRequestMods = false;
bool Configuration::bEnableUnseenItems = false;

bool Configuration::bEnablePrizeWheel = false;
bool Configuration::bPrizeWheelUseLocked = false;
bool Configuration::PrizeWheelSlots[9] = { false, false, false, false, false, false, false, false, false };


// Origin
bool Configuration::bFirstUse = true;
bool Configuration::bShowWatermark = true;
bool Configuration::showEnabledFeaturesWindow = false;
int Configuration::selectedTheme = 0;
std::map<int, ImVec4> Configuration::customColors = {};



// Window configs
float Configuration::windowSizeX = 1450.0f;
float Configuration::windowSizeY = 950.0f;
float Configuration::activeWindowPosX = 125.0f;
float Configuration::activeWindowPosY = 5.0f;



// OVERLAY ~ Loading/Saving
#pragma region OVERLAY_Load_Save

void Configuration::LoadOverlaySettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open overlay settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;
    
    // OVERLAY SETTINGS
    bFirstUse                   = config["OverlaySettings"]["MainWindow"].get("firstUse", true).asBool();
    windowSizeX                 = config["OverlaySettings"]["MainWindow"].get("windowSizeX", 1450.0f).asFloat();
    windowSizeY                 = config["OverlaySettings"]["MainWindow"].get("windowSizeY", 950.0f).asFloat();
    selectedTheme               = config["OverlaySettings"]["MainWindow"].get("selectedTheme", 0).asInt();

    activeWindowPosX            = config["OverlaySettings"]["ModMenu"]["Position"].get("activeWindowPosX", 125.0f).asFloat();
    activeWindowPosY            = config["OverlaySettings"]["ModMenu"]["Position"].get("activeWindowPosY", 5.0f).asFloat();
    showEnabledFeaturesWindow   = config["OverlaySettings"]["ModMenu"]["Toggled"].get("showEnabledFeaturesWindow", false).asBool();

    bShowWatermark              = config["OverlaySettings"]["Settings"].get("bShowWatermark", true).asBool();
}

void Configuration::SaveOverlaySettings() {
    Json::Value config;

    // OVERLAY SETTINGS
    config["OverlaySettings"]["MainWindow"]["firstUse"] = bFirstUse;
    config["OverlaySettings"]["MainWindow"]["windowSizeX"] = windowSizeX;
    config["OverlaySettings"]["MainWindow"]["windowSizeY"] = windowSizeY;
    config["OverlaySettings"]["MainWindow"]["selectedTheme"] = selectedTheme;

    config["OverlaySettings"]["ModMenu"]["Position"]["activeWindowPosX"] = activeWindowPosX;
    config["OverlaySettings"]["ModMenu"]["Position"]["activeWindowPosY"] = activeWindowPosY;
    config["OverlaySettings"]["ModMenu"]["Toggled"]["showEnabledFeaturesWindow"] = showEnabledFeaturesWindow;

    config["OverlaySettings"]["Settings"]["bShowWatermark"] = bShowWatermark;

    std::ofstream configFile(overlayConfigFilePath);
    if (configFile.is_open()) {
        configFile << config.toStyledString();
        configFile.close();
    }
    else {
        std::cerr << "Failed to open Overlay settings file for writing." << '\n';
    }
}

#pragma endregion

// ESP ~ Loading/Saving
#pragma region ESP_Load_Save

void Configuration::LoadESPSettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open esp settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;

    // ESP Settings
    bEnableESP              = config["ESPSettings"].get("bEnableESP", true).asBool();
    ESPTextScale            = config["ESPSettings"].get("ESPTextScale", 1.0f).asFloat();
    CullDistance            = config["ESPSettings"].get("CullDistance", 300.0).asDouble();
    bEnableESPDistance      = config["ESPSettings"].get("bEnableESPDistance", true).asBool();
    bEnableESPDespawnTimer  = config["ESPSettings"].get("bEnableESPDespawnTimer", false).asBool();
    bEnableMapESP           = config["ESPSettings"].get("bEnableMapESP", false).asBool();
    bEnableESPIcons         = config["ESPSettings"].get("bEnableESPIcons", true).asBool();

    // Animals
    for (int i = 0; i < static_cast<int>(ECreatureKind::MAX); i++) {
        for (int j = 0; j < static_cast<int>(ECreatureQuality::MAX); j++) {
            HUDDetours::Animals[i][j] = config["ESPSettings"]["Categories"]["Animals"][ECreatureKindString[i]][ECreatureQualityString[j]].get("Checked", false).asBool();
            if (config["ESPSettings"]["Categories"]["Animals"][ECreatureKindString[i]][ECreatureQualityString[j]].isMember("Color")) {
                ImU32 color = config["ESPSettings"]["Categories"]["Animals"][ECreatureKindString[i]][ECreatureQualityString[j]]["Color"].asUInt();
                AnimalColors[{static_cast<ECreatureKind>(i), static_cast<ECreatureQuality>(j)}] = color;
            }
        }
    }
    
    // Ores
    for (int i = 0; i < static_cast<int>(EOreType::MAX); i++) {
        for (int j = 0; j < static_cast<int>(EGatherableSize::MAX); j++) {
            HUDDetours::Ores[i][j] = config["ESPSettings"]["Categories"]["Ores"][EOreTypeString[i]].get(EGatherableSizeString[j], false).asBool();
        }
        if (config["ESPSettings"]["Categories"]["Ores"][EOreTypeString[i]].isMember("Color")) {
            ImU32 color = config["ESPSettings"]["Categories"]["Ores"][EOreTypeString[i]]["Color"].asUInt();
            OreColors[static_cast<EOreType>(i)] = color;
        }
    }
        
    // Forageables
    for (int i = 0; i < static_cast<int>(EForageableType::MAX); i++) {
        HUDDetours::Forageables[i][0] = config["ESPSettings"]["Categories"]["Forageables"][EForageableTypeString[i]].get("Regular", false).asBool();
        HUDDetours::Forageables[i][1] = config["ESPSettings"]["Categories"]["Forageables"][EForageableTypeString[i]].get("Star Quality", false).asBool();
        if (config["ESPSettings"]["Categories"]["Forageables"][EForageableTypeString[i]].isMember("Color")) {
            ImU32 color = config["ESPSettings"]["Categories"]["Forageables"][EForageableTypeString[i]]["Color"].asUInt();
            ForageableColors[static_cast<EForageableType>(i)] = color;
        }
    }

    // Bugs
    for (int i = 0; i < static_cast<int>(EBugKind::MAX); i++) {
        for (int j = 0; j < static_cast<int>(EBugQuality::MAX); j++) {
            HUDDetours::Bugs[i][j][0] = config["ESPSettings"]["Categories"]["Bugs"][EBugKindString[i]][EBugQualityString[j]].get("Regular", false).asBool();
            HUDDetours::Bugs[i][j][1] = config["ESPSettings"]["Categories"]["Bugs"][EBugKindString[i]][EBugQualityString[j]].get("Star Quality", false).asBool();
            if (config["ESPSettings"]["Categories"]["Bugs"][EBugKindString[i]][EBugQualityString[j]].isMember("Color")) {
                ImU32 color = config["ESPSettings"]["Categories"]["Bugs"][EBugKindString[i]][EBugQualityString[j]]["Color"].asUInt();
                BugColors[{static_cast<EBugKind>(i), static_cast<EBugQuality>(j)}] = color;
            }
        }
    }

    // Trees
    for (int i = 0; i < static_cast<int>(ETreeType::MAX); i++) {
        for (int j = 0; j < static_cast<int>(EGatherableSize::MAX); j++) {
            HUDDetours::Trees[i][j] = config["ESPSettings"]["Categories"]["Trees"][ETreeTypeString[i]].get(EGatherableSizeString[j], false).asBool();
        }
        if (config["ESPSettings"]["Categories"]["Trees"][ETreeTypeString[i]].isMember("Color")) {
            ImU32 color = config["ESPSettings"]["Categories"]["Trees"][ETreeTypeString[i]]["Color"].asUInt();
            TreeColors[static_cast<ETreeType>(i)] = color;
        }
    }

    // Singles
    for (int i = 0; i < static_cast<int>(EOneOffs::MAX); i++) {
        HUDDetours::Singles[i] = config["ESPSettings"]["Categories"]["Singles"][EOneOffsString[i]].get("Checked", false).asBool();
        if (config["ESPSettings"]["Categories"]["Singles"][EOneOffsString[i]].isMember("Color")) {
            ImU32 color = config["ESPSettings"]["Categories"]["Singles"][EOneOffsString[i]]["Color"].asUInt();
            SingleColors[static_cast<EOneOffs>(i)] = color;
        }
    }
}

void Configuration::SaveESPSettings() {
    Json::Value config;

    // ESP SETTINGS
    config["ESPSettings"]["bEnableESP"] = bEnableESP;
    config["ESPSettings"]["bEnableMapESP"] = bEnableMapESP;
    config["ESPSettings"]["bEnableESPIcons"] = bEnableESPIcons;
    config["ESPSettings"]["bEnableESPDistance"] = bEnableESPDistance;
    config["ESPSettings"]["bEnableESPDespawnTimer"] = bEnableESPDespawnTimer;
    config["ESPSettings"]["ESPTextScale"] = ESPTextScale;
    config["ESPSettings"]["CullDistance"] = CullDistance;
    
    // Animals
    for (int i = 1; i < static_cast<int>(ECreatureKind::MAX); i++) { // skip unknown
        for (int j = 1; j < static_cast<int>(ECreatureQuality::MAX); j++) { // skip unknow
            if (HUDDetours::Animals[i][j]) { // only save if true, since we default to false
                config["ESPSettings"]["Categories"]["Animals"][ECreatureKindString[i]][ECreatureQualityString[j]]["Checked"] = true;
            }
            if (auto it = AnimalColors.find(FCreatureType{ static_cast<ECreatureKind>(i), static_cast<ECreatureQuality>(j) }); it != AnimalColors.end()) {
                config["ESPSettings"]["Categories"]["Animals"][ECreatureKindString[i]][ECreatureQualityString[j]]["Color"] = it->second;
            }
        }
    }
    
    // Ores
    for (int i = 1; i < static_cast<int>(EOreType::MAX); i++) { // skip unknown
        if (auto it = OreColors.find(static_cast<EOreType>(i)); it != OreColors.end()) {
            config["ESPSettings"]["Categories"]["Ores"][EOreTypeString[i]]["Color"] = it->second;
        }
        else {
            // if we don't have a color for that type, it's not an ore we're worried about
            continue;
        }
        for (int j = 2; j < static_cast<int>(EGatherableSize::MAX); j++) { //skip unknown and bush
            if (HUDDetours::Ores[i][j]) { // only save if true, since we default to false
                config["ESPSettings"]["Categories"]["Ores"][EOreTypeString[i]][EGatherableSizeString[j]] = true;
            }
        }
    }

    // Forageables
    for (int i = 1; i < static_cast<int>(EForageableType::MAX); i++) { // skip unknown
        if (auto it = ForageableColors.find(static_cast<EForageableType>(i)); it != ForageableColors.end()) {
            config["ESPSettings"]["Categories"]["Forageables"][EForageableTypeString[i]]["Color"] = it->second;
        }
        else {
            // if we don't have a color for that type, it's not an forageable we're worried about
            // this one shouldn't ever happen
            continue;
        }
        if (HUDDetours::Forageables[i][0]) { // only save if true, since we default to false
            config["ESPSettings"]["Categories"]["Forageables"][EForageableTypeString[i]]["Regular"] = true;
        }
        if (HUDDetours::Forageables[i][1]) { // only save if true, since we default to false
            config["ESPSettings"]["Categories"]["Forageables"][EForageableTypeString[i]]["Star Quality"] = true;
        }
    }

    // Bugs
    for (int i = 1; i < static_cast<int>(EBugKind::MAX); i++) { // skip unknown
        for (int j = 1; j < static_cast<int>(EBugQuality::MAX); j++) { // skip unknown
            if (auto it = BugColors.find({static_cast<EBugKind>(i), static_cast<EBugQuality>(j)}); it != BugColors.end()) {
                config["ESPSettings"]["Categories"]["Bugs"][EBugKindString[i]][EBugQualityString[j]]["Color"] = it->second;
            } else {
                // if we don't have a color for that kind & quality it's not a bug so skip
                continue;
            }
            if (HUDDetours::Bugs[i][j][0]) { // only save if true, since we default to false
                config["ESPSettings"]["Categories"]["Bugs"][EBugKindString[i]][EBugQualityString[j]]["Regular"] = true;
            }
            if (HUDDetours::Bugs[i][j][1]) { // only save if true, since we default to false
                config["ESPSettings"]["Categories"]["Bugs"][EBugKindString[i]][EBugQualityString[j]]["Star Quality"] = true;
            }
        }
    }

    // Trees
    for (int i = 1; i < static_cast<int>(ETreeType::MAX); i++) { // skip unknown
        for (int j = 1; j < static_cast<int>(EGatherableSize::MAX); j++) { // skip unknown
            if (HUDDetours::Trees[i][j]) { // only save if true, since we default to false
                config["ESPSettings"]["Categories"]["Trees"][ETreeTypeString[i]][EGatherableSizeString[j]] = true;
            }
        }
        if (auto it = TreeColors.find(static_cast<ETreeType>(i)); it != TreeColors.end()) {
            config["ESPSettings"]["Categories"]["Trees"][ETreeTypeString[i]]["Color"] = it->second;
        }
    }

    // Singles
    for (int i = 0; i < static_cast<int>(EOneOffs::MAX); i++) {
        if (HUDDetours::Singles[i]) {
            config["ESPSettings"]["Categories"]["Singles"][EOneOffsString[i]]["Checked"] = true;
        }
        if (auto it = SingleColors.find(static_cast<EOneOffs>(i)); it != SingleColors.end()) {
            config["ESPSettings"]["Categories"]["Singles"][EOneOffsString[i]]["Color"] = it->second;
        }
    }

    std::ofstream configFile(espConfigFilePath);
    if (configFile.is_open()) {
        configFile << config.toStyledString();
        configFile.close();
    }
    else {
        std::cerr << "Failed to open ESP settings file for writing." << '\n';
    }
}

#pragma endregion

// AIM ~ Loading/Saving
#pragma region AIM_Load_Save

void Configuration::LoadAimSettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open aim settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;

    // INTELIAIM SETTINGS
    bEnableInteliAim    = config["AimSettings"]["FOV"].get("bEnableInteliAim", true).asBool();
    FOVRadius           = config["AimSettings"]["FOV"].get("FOVRadius", 120.0f).asFloat();
    bHideFOVCircle      = config["AimSettings"]["FOV"].get("bHideFOVCircle", false).asBool();
    CircleAlpha         = config["AimSettings"]["FOV"].get("CircleAlpha", 1.0f).asFloat();
    bDrawCrosshair      = config["AimSettings"]["FOV"].get("bDrawCrosshair", true).asBool();

    // INTELITARGET SETTINGS
    bEnableSilentAimbot             = config["AimSettings"]["Target"].get("bEnableSilentAimbot", false).asBool();
    bTeleportToTargeted             = config["AimSettings"]["Target"].get("bTeleportToTargeted", false).asBool();
    bAvoidTeleportingToPlayers      = config["AimSettings"]["Target"].get("bAvoidTeleportingToPlayers", true).asBool();
    bDoRadiusPlayersAvoidance       = config["AimSettings"]["Target"].get("bDoRadiusPlayersAvoidance", true).asBool();
    TeleportPlayerAvoidanceRadius   = config["AimSettings"]["Target"].get("TeleportPlayerAvoidanceRadius", 30).asInt();
}

void Configuration::SaveAimSettings() {
    Json::Value config;

    // INTELIAIM SETTINGS
    config["AimSettings"]["FOV"]["bEnableInteliAim"] = bEnableInteliAim;
    config["AimSettings"]["FOV"]["FOVRadius"] = FOVRadius;
    config["AimSettings"]["FOV"]["bHideFOVCircle"] = bHideFOVCircle;
    config["AimSettings"]["FOV"]["CircleAlpha"] = CircleAlpha;
    config["AimSettings"]["FOV"]["bDrawCrosshair"] = bDrawCrosshair;

    // INTELITARGET SETTINGS
    config["AimSettings"]["Target"]["bEnableSilentAimbot"] = bEnableSilentAimbot;
    config["AimSettings"]["Target"]["bTeleportToTargeted"] = bTeleportToTargeted;
    config["AimSettings"]["Target"]["bAvoidTeleportingToPlayers"] = bAvoidTeleportingToPlayers;
    config["AimSettings"]["Target"]["bDoRadiusPlayersAvoidance"] = bDoRadiusPlayersAvoidance;
    config["AimSettings"]["Target"]["TeleportPlayerAvoidanceRadius"] = TeleportPlayerAvoidanceRadius;
    
    std::ofstream configFile(aimConfigFilePath);
    if (configFile.is_open()) {
        configFile << config.toStyledString();
        configFile.close();
    }
    else {
        std::cerr << "Failed to open Aim settings file for writing." << '\n';
    }
}

#pragma endregion

// MOVEMENT ~ Loading/Saving
#pragma region MOVEMENT_Load_Save

void Configuration::LoadMovementSettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open movement settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;

    // MOVEMENT SETTINGS
    CustomWalkSpeed         = config["MovementSettings"].get("CustomWalkSpeed", 565.0f).asFloat();
    CustomSprintSpeed       = config["MovementSettings"].get("CustomSprintSpeed", 1.65f).asFloat();
    CustomClimbingSpeed     = config["MovementSettings"].get("CustomClimbingSpeed", 80.0f).asFloat();
    CustomGlidingSpeed      = config["MovementSettings"].get("CustomGlidingSpeed", 900.0f).asFloat();
    CustomGlidingFallSpeed  = config["MovementSettings"].get("CustomGlidingFallSpeed", 250.0f).asFloat();
    CustomJumpVelocity      = config["MovementSettings"].get("CustomJumpVelocity", 700.0f).asFloat();
    CustomMaxJumps          = config["MovementSettings"].get("CustomMaxJumps", 1).asInt();
    CustomMaxStepHeight     = config["MovementSettings"].get("CustomMaxStepHeight", 45.0f).asFloat();
}

void Configuration::SaveMovementSettings() {
    Json::Value config;
    
    // MOVEMENT SETTINGS
    config["MovementSettings"]["CustomWalkSpeed"] = CustomWalkSpeed;
    config["MovementSettings"]["CustomSprintSpeed"] = CustomSprintSpeed;
    config["MovementSettings"]["CustomClimbingSpeed"] = CustomClimbingSpeed;
    config["MovementSettings"]["CustomGlidingSpeed"] = CustomGlidingSpeed;
    config["MovementSettings"]["CustomGlidingFallSpeed"] = CustomGlidingFallSpeed;
    config["MovementSettings"]["CustomJumpVelocity"] = CustomJumpVelocity;
    config["MovementSettings"]["CustomMaxJumps"] = CustomMaxJumps;
    config["MovementSettings"]["CustomMaxStepHeight"] = CustomMaxStepHeight;
    
    std::ofstream configFile(movementConfigFilePath);
    if (configFile.is_open()) {
        configFile << config.toStyledString();
        configFile.close();
    }
    else {
        std::cerr << "Failed to open Movement settings file for writing." << '\n';
    }
}

#pragma endregion

// TOOL_SKILLS ~ Loading/Saving
#pragma region TOOLSKILLS_Load_Save

void Configuration::LoadToolSkillsSettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open ToolSkills settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;

    // AXE/PICKAXE SETTINGS
    bEnableAutoSwinging     = config["ToolSkillsSettings"]["ToolSwinger"].get("bEnableAutoSwinging", false).asBool();
    bResourceAutoEquipTool  = config["ToolSkillsSettings"]["ToolSwinger"].get("bResourceAutoEquipTool", false).asBool();
    bAvoidGroveChopping     = config["ToolSkillsSettings"]["ToolSwinger"].get("bAvoidGroveChopping", false).asBool();

    // BUG CATCHING SETTINGS
    bEnableBugCatching  = config["ToolSkillsSettings"]["ButCatching"].get("bEnableBugCatching", false).asBool();
    BugCatchingDistance = config["ToolSkillsSettings"]["ButCatching"].get("BugCatchingDistance", 10.0f).asFloat();
    bBugAutoEquipTool   = config["ToolSkillsSettings"]["ButCatching"].get("bBugAutoEquipTool", false).asBool();
    bBugUseRandomDelay  = config["ToolSkillsSettings"]["ButCatching"].get("bBugUseRandomDelay", false).asBool();
    BugSpeedPreset      = config["ToolSkillsSettings"]["ButCatching"].get("BugSpeedPreset", 1).asInt();
    BugCatchingDelay    = config["ToolSkillsSettings"]["ButCatching"].get("BugCatchingDelay", 750).asInt();

    // ANIMAL HUNTING SETTINGS
    bEnableAnimalHunting    = config["ToolSkillsSettings"]["AnimalHunting"].get("bEnableAnimalHunting", false).asBool();
    AnimalHuntingDistance   = config["ToolSkillsSettings"]["AnimalHunting"].get("AnimalHuntingDistance", 10.0f).asFloat();
    bAnimalAutoEquipTool    = config["ToolSkillsSettings"]["AnimalHunting"].get("bAnimalAutoEquipTool", false).asBool();
    bAnimalUseRandomDelay   = config["ToolSkillsSettings"]["AnimalHunting"].get("bAnimalUseRandomDelay", false).asBool();
    AnimalSpeedPreset       = config["ToolSkillsSettings"]["AnimalHunting"].get("AnimalSpeedPreset", 1).asInt();
    AnimalHuntingDelay      = config["ToolSkillsSettings"]["AnimalHunting"].get("AnimalHuntingDelay", 750).asInt();

    // OTHER SETTINGS
    bEnableAutoGather           = config["ToolSkillsSettings"]["Others"].get("bEnableAutoGather", false).asBool();
    bEnableCookingMinigameSkip  = config["ToolSkillsSettings"]["Others"].get("bEnableCookingMinigameSkip", false).asBool();

    // FISHING SETTINGS
    bFishingNoDurability            = config["ToolSkillsSettings"]["Fishing"].get("bFishingNoDurability", true).asBool();
    bFishingMultiplayerHelp         = config["ToolSkillsSettings"]["Fishing"].get("bFishingMultiplayerHelp", false).asBool();
    bFishingPerfectCatch            = config["ToolSkillsSettings"]["Fishing"].get("bFishingPerfectCatch", true).asBool();
    bFishingInstantCatch            = config["ToolSkillsSettings"]["Fishing"].get("bFishingInstantCatch", false).asBool();
    bFishingDiscard                 = config["ToolSkillsSettings"]["Fishing"].get("bFishingDiscard", false).asBool();
    bFishingOpenStoreWaterlogged    = config["ToolSkillsSettings"]["Fishing"].get("bFishingOpenStoreWaterlogged", false).asBool();
    bRequireClickFishing            = config["ToolSkillsSettings"]["Fishing"].get("bRequireClickFishing", true).asBool();
}

void Configuration::SaveToolSkillsSettings() {
    Json::Value config;
    
    // AXE/PICKAXE SETTINGS
    config["ToolSkillsSettings"]["ToolSwinger"]["bEnableAutoSwinging"] = bEnableAutoSwinging;
    config["ToolSkillsSettings"]["ToolSwinger"]["bResourceAutoEquipTool"] = bResourceAutoEquipTool;
    config["ToolSkillsSettings"]["ToolSwinger"]["bAvoidGroveChopping"] = bAvoidGroveChopping;

    // BUG CATCHING SETTINGS
    config["ToolSkillsSettings"]["ButCatching"]["bEnableBugCatching"] = bEnableBugCatching;
    config["ToolSkillsSettings"]["ButCatching"]["BugCatchingDistance"] = BugCatchingDistance;
    config["ToolSkillsSettings"]["ButCatching"]["bBugAutoEquipTool"] = bBugAutoEquipTool;
    config["ToolSkillsSettings"]["ButCatching"]["bBugUseRandomDelay"] = bBugUseRandomDelay;
    config["ToolSkillsSettings"]["ButCatching"]["BugSpeedPreset"] = BugSpeedPreset;
    config["ToolSkillsSettings"]["ButCatching"]["BugCatchingDelay"] = BugCatchingDelay;
    
    // ANIMAL HUNTING SETTINGS
    config["ToolSkillsSettings"]["AnimalHunting"]["bEnableAnimalHunting"] = bEnableAnimalHunting;
    config["ToolSkillsSettings"]["AnimalHunting"]["AnimalHuntingDistance"] = AnimalHuntingDistance;
    config["ToolSkillsSettings"]["AnimalHunting"]["bAnimalAutoEquipTool"] = bAnimalAutoEquipTool;
    config["ToolSkillsSettings"]["AnimalHunting"]["bAnimalUseRandomDelay"] = bAnimalUseRandomDelay;
    config["ToolSkillsSettings"]["AnimalHunting"]["AnimalSpeedPreset"] = AnimalSpeedPreset;
    config["ToolSkillsSettings"]["AnimalHunting"]["AnimalHuntingDelay"] = AnimalHuntingDelay;

    // OTHER SETTINGS
    config["ToolSkillsSettings"]["Others"]["bEnableAutoGather"] = bEnableAutoGather;
    config["ToolSkillsSettings"]["Others"]["bEnableCookingMinigameSkip"] = bEnableCookingMinigameSkip;
    
    // FISHING SETTINGS
    config["ToolSkillsSettings"]["Fishing"]["bFishingNoDurability"] = bFishingNoDurability;
    config["ToolSkillsSettings"]["Fishing"]["bFishingMultiplayerHelp"] = bFishingMultiplayerHelp;
    config["ToolSkillsSettings"]["Fishing"]["bFishingInstantCatch"] = bFishingInstantCatch;
    config["ToolSkillsSettings"]["Fishing"]["bFishingPerfectCatch"] = bFishingPerfectCatch;
    config["ToolSkillsSettings"]["Fishing"]["bFishingDiscard"] = bFishingDiscard;
    config["ToolSkillsSettings"]["Fishing"]["bFishingOpenStoreWaterlogged"] = bFishingOpenStoreWaterlogged;
    config["ToolSkillsSettings"]["Fishing"]["bRequireClickFishing"] = bRequireClickFishing;
    
    std::ofstream configFile(toolskillsConfigFilePath);
    if (configFile.is_open()) {
        configFile << config.toStyledString();
        configFile.close();
    }
    else {
        std::cerr << "Failed to open ToolSkills settings file for writing." << '\n';
    }
}

#pragma endregion

// SELL_ITEMS ~ Loading/Saving
#pragma region SELLITEMS_Load_Save

void Configuration::LoadSellItemsSettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open sell items settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;

    // Manual Settings
    bEnableBuyMultiplier    = config["SellItems"]["Manual"].get("bEnableBuyMultiplier", false).asBool();
    bEnableCtrlClickBuy     = config["SellItems"]["Manual"].get("bEnableCtrlClickBuy", false).asBool();
    buyMultiplierValue      = config["SellItems"]["Manual"].get("buyMultiplierValue", 1).asInt();
    
    // AutoSell Settings
    bFishingSell = config["SellItems"]["AutoSell"].get("bFishingSell", false).asBool();
    bFishingSellSQ = config["SellItems"]["AutoSell"].get("bFishingSellSQ", false).asBool();
    for (int i = 0; i < 7; i++) {
        bFishingSellRarity[i] = config["SellItems"]["AutoSell"]["bFishingSellRarity"].get(i, false).asBool();
    }

    bBugSell = config["SellItems"]["AutoSell"].get("bBugSell", false).asBool();
    bBugSellSQ = config["SellItems"]["AutoSell"].get("bBugSellSQ", false).asBool();
    for (int i = 0; i < 7; i++) {
        bBugSellRarity[i] = config["SellItems"]["AutoSell"]["bBugSellRarity"].get(i, false).asBool();
    }

    bHuntingSell = config["SellItems"]["AutoSell"].get("bHuntingSell", false).asBool();
    bHuntingSellSQ = config["SellItems"]["AutoSell"].get("bHuntingSellSQ", false).asBool();
    for (int i = 0; i < 7; i++) {
        bHuntingSellRarity[i] = config["SellItems"]["AutoSell"]["bHuntingSellRarity"].get(i, false).asBool();
    }

    bForageableSell = config["SellItems"]["AutoSell"].get("bForageableSell", false).asBool();
    bForageableSellSQ = config["SellItems"]["AutoSell"].get("bForageableSellSQ", false).asBool();
    for (int i = 0; i < 7; i++) {
        bForageableSellRarity[i] = config["SellItems"]["AutoSell"]["bForageableSellRarity"].get(i, false).asBool();
    }

    bTreeSell = config["SellItems"]["AutoSell"].get("bTreeSell", false).asBool();
    for (int i = 0; i < 4; i++) {
        bTreeSellRarity[i] = config["SellItems"]["AutoSell"]["bTreeSellRarity"].get(i, false).asBool();
    }

    bFlowerSell = config["SellItems"]["AutoSell"].get("bFlowerSell", false).asBool();
}

void Configuration::SaveSellItemsSettings() {
    Json::Value config;

    // Selling Settings
    config["SellItems"]["Manual"]["bEnableBuyMultiplier"] = bEnableBuyMultiplier;
    config["SellItems"]["Manual"]["bEnableCtrlClickBuy"] = bEnableCtrlClickBuy;
    config["SellItems"]["Manual"]["buyMultiplierValue"] = buyMultiplierValue;
    
    // AutoSell Settings
    config["SellItems"]["AutoSell"]["bFishingSell"] = bFishingSell;
    config["SellItems"]["AutoSell"]["bFishingSellSQ"] = bFishingSellSQ;
    for (int i = 0; i < 7; i++) {
        config["SellItems"]["AutoSell"]["bFishingSellRarity"].insert(i, bFishingSellRarity[i]);
    }
    
    config["SellItems"]["AutoSell"]["bBugSell"] = bBugSell;
    config["SellItems"]["AutoSell"]["bBugSellSQ"] = bBugSellSQ;
    for (int i = 0; i < 7; i++) {
        config["SellItems"]["AutoSell"]["bBugSellRarity"].insert(i, bBugSellRarity[i]);
    }
    
    config["SellItems"]["AutoSell"]["bHuntingSell"] = bHuntingSell;
    config["SellItems"]["AutoSell"]["bHuntingSellSQ"] = bHuntingSellSQ;
    for (int i = 0; i < 7; i++) {
        config["SellItems"]["AutoSell"]["bHuntingSellRarity"].insert(i, bHuntingSellRarity[i]);
    }
    
    config["SellItems"]["AutoSell"]["bForageableSell"] = bForageableSell;
    config["SellItems"]["AutoSell"]["bForageableSellSQ"] = bForageableSellSQ;
    for (int i = 0; i < 7; i++) {
        config["SellItems"]["AutoSell"]["bForageableSellRarity"].insert(i, bForageableSellRarity[i]);
    }
    
    config["SellItems"]["AutoSell"]["bTreeSell"] = bTreeSell;
    for (int i = 0; i < 4; i++) {
        config["SellItems"]["AutoSell"]["bTreeSellRarity"].insert(i, bTreeSellRarity[i]);
    }

    config["SellItems"]["AutoSell"]["bFlowerSell"] = bFlowerSell;
    
    std::ofstream configFile(sellitemsConfigFilePath);
    if (configFile.is_open()) {
        configFile << config.toStyledString();
        configFile.close();
    } else {
        std::cerr << "Failed to open sell items settings file for writing." << '\n';
    }
}

#pragma endregion

// MODS ~ Loading/Saving
#pragma region MODS_Load_Save

void Configuration::LoadModSettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open mod settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;

    // CHARACTER SETTINGS
    bEnableAntiAfk                  = config["Mods"]["Character"].get("bEnableAntiAfk", false).asBool();
    bEnableUnlimitedWardrobeRespec  = config["Mods"]["Character"].get("bEnableUnlimitedWardrobeRespec", false).asBool();
    bEnableInteractionMods          = config["Mods"]["Character"].get("bEnableInteractionMods", false).asBool();
    InteractionRadius               = config["Mods"]["Character"].get("InteractionRadius", 500.0f).asFloat();
    bEnableWaypointTeleport         = config["Mods"]["Character"].get("bEnableWaypointTeleport", false).asBool();
    bEasyModeActive                 = config["Mods"]["Character"].get("bEasyModeActive", false).asBool();

    // FUN SETTINGS
    bPlaceAnywhere                  = config["Mods"]["Fun"].get("bPlaceAnywhere", false).asBool();
    PlacementRotation               = config["Mods"]["Fun"].get("PlacementRotation", 15.0).asDouble();
    // Temp Disable outfits
    // bEnableOutfitCustomization      = config["Mods"]["Fun"].get("bEnableOutfitCustomization", false).asBool();
    bEnableContentUnlocker          = config["Mods"]["Fun"].get("bEnableContentUnlocker", false).asBool();
    bEnableFreeLandscape            = config["Mods"]["Fun"].get("bEnableFreeLandscape", false).asBool();
    bEnableShowWeeklyWants          = config["Mods"]["Fun"].get("bEnableShowWeeklyWants", false).asBool();
    bEnableToolBricker              = config["Mods"]["Fun"].get("bEnableToolBricker", false).asBool();
    bEnableRequestMods              = config["Mods"]["Fun"].get("bEnableRequestMods", false).asBool();
    bEnablePrizeWheel               = config["Mods"]["Fun"].get("bEnablePrizeWheel", false).asBool();
    bPrizeWheelUseLocked            = config["Mods"]["Fun"].get("bPrizeWheelUseLocked", false).asBool();
    for (int i = 0; i < 9; ++i) {
        std::string key = "PrizeWheelSlot" + std::to_string(i);
        PrizeWheelSlots[i] = config["Mods"]["Fun"]["PrizeWheelSettings"].get(key, false).asBool();
    }
}

void Configuration::SaveModSettings() {
    Json::Value config;

    // CHARACTER SETTINGS
    config["Mods"]["Character"]["bEnableAntiAfk"] = bEnableAntiAfk;
    config["Mods"]["Character"]["bEnableUnlimitedWardrobeRespec"] = bEnableUnlimitedWardrobeRespec;
    config["Mods"]["Character"]["bEnableInteractionMods"] = bEnableInteractionMods;
    config["Mods"]["Character"]["InteractionRadius"] = InteractionRadius;
    config["Mods"]["Character"]["bEnableWaypointTeleport"] = bEnableWaypointTeleport;
    config["Mods"]["Character"]["bEasyModeActive"] = bEasyModeActive;

    // FUN SETTINGS
    config["Mods"]["Fun"]["bPlaceAnywhere"] = bPlaceAnywhere;
    config["Mods"]["Fun"]["PlacementRotation"] = PlacementRotation;
    config["Mods"]["Fun"]["bEnableOutfitCustomization"] = bEnableOutfitCustomization;
    config["Mods"]["Fun"]["bEnableContentUnlocker"] = bEnableContentUnlocker;
    config["Mods"]["Fun"]["bEnableFreeLandscape"] = bEnableFreeLandscape;
    config["Mods"]["Fun"]["bEnableShowWeeklyWants"] = bEnableShowWeeklyWants;
    config["Mods"]["Fun"]["bEnableRequestMods"] = bEnableRequestMods;
    config["Mods"]["Fun"]["bEnableToolBricker"] = bEnableToolBricker;
    config["Mods"]["Fun"]["bEnablePrizeWheel"] = bEnablePrizeWheel;
    config["Mods"]["Fun"]["bPrizeWheelUseLocked"] = bPrizeWheelUseLocked;
    for (int i = 0; i < 9; ++i) {
        std::string key = "PrizeWheelSlot" + std::to_string(i);
        config["Mods"]["Fun"]["PrizeWheelSettings"][key] = PrizeWheelSlots[i];
    }
    
    std::ofstream configFile(modConfigFilePath);
    if (configFile.is_open()) {
        configFile << config.toStyledString();
        configFile.close();
    }
    else {
        std::cerr << "Failed to open Mod settings file for writing." << '\n';
    }
}

#pragma endregion

// THEME ~ Loading/Saving
#pragma region THEME_Load_Save

void Configuration::LoadCustomThemeSettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open custom theme settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;

    if (!config.isMember("Custom Theme Settings")) {
        std::cerr << "Missing 'Custom Theme Settings' in config." << '\n';
        return;
    }

    for (const auto& colorIndex : config["Custom Theme Settings"].getMemberNames()) {
        int index = std::stoi(colorIndex);
        Json::Value colorJson = config["Custom Theme Settings"][colorIndex];

        float r = colorJson.get("R", 0.0).asFloat();
        float g = colorJson.get("G", 0.0).asFloat();
        float b = colorJson.get("B", 0.0).asFloat();
        float a = colorJson.get("A", 1.0).asFloat();

        auto color = ImVec4(r, g, b, a);
        customColors[index] = color;
    }
}

void Configuration::SaveCustomThemeSettings() {
    Json::Value config;

    for (const auto& pair : customColors) {
        int colorIndex = pair.first;
        ImVec4 colorValue = pair.second;

        Json::Value colorJson;
        colorJson["R"] = colorValue.x;
        colorJson["G"] = colorValue.y;
        colorJson["B"] = colorValue.z;
        colorJson["A"] = colorValue.w;

        config["Custom Theme Settings"][std::to_string(colorIndex)] = colorJson;
    }

    std::ofstream configFile(customThemeConfigFilePath);
    if (configFile.is_open()) {
        configFile << config.toStyledString();
        configFile.close();
    }
    else {
        std::cerr << "Failed to open Mod settings file for writing." << '\n';
    }
}

#pragma endregion

// HOTKEYS ~ Loading/Saving
#pragma region HOTKEYS_Load_Save

void Configuration::LoadHotkeySettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open hotkey settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;
    
    for (auto it = config.begin(); it != config.end(); ++it) {
        std::string actionName = it.name();
        int keyCode = it->asInt();
        HotkeysManager::SetHotkey(actionName, keyCode);
    }
}

void Configuration::SaveHotkeySettings() {
    Json::Value config;
    
    for (const auto& [actionName, keyCallbackPair] : HotkeysManager::GetHotkeys()) {
        config[actionName] = keyCallbackPair.first;
    }
    
    std::ofstream configFile(hotkeysConfigFilePath);
    if (configFile.is_open()) {
        configFile << config.toStyledString();
        configFile.close();
    }
    else {
        std::cerr << "Failed to open Hotkey settings file for writing." << '\n';
    }
}

#pragma endregion

void Configuration::Load() {
    if (ConfigLoaded) return;

    std::ifstream overlayConfigStream(overlayConfigFilePath, std::ifstream::binary);
    LoadOverlaySettings(overlayConfigStream);
    overlayConfigStream.close();
    
    std::ifstream espConfigStream(espConfigFilePath, std::ifstream::binary);
    LoadESPSettings(espConfigStream);
    espConfigStream.close();
    
    std::ifstream aimConfigStream(aimConfigFilePath, std::ifstream::binary);
    LoadAimSettings(aimConfigStream);
    aimConfigStream.close();

    std::ifstream movementConfigStream(movementConfigFilePath, std::ifstream::binary);
    LoadMovementSettings(movementConfigStream);
    movementConfigStream.close();

    std::ifstream toolskillsConfigStream(toolskillsConfigFilePath, std::ifstream::binary);
    LoadToolSkillsSettings(toolskillsConfigStream);
    toolskillsConfigStream.close();

    std::ifstream sellitemsConfigStream(sellitemsConfigFilePath, std::ifstream::binary);
    LoadSellItemsSettings(sellitemsConfigStream);
    sellitemsConfigStream.close();
    
    std::ifstream modConfigStream(modConfigFilePath, std::ifstream::binary);
    LoadModSettings(modConfigStream);
    modConfigStream.close();
    
    std::ifstream customThemeStream(customThemeConfigFilePath, std::ifstream::binary);
    LoadCustomThemeSettings(customThemeStream);
    customThemeStream.close();

    std::ifstream hotkeyConfigStream(hotkeysConfigFilePath, std::ifstream::binary);
    LoadHotkeySettings(hotkeyConfigStream);
    hotkeyConfigStream.close();
    
    // Set ConfigLoaded to true after loading all the configurations
    ConfigLoaded = true;
}

void Configuration::Save(ESaveFile save_file) {
    if (std::filesystem::path dir(configDirectory); !exists(dir)) {
        create_directories(dir);
    }

    switch(save_file) {
    case NONE:
        // do nothing.
        break;
    case OverlaySettings:
        SaveOverlaySettings();
        break;
    case ESPSettings:
        SaveESPSettings();
        break;
    case AimSettings:
        SaveAimSettings();
        break;
    case MovementSettings:
        SaveMovementSettings();
        break;
    case ToolSkillsSettings:
        SaveToolSkillsSettings();
        break;
    case SellItemsSettings:
        SaveSellItemsSettings();
        break;
    case ModSettings:
        SaveModSettings();
        break;
    case CustomThemeSettings:
        SaveCustomThemeSettings();
        break;
    case HotkeySettings:
        SaveHotkeySettings();
        break;
    case SAVE_ALL:
        SaveOverlaySettings();
        SaveESPSettings();
        SaveAimSettings();
        SaveMovementSettings();
        SaveToolSkillsSettings();
        SaveSellItemsSettings();
        SaveModSettings();
        SaveCustomThemeSettings();
        SaveHotkeySettings();
        break;
    }
}

void Configuration::ApplyCustomTheme() {
    ImGuiStyle* style = &ImGui::GetStyle();
    style->GrabRounding = 4.0f;

    for (const auto& it : GuiColors) {
        int idx = it.first;
        auto colorIt = customColors.find(idx);
        if (colorIt != customColors.end()) {
            style->Colors[idx] = colorIt->second;
        }
    }
}