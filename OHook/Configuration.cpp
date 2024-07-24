#include "Configuration.h"
#include "PaliaOverlay.h"

#include <imgui.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <json/json.h>

#include "Misc/ImGuiExt.h"
#include "Detours/HUDDetours.h"

namespace fs = std::filesystem;

#pragma region VarsInit

bool Configuration::ConfigLoaded = false;
bool Configuration::HasMigrated = false;

// Config Versions
const float Configuration::Version = 2.0;
const float Configuration::AimVersion = 1.0;
const float Configuration::ESPVersion = 1.1;
const float Configuration::ModVersion = 1.0;
const float Configuration::MovementVersion = 1.0;
const float Configuration::OverlayVersion = 1.0;
const float Configuration::CustomThemeVersion = 1.0;

// Crafting Cooking Booleans
// bool bEnableInstantCraftingCooking = false;

// Window configs
float Configuration::windowSizeX = 1450.0f;
float Configuration::windowSizeY = 950.0f;
int Configuration::selectedTheme = 0;
std::map<int, ImVec4> Configuration::customColors = {};

float Configuration::activeWindowPosX = 125.0f;
float Configuration::activeWindowPosY = 5.0f;
float Configuration::activeModsOpacity = 0.70f;
float Configuration::activeModsRounding = 5.0f;

bool Configuration::showEnabledFeaturesWindow = false;
bool Configuration::activeModsStickToRight = false;

//Aimbots
bool Configuration::bEnableAimbot = false;
bool Configuration::bEnableSilentAimbot = false;

// Tools
#ifdef ENABLE_SUPPORTER_FEATURES
bool Configuration::bEnableAutoToolUse = false;
#endif

// Game Modifiers
float Configuration::CustomWalkSpeed = 565.0f; // Custom, Dynamic walk speed
float Configuration::CustomSprintSpeedMultiplier = 1.65f; // Custom, Dynamic sprint speed multiplier
float Configuration::CustomClimbingSpeed = 80.0f; // Custom, Dynamic climbing speed
float Configuration::CustomGlidingSpeed = 900.0f; // Custom, Dynamic gliding speed
float Configuration::CustomGlidingFallSpeed = 250.0f; // Custom, Dynamic gliding fall speed
float Configuration::CustomJumpVelocity = 700.0f; // Custom, Dynamic jump velocity
float Configuration::CustomMaxStepHeight = 45.0f; // Custom, Dynamic maximum step height

//bool Configuration::bAutoGatherOnTeleport = false;
bool Configuration::bEnableAutoGather = false;

// Fishing Options
bool Configuration::bFishingNoDurability = true;
bool Configuration::bFishingMultiplayerHelp = false;
bool Configuration::bFishingInstantCatch = false;
bool Configuration::bFishingPerfectCatch = true;
bool Configuration::bFishingSell = false;
bool Configuration::bFishingDiscard = false;
bool Configuration::bFishingOpenStoreWaterlogged = false;
bool Configuration::bRequireClickFishing = true;

// Item Booleans
bool Configuration::bEasyModeActive = false;
bool Configuration::bEnableLootbagTeleportation = false;
bool Configuration::bEnableWaypointTeleport = false;

// Housing
bool Configuration::bPlaceAnywhere = false;
#ifdef ENABLE_SUPPORTER_FEATURES
bool Configuration::bEnableFreeLandscape = false;
#endif

//bool Configuration::bManualPositionAdjustment = false;

// Gardening
//bool Configuration::bInfiniteWateringCan = false;

// Quicksell Hotkeys
//bool Configuration::bEnableQuicksellHotkeys = false;

// Fun Mods
bool Configuration::bEnableAntiAfk = false;
bool Configuration::bEnableUnlimitedWardrobeRespec = false;
bool Configuration::bEnableMinigameSkip = false;
bool Configuration::bEnableInteractionMods = false;
bool Configuration::bEnablePrizeWheel = false;
bool Configuration::bPrizeWheelUseLocked = false;
bool Configuration::PrizeWheelSlots[9] = { false, false, false, false, false, false, false, false, false };
bool* Configuration::bEnableWheelSlot0 = nullptr;
bool* Configuration::bEnableWheelSlot1 = nullptr;
bool* Configuration::bEnableWheelSlot2 = nullptr;
bool* Configuration::bEnableWheelSlot3 = nullptr;
bool* Configuration::bEnableWheelSlot4 = nullptr;
bool* Configuration::bEnableWheelSlot5 = nullptr;
bool* Configuration::bEnableWheelSlot6 = nullptr;
bool* Configuration::bEnableWheelSlot7 = nullptr;
bool* Configuration::bEnableWheelSlot8 = nullptr;


// Customization Mods
#ifdef ENABLE_SUPPORTER_FEATURES
bool Configuration::bEnableOutfitCustomization = false;
#endif
bool Configuration::bEnableRequestMods = false;

float Configuration::FOVRadius = 185.0f;
float Configuration::InteractionRadius = 500.0f;
int Configuration::AvoidanceRadius = 30;
float Configuration::CircleAlpha = 1.0f;

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
    {EOneOffs::Loot, IM_COL32(0xEE, 0x82, 0xEE, 0xFF)}, // Violet
    {EOneOffs::Quest, IM_COL32(0xFF, 0xA5, 0x00, 0xFF)}, // Orange
    {EOneOffs::RummagePiles, IM_COL32(0xFF, 0x45, 0x00, 0xFF)}, // Orange Red
    {EOneOffs::Treasure, IM_COL32(0xFF, 0xD7, 0x00, 0xFF)},  // Bright Gold
    {EOneOffs::TimedDrop, IM_COL32(0xDB, 0xAC, 0x34, 0xFF)}, // Gold
    {EOneOffs::Others, IM_COL32(0xC0, 0xC0, 0xC0, 0xFF)}, // Washed Gray
    {EOneOffs::Stables, IM_COL32(0x88, 0x45, 0x13, 0xFF)} // Saddle Brown
};

std::map<ETreeType, ImU32> Configuration::TreeColors = {
    {ETreeType::Unknown, IM_COL32(0xFF, 0xFF, 0xFF, 0xFF)},
    {ETreeType::Flow, IM_COL32(0x67, 0x00, 0xEA, 0xFF)},
    {ETreeType::Heartwood, IM_COL32(0x00, 0xFF, 0x00, 0xFF)},
    {ETreeType::Sapwood, IM_COL32(0x00, 0xFF, 0x00, 0xFF)},
    {ETreeType::Bush, IM_COL32(0xFF, 0xFF, 0xFF, 0xFF)},
};

std::map<EFishType, ImU32> Configuration::FishColors = {
    {EFishType::Unknown, IM_COL32(0xFF, 0xFF, 0xFF, 0xFF)},
    {EFishType::Node, IM_COL32(0xFF, 0xFF, 0xFF, 0xFF)},
    {EFishType::Hook, IM_COL32(0xFF, 0xFF, 0xFF, 0xFF)}
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
    {{EBugKind::Glowbug, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
    //Ladybug
    {{EBugKind::Ladybug, EBugQuality::Common}, IM_COL32(0x80, 0x80, 0x80, 0xFF)}, // Gray
    {{EBugKind::Ladybug, EBugQuality::Uncommon}, IM_COL32(0x32, 0xCD, 0x32, 0xFF)}, // Lime Green
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

// ESP Booleans

bool Configuration::bShowWatermark = true;

bool Configuration::bEnableESP = true;
bool Configuration::bESPIcons = false;
bool Configuration::bESPIconDistance = false;
bool Configuration::bDrawFOVCircle = true;
bool Configuration::bHideFOVCircle = false;
bool Configuration::bDrawCrosshair = true;
bool Configuration::bTeleportToTargeted = true;
bool Configuration::bAvoidTeleportingToPlayers = true;
bool Configuration::bDoRadiusPlayersAvoidance = true;
bool Configuration::bEnableESPCulling = true;
// bool Configuration::bEnableDespawnTimer = true; // TODO: Still crashing

// ESP Numericals

//float Configuration::ESPTextScale = 1.0f; // DISABLED: ImGui Font-resizing not setup
int Configuration::CullDistance = 200;

// ESP Animals

bool* Configuration::bEnableSernuk = nullptr;
bool* Configuration::bEnableElderSernuk = nullptr;
bool* Configuration::bEnableProudhornSernuk = nullptr;
bool* Configuration::bEnableChapaa = nullptr;
bool* Configuration::bEnableStripedChapaa = nullptr;
bool* Configuration::bEnableAzureChapaa = nullptr;
bool* Configuration::bEnableMinigameChapaa = nullptr;
bool* Configuration::bEnableMuujin = nullptr;
bool* Configuration::bEnableBandedMuujin = nullptr;
bool* Configuration::bEnableBluebristleMuujin = nullptr;

// ESP Ores

bool* Configuration::bEnableClayLg = nullptr;

bool* Configuration::bEnableStoneSm = nullptr;
bool* Configuration::bEnableStoneMed = nullptr;
bool* Configuration::bEnableStoneLg = nullptr;

bool* Configuration::bEnableCopperSm = nullptr;
bool* Configuration::bEnableCopperMed = nullptr;
bool* Configuration::bEnableCopperLg = nullptr;

bool* Configuration::bEnableIronSm = nullptr;
bool* Configuration::bEnableIronMed = nullptr;
bool* Configuration::bEnableIronLg = nullptr;

bool* Configuration::bEnablePaliumSm = nullptr;
bool* Configuration::bEnablePaliumMed = nullptr;
bool* Configuration::bEnablePaliumLg = nullptr;

// ESP Forage Types

bool* Configuration::bEnableCoral = nullptr;
bool* Configuration::bEnableOyster = nullptr;
bool* Configuration::bEnableShell = nullptr;

bool* Configuration::bEnablePoisonFlower = nullptr;
bool* Configuration::bEnablePoisonFlowerP = nullptr;

bool* Configuration::bEnableWaterFlower = nullptr;
bool* Configuration::bEnableWaterFlowerP = nullptr;

bool* Configuration::bEnableHeartdrop = nullptr;
bool* Configuration::bEnableHeartdropP = nullptr;

bool* Configuration::bEnableSundrop = nullptr;
bool* Configuration::bEnableSundropP = nullptr;

bool* Configuration::bEnableDragonsBeard = nullptr;
bool* Configuration::bEnableDragonsBeardP = nullptr;

bool* Configuration::bEnableEmeraldCarpet = nullptr;
bool* Configuration::bEnableEmeraldCarpetP = nullptr;

bool* Configuration::bEnableMushroomBlue = nullptr;
bool* Configuration::bEnableMushroomBlueP = nullptr;

bool* Configuration::bEnableMushroomRed = nullptr;
bool* Configuration::bEnableMushroomRedP = nullptr;

bool* Configuration::bEnableDariCloves = nullptr;
bool* Configuration::bEnableDariClovesP = nullptr;

bool* Configuration::bEnableHeatRoot = nullptr;
bool* Configuration::bEnableHeatRootP = nullptr;

bool* Configuration::bEnableSpicedSprouts = nullptr;
bool* Configuration::bEnableSpicedSproutsP = nullptr;

bool* Configuration::bEnableSweetLeaves = nullptr;
bool* Configuration::bEnableSweetLeavesP = nullptr;

bool* Configuration::bEnableGarlic = nullptr;
bool* Configuration::bEnableGarlicP = nullptr;

bool* Configuration::bEnableGinger = nullptr;
bool* Configuration::bEnableGingerP = nullptr;

bool* Configuration::bEnableGreenOnion = nullptr;
bool* Configuration::bEnableGreenOnionP = nullptr;

// ESP Bug Types

bool* Configuration::bEnableBeeU = nullptr;
bool* Configuration::bEnableBeeUP = nullptr;

bool* Configuration::bEnableBeeR = nullptr;
bool* Configuration::bEnableBeeRP = nullptr;

bool* Configuration::bEnableBeetleC = nullptr;
bool* Configuration::bEnableBeetleCP = nullptr;

bool* Configuration::bEnableBeetleU = nullptr;
bool* Configuration::bEnableBeetleUP = nullptr;

bool* Configuration::bEnableBeetleR = nullptr;
bool* Configuration::bEnableBeetleRP = nullptr;

bool* Configuration::bEnableBeetleE = nullptr;
bool* Configuration::bEnableBeetleEP = nullptr;

bool* Configuration::bEnableButterflyC = nullptr;
bool* Configuration::bEnableButterflyCP = nullptr;

bool* Configuration::bEnableButterflyU = nullptr;
bool* Configuration::bEnableButterflyUP = nullptr;

bool* Configuration::bEnableButterflyR = nullptr;
bool* Configuration::bEnableButterflyRP = nullptr;

bool* Configuration::bEnableButterflyE = nullptr;
bool* Configuration::bEnableButterflyEP = nullptr;

bool* Configuration::bEnableCicadaC = nullptr;
bool* Configuration::bEnableCicadaCP = nullptr;

bool* Configuration::bEnableCicadaU = nullptr;
bool* Configuration::bEnableCicadaUP = nullptr;

bool* Configuration::bEnableCicadaR = nullptr;
bool* Configuration::bEnableCicadaRP = nullptr;

bool* Configuration::bEnableCrabC = nullptr;
bool* Configuration::bEnableCrabCP = nullptr;

bool* Configuration::bEnableCrabU = nullptr;
bool* Configuration::bEnableCrabUP = nullptr;

bool* Configuration::bEnableCrabR = nullptr;
bool* Configuration::bEnableCrabRP = nullptr;

bool* Configuration::bEnableCricketC = nullptr;
bool* Configuration::bEnableCricketCP = nullptr;

bool* Configuration::bEnableCricketU = nullptr;
bool* Configuration::bEnableCricketUP = nullptr;

bool* Configuration::bEnableCricketR = nullptr;
bool* Configuration::bEnableCricketRP = nullptr;

bool* Configuration::bEnableDragonflyC = nullptr;
bool* Configuration::bEnableDragonflyCP = nullptr;

bool* Configuration::bEnableDragonflyU = nullptr;
bool* Configuration::bEnableDragonflyUP = nullptr;

bool* Configuration::bEnableDragonflyR = nullptr;
bool* Configuration::bEnableDragonflyRP = nullptr;

bool* Configuration::bEnableDragonflyE = nullptr;
bool* Configuration::bEnableDragonflyEP = nullptr;

bool* Configuration::bEnableGlowbugC = nullptr;
bool* Configuration::bEnableGlowbugCP = nullptr;

bool* Configuration::bEnableGlowbugU = nullptr;
bool* Configuration::bEnableGlowbugUP = nullptr;

bool* Configuration::bEnableLadybugC = nullptr;
bool* Configuration::bEnableLadybugCP = nullptr;

bool* Configuration::bEnableLadybugU = nullptr;
bool* Configuration::bEnableLadybugUP = nullptr;

bool* Configuration::bEnableMantisU = nullptr;
bool* Configuration::bEnableMantisUP = nullptr;

bool* Configuration::bEnableMantisR = nullptr;
bool* Configuration::bEnableMantisRP = nullptr;

bool* Configuration::bEnableMantisR2 = nullptr;
bool* Configuration::bEnableMantisR2P = nullptr;

bool* Configuration::bEnableMantisE = nullptr;
bool* Configuration::bEnableMantisEP = nullptr;

bool* Configuration::bEnableMothC = nullptr;
bool* Configuration::bEnableMothCP = nullptr;

bool* Configuration::bEnableMothU = nullptr;
bool* Configuration::bEnableMothUP = nullptr;

bool* Configuration::bEnableMothR = nullptr;
bool* Configuration::bEnableMothRP = nullptr;

bool* Configuration::bEnablePedeU = nullptr;
bool* Configuration::bEnablePedeUP = nullptr;

bool* Configuration::bEnablePedeR = nullptr;
bool* Configuration::bEnablePedeRP = nullptr;

bool* Configuration::bEnablePedeR2 = nullptr;
bool* Configuration::bEnablePedeR2P = nullptr;

bool* Configuration::bEnableSnailU = nullptr;
bool* Configuration::bEnableSnailUP = nullptr;

bool* Configuration::bEnableSnailR = nullptr;
bool* Configuration::bEnableSnailRP = nullptr;

// ESP Trees

bool* Configuration::bEnableBushSm = nullptr;

bool* Configuration::bEnableSapwoodSm = nullptr;
bool* Configuration::bEnableSapwoodMed = nullptr;
bool* Configuration::bEnableSapwoodLg = nullptr;

bool* Configuration::bEnableHeartwoodSm = nullptr;
bool* Configuration::bEnableHeartwoodMed = nullptr;
bool* Configuration::bEnableHeartwoodLg = nullptr;

bool* Configuration::bEnableFlowSm = nullptr;
bool* Configuration::bEnableFlowMed = nullptr;
bool* Configuration::bEnableFlowLg = nullptr;

// ESP Player & Entities

bool* Configuration::bEnablePlayers = nullptr;
bool* Configuration::bEnableNPC = nullptr;
bool* Configuration::bEnableFish = nullptr;
bool* Configuration::bEnablePools = nullptr;
bool* Configuration::bEnableLoot = nullptr;
bool* Configuration::bEnableQuest = nullptr;
bool* Configuration::bEnableRummagePiles = nullptr;
bool* Configuration::bEnableStables = nullptr;
bool* Configuration::bEnableTreasure = nullptr;
bool* Configuration::bEnableTimedDrop = nullptr;
bool Configuration::bEnableOthers = false;

#pragma endregion
#pragma endregion

// Path for the configuration file
static const std::string configDirectory = R"(C:\ProgramData\OriginPalia\config)";
static const std::string oldConfigFilePath = configDirectory + "\\overlay_config.json";
static const std::string overlayConfigFilePath      = configDirectory + "\\Overlay_settings.json";
static const std::string espConfigFilePath          = configDirectory + "\\ESP_settings.json";
static const std::string aimConfigFilePath          = configDirectory + "\\Aim_settings.json";
static const std::string modConfigFilePath          = configDirectory + "\\Mod_settings.json";
static const std::string movementConfigFilePath     = configDirectory + "\\Movement_settings.json";
static const std::string customThemeConfigFilePath  = configDirectory + "\\Custom_theme.json";

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

// Functions for saving and loading config data
static void EnsureDirectoryExists(const std::string& path) {
    std::filesystem::path dir(path);
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
}

void Configuration::CheckAndMigrateOldConfig() {
    std::ifstream oldConfig(oldConfigFilePath);
    if (oldConfig.is_open()) {
        Json::CharReaderBuilder readerBuilder;
        Json::Value config;
        std::string errs;

        if (Json::parseFromStream(readerBuilder, oldConfig, &config, &errs)) {
            double oldVersion = config.get("Version", 0).asDouble();
            if (oldVersion < Version) {
                std::cout << "Migrating from old config." << '\n';
                MigrateOldConfig(config);
                if (HasMigrated) {
                    oldConfig.close();
                    std::remove(oldConfigFilePath.c_str());

                    if (std::filesystem::exists(oldConfigFilePath)) {
                        std::filesystem::remove(oldConfigFilePath);
                    }
                }
            }
            oldConfig.close();
        }
        else {
            std::cerr << "Failed to parse old configuration file: " << errs << '\n';
            oldConfig.close();
        }
    }
    else {
        std::cerr << "Old config file not found." << '\n';
        return;
    }
}

void Configuration::MigrateOldConfig(const Json::Value& oldConfig) {
    //Migrate Aim settings
    Json::Value aimConfig;
    aimConfig["Version"] = AimVersion;

    aimConfig["AimSettings"]["Toggles"]["bEnableAimbot"] = oldConfig.get("Enable Legacy Aimbot", false).asBool();
    aimConfig["AimSettings"]["Toggles"]["bEnableSilentAimbot"] = oldConfig.get("Enable Silent Aimbot", false).asBool();

    aimConfig["AimSettings"]["FOV"]["Radius"]["FOVRadius"] = oldConfig.get("InteliAim Radius", 185.0f).asFloat();
    aimConfig["AimSettings"]["FOV"]["Transparency"]["CircleAlpha"] = oldConfig.get("InteliAim Transparency", 185.0f).asFloat();
    aimConfig["AimSettings"]["FOV"]["Toggles"]["bDrawFOVCircle"] = oldConfig.get("Enable InteliAim Circle", true).asBool();
    aimConfig["AimSettings"]["FOV"]["Toggles"]["bHideFOVCircle"] = oldConfig.get("Hide Circle", false).asBool();

    aimConfig["AimSettings"]["Settings"]["AvoidanceRadius"] = oldConfig.get("Avoidance Radius", 30).asInt();
    aimConfig["AimSettings"]["Settings"]["bDrawCrosshair"] = oldConfig.get("Enable Crosshair", true).asBool();

    std::ofstream aimConfigFile(aimConfigFilePath);
    aimConfigFile << aimConfig.toStyledString();
    aimConfigFile.close();

    // Migrate Overlay settings
    Json::Value overlayConfig;
    overlayConfig["Version"] = OverlayVersion;

    overlayConfig["OverlaySettings"]["MainWindow"]["windowSizeX"] = oldConfig.get("Menu Size X", 1450.0f).asFloat();
    overlayConfig["OverlaySettings"]["MainWindow"]["windowSizeY"] = oldConfig.get("Menu Size Y", 950.0f).asFloat();
    overlayConfig["OverlaySettings"]["MainWindow"]["selectedTheme"] = oldConfig.get("Selected Theme Id", 0).asInt();

    overlayConfig["OverlaySettings"]["ModMenu"]["Position"]["activeWindowPosX"] = oldConfig.get("Active Mods Pos X", 125.0f).asFloat();
    overlayConfig["OverlaySettings"]["ModMenu"]["Position"]["activeWindowPosY"] = oldConfig.get("Active Mods Pos Y", 5.0f).asFloat();
    overlayConfig["OverlaySettings"]["ModMenu"]["Styling"]["activeModsOpacity"] = oldConfig.get("Active Mods Opacity", 0.70f).asFloat();
    overlayConfig["OverlaySettings"]["ModMenu"]["Styling"]["activeModsRounding"] = oldConfig.get("Active Mods Rounding", 5.0f).asFloat();
    overlayConfig["OverlaySettings"]["ModMenu"]["Toggled"]["showEnabledFeaturesWindow"] = oldConfig.get("Show Active Mods Window", false).asBool();
    overlayConfig["OverlaySettings"]["ModMenu"]["Settings"]["activeModsStickToRight"] = oldConfig.get("Active Mods Sticky Right", false).asBool();

    overlayConfig["OverlaySettings"]["Settings"]["bShowWatermark"] = oldConfig.get("Show Watermark", true).asBool();

    std::ofstream overlayConfigFile(overlayConfigFilePath);
    overlayConfigFile << overlayConfig.toStyledString();
    overlayConfigFile.close();

    // Migrate Mod settings
    Json::Value modConfig;
    modConfig["Version"] = ModVersion;

    modConfig["Mods"]["Fishing"]["bFishingNoDurability"] = oldConfig.get("No Fishing Rod Durability", true).asBool();
    modConfig["Mods"]["Fishing"]["bFishingMultiplayerHelp"] = oldConfig.get("Enable Fishing Multiplayer Help", false).asBool();
    modConfig["Mods"]["Fishing"]["bFishingInstantCatch"] = oldConfig.get("Enable Fishing Instant Catch", false).asBool();
    modConfig["Mods"]["Fishing"]["bFishingPerfectCatch"] = oldConfig.get("Enable Fishing Perfect Catch", true).asBool();
    modConfig["Mods"]["Fishing"]["bFishingSell"] = oldConfig.get("Enable Sell All Fish", false).asBool();
    modConfig["Mods"]["Fishing"]["bFishingDiscard"] = oldConfig.get("Enable Discarding Fishing Junk", false).asBool();
    modConfig["Mods"]["Fishing"]["bFishingOpenStoreWaterlogged"] = oldConfig.get("Fishing Open Store Waterlogged", false).asBool();
    modConfig["Mods"]["Fishing"]["bRequireClickFishing"] = oldConfig.get("Require Left Click Fishing", true).asBool();

    //modConfig["Mods"]["bEasyModeActive"] = oldConfig.get("bEasyModeActive", false).asBool(); // not used in old config?

    modConfig["Mods"]["Interaction Mods"]["bEnableInteractionMods"] = oldConfig.get("Enable Interaction Mods", false).asBool();
    modConfig["Mods"]["Interaction Mods"]["InteractionRadius"] = oldConfig.get("Interaction Max Radius", 500.0f).asFloat();

    modConfig["Mods"]["PrizeWheel"]["Toggled"]["bEnablePrizeWheel"] = oldConfig.get("Enable Prize Wheel", false).asBool();
    modConfig["Mods"]["PrizeWheel"]["Settings"]["bPrizeWheelUseLocked"] = oldConfig.get("Prize Wheel Use Lockbox", false).asBool();
    for (int i = 0; i < 9; ++i) {
        std::string oldkey = "Enable Wheel Slot " + std::to_string(i);
        std::string newkey = "PrizeWheelSlot" + std::to_string(i);
        modConfig["Mods"]["PrizeWheel"]["Settings"][newkey] = oldConfig.get(oldkey, false).asBool();
    }

    modConfig["Mods"]["Unlockables"]["bEnableFreeLandscape"] = oldConfig.get("Unlock Landscape", false).asBool();
    modConfig["Mods"]["Unlockables"]["bEnableOutfitCustomization"] = oldConfig.get("Enable Unlock All Outfits", false).asBool();

    modConfig["Mods"]["Misc"]["bEnableAutoGather"] = oldConfig.get("Enable AutoGather", false).asBool();
    modConfig["Mods"]["Misc"]["bEnableRequestMods"] = oldConfig.get("Enable Request Mods", false).asBool();
    modConfig["Mods"]["Misc"]["bPlaceAnywhere"] = oldConfig.get("Place Items Anywhere", false).asBool();
    modConfig["Mods"]["Misc"]["bEnableAntiAfk"] = oldConfig.get("Enable AntiAFK", false).asBool();
    modConfig["Mods"]["Misc"]["bEnableMinigameSkip"] = oldConfig.get("Enable Minigame Skip", false).asBool();

    std::ofstream modConfigFile(modConfigFilePath);
    modConfigFile << modConfig.toStyledString();
    modConfigFile.close();

    // Migrate movement settings
    Json::Value moveConfig;
    moveConfig["Version"] = MovementVersion;

    moveConfig["MovementSettings"]["CustomWalkSpeed"] = oldConfig.get("Custom Walk Speed", 565.0f).asFloat();
    moveConfig["MovementSettings"]["CustomSprintSpeedMultiplier"] = oldConfig.get("Custom Sprint Speed Multiplier", 1.65f).asFloat();
    moveConfig["MovementSettings"]["CustomClimbingSpeed"] = oldConfig.get("Custom Climbing Speed", 80.0f).asFloat();
    moveConfig["MovementSettings"]["CustomGlidingSpeed"] = oldConfig.get("Custom Gliding Speed", 900.0f).asFloat();
    moveConfig["MovementSettings"]["CustomGlidingFallSpeed"] = oldConfig.get("Custom Gliding Fall Speed", 250.0f).asFloat();
    moveConfig["MovementSettings"]["CustomJumpVelocity"] = oldConfig.get("Custom Jump Velocity", 700.0f).asFloat();
    moveConfig["MovementSettings"]["CustomMaxStepHeight"] = oldConfig.get("Custom Max Step Height", 45.0f).asFloat();

    moveConfig["TeleportSettings"]["bEnableWaypointTeleport"] = oldConfig.get("Teleport to Waypoint", false).asBool();
    moveConfig["TeleportSettings"]["bTeleportToTargeted"] = oldConfig.get("Teleport to Targeted", true).asBool();
    moveConfig["TeleportSettings"]["bAvoidTeleportingToPlayers"] = oldConfig.get("Avoid Teleporting To Targeted Players", true).asBool();
    moveConfig["TeleportSettings"]["bDoRadiusPlayersAvoidance"] = oldConfig.get("Avoidance Radius", true).asBool();

    std::ofstream moveConfigFile(movementConfigFilePath);
    moveConfigFile << moveConfig.toStyledString();
    moveConfigFile.close();

    // Migrate ESP settings
    Json::Value espConfig;
    espConfig["Version"] = ESPVersion;

    espConfig["ESPSettings"]["Toggle"]["bEnableESP"] = oldConfig.get("Enable ESP", true).asBool();
    espConfig["ESPSettings"]["Icons"]["bESPIcons"] = oldConfig.get("ESP Icons", false).asBool();
    espConfig["ESPSettings"]["Icons"]["bESPIconDistance"] = oldConfig.get("ESP Icon Distance", false).asBool();
    //espConfig["ESPSettings"]["Culling"]["bEnableESPCulling"] = oldConfig.get("bEnableESPCulling", true).asBool(); // unused
    espConfig["ESPSettings"]["Culling"]["CullDistance"] = oldConfig.get("Distance", 200).asInt();
    espConfig["ESPSettings"]["Others"]["ESPTextScale"] = oldConfig.get("ESP Text Scale", 1.0f).asFloat();

    // Animals
    espConfig["ESPSettings"]["Categories"]["Animals"]["Sernuk"]["bEnableSernuk"] = oldConfig.get("Enable Sernuk", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Animals"]["Sernuk"]["bEnableElderSernuk"] = oldConfig.get("Enable Elder Sernuk", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Animals"]["Sernuk"]["bEnableProudhornSernuk"] = oldConfig.get("Enable Proudhorn Sernuk", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Animals"]["Chapaa"]["bEnableChapaa"] = oldConfig.get("Enable Chapaa", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Animals"]["Chapaa"]["bEnableStripedChapaa"] = oldConfig.get("Enable Striped Chapaa", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Animals"]["Chapaa"]["bEnableAzureChapaa"] = oldConfig.get("Enable Azure Chapaa", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Animals"]["Chapaa"]["bEnableMinigameChapaa"] = oldConfig.get("Enable Minigame Chapaa", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Animals"]["Muujin"]["bEnableMuujin"] = oldConfig.get("Enable Muujin", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Animals"]["Muujin"]["bEnableBandedMuujin"] = oldConfig.get("Enable Banded Muujin", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Animals"]["Muujin"]["bEnableBluebristleMuujin"] = oldConfig.get("Enable Bluebristle Muujin", false).asBool();

    // Ores
    espConfig["ESPSettings"]["Categories"]["Ores"]["Clay"]["bEnableClayLg"] = oldConfig.get("Enable Clay Lg", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Ores"]["Stone"]["bEnableStoneSm"] = oldConfig.get("Enable Stone Sm", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Ores"]["Stone"]["bEnableStoneMed"] = oldConfig.get("Enable Stone Med", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Ores"]["Stone"]["bEnableStoneLg"] = oldConfig.get("Enable Stone Lg", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Ores"]["Copper"]["bEnableCopperSm"] = oldConfig.get("Enable Copper Sm", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Ores"]["Copper"]["bEnableCopperMed"] = oldConfig.get("Enable Copper Med", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Ores"]["Copper"]["bEnableCopperLg"] = oldConfig.get("Enable Copper Lg", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Ores"]["Iron"]["bEnableIronSm"] = oldConfig.get("Enable Iron Sm", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Ores"]["Iron"]["bEnableIronMed"] = oldConfig.get("Enable Iron Med", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Ores"]["Iron"]["bEnableIronLg"] = oldConfig.get("Enable Iron Lg", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Ores"]["Palium"]["bEnablePaliumSm"] = oldConfig.get("Enable Palium Sm", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Ores"]["Palium"]["bEnablePaliumMed"] = oldConfig.get("Enable Palium Med", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Ores"]["Palium"]["bEnablePaliumLg"] = oldConfig.get("Enable Palium Lg", false).asBool();

    // Forageables
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Coral"]["bEnableCoral"] = oldConfig.get("Enable Coral", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Oyster"]["bEnableOyster"] = oldConfig.get("Enable Oyster", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Shell"]["bEnableShell"] = oldConfig.get("Enable Shell", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Poison Flower"]["bEnablePoisonFlower"] = oldConfig.get("Enable PoisonFlower", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Poison Flower"]["bEnablePoisonFlowerP"] = oldConfig.get("Enable PoisonFlower P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Water Flower"]["bEnableWaterFlower"] = oldConfig.get("Enable WaterFlower", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Water Flower"]["bEnableWaterFlowerP"] = oldConfig.get("Enable WaterFlower P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Heartdrop"]["bEnableHeartdrop"] = oldConfig.get("Enable Heartdrop", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Heartdrop"]["bEnableHeartdropP"] = oldConfig.get("Enable Heartdrop P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Sundrop"]["bEnableSundrop"] = oldConfig.get("Enable Sundrop", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Sundrop"]["bEnableSundropP"] = oldConfig.get("Enable Sundrop P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Dragons Beard"]["bEnableDragonsBeard"] = oldConfig.get("Enable DragonsBeard", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Dragons Beard"]["bEnableDragonsBeardP"] = oldConfig.get("Enable DragonsBeard P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Emerald Carpet"]["bEnableEmeraldCarpet"] = oldConfig.get("Enable EmeraldCarpet", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Emerald Carpet"]["bEnableEmeraldCarpetP"] = oldConfig.get("Enable EmeraldCarpet P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Mushroom Blue"]["bEnableMushroomBlue"] = oldConfig.get("Enable MushroomBlue", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Mushroom Blue"]["bEnableMushroomBlueP"] = oldConfig.get("Enable MushroomBlue P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Mushroom Red"]["bEnableMushroomRed"] = oldConfig.get("Enable MushroomRed", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Mushroom Red"]["bEnableMushroomRedP"] = oldConfig.get("Enable MushroomRed P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Dari Cloves"]["bEnableDariCloves"] = oldConfig.get("Enable DariCloves", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Dari Cloves"]["bEnableDariClovesP"] = oldConfig.get("Enable DariCloves P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Heat Root"]["bEnableHeatRoot"] = oldConfig.get("Enable HeatRoot", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Heat Root"]["bEnableHeatRootP"] = oldConfig.get("Enable HeatRoot P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Spiced Sprouts"]["bEnableSpicedSprouts"] = oldConfig.get("Enable SpicedSprouts", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Spiced Sprouts"]["bEnableSpicedSproutsP"] = oldConfig.get("Enable SpicedSprouts P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Sweet Leaves"]["bEnableSweetLeaves"] = oldConfig.get("Enable SweetLeaves", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Sweet Leaves"]["bEnableSweetLeavesP"] = oldConfig.get("Enable SweetLeaves P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Garlic"]["bEnableGarlic"] = oldConfig.get("Enable Garlic", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Garlic"]["bEnableGarlicP"] = oldConfig.get("Enable Garlic P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Ginger"]["bEnableGinger"] = oldConfig.get("Enable Ginger", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Ginger"]["bEnableGingerP"] = oldConfig.get("Enable Ginger P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Green Onion"]["bEnableGreenOnion"] = oldConfig.get("Enable GreenOnion", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Forageables"]["Green Onion"]["bEnableGreenOnionP"] = oldConfig.get("Enable GreenOnion P", false).asBool();

    // Bugs
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Bee"]["Enable Bee U"] = oldConfig.get("Enable Bee U", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Bee"]["Enable Bee U P"] = oldConfig.get("Enable Bee U P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Bee"]["Enable Bee R"] = oldConfig.get("Enable Bee R", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Bee"]["Enable Bee R P"] = oldConfig.get("Enable Bee R P", true).asBool();

    espConfig["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["Enable Beetle C"] = oldConfig.get("Enable Beetle C", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["Enable Beetle C P"] = oldConfig.get("Enable Beetle C P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["Enable Beetle U"] = oldConfig.get("Enable Beetle U", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["Enable Beetle U P"] = oldConfig.get("Enable Beetle U P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["Enable Beetle R"] = oldConfig.get("Enable Beetle R", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["Enable Beetle R P"] = oldConfig.get("Enable Beetle R P", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["Enable Beetle E"] = oldConfig.get("Enable Beetle E", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["Enable Beetle E P"] = oldConfig.get("Enable Beetle E P", true).asBool();

    espConfig["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["Enable Butterfly C"] = oldConfig.get("Enable Butterfly C", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["Enable Butterfly C P"] = oldConfig.get("Enable Butterfly C P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["Enable Butterfly U"] = oldConfig.get("Enable Butterfly U", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["Enable Butterfly U P"] = oldConfig.get("Enable Butterfly U P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["Enable Butterfly R"] = oldConfig.get("Enable Butterfly R", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["Enable Butterfly R P"] = oldConfig.get("Enable Butterfly R P", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["Enable Butterfly E"] = oldConfig.get("Enable Butterfly E", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["Enable Butterfly E P"] = oldConfig.get("Enable Butterfly E P", true).asBool();

    espConfig["ESPSettings"]["Categories"]["Bugs"]["Cicada"]["Enable Cicada C"] = oldConfig.get("Enable Cicada C", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Cicada"]["Enable Cicada C P"] = oldConfig.get("Enable Cicada C P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Cicada"]["Enable Cicada U"] = oldConfig.get("Enable Cicada U", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Cicada"]["Enable Cicada U P"] = oldConfig.get("Enable Cicada U P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Cicada"]["Enable Cicada R"] = oldConfig.get("Enable Cicada R", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Cicada"]["Enable Cicada R P"] = oldConfig.get("Enable Cicada R P", false).asBool();

    espConfig["ESPSettings"]["Categories"]["Bugs"]["Crab"]["Enable Crab C"] = oldConfig.get("Enable Crab C", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Crab"]["Enable Crab C P"] = oldConfig.get("Enable Crab C P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Crab"]["Enable Crab U"] = oldConfig.get("Enable Crab U", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Crab"]["Enable Crab U P"] = oldConfig.get("Enable Crab U P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Crab"]["Enable Crab R"] = oldConfig.get("Enable Crab R", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Crab"]["Enable Crab R P"] = oldConfig.get("Enable Crab R P", true).asBool();

    espConfig["ESPSettings"]["Categories"]["Bugs"]["Cricket"]["Enable Cricket C"] = oldConfig.get("Enable Cricket C", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Cricket"]["Enable Cricket C P"] = oldConfig.get("Enable Cricket C P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Cricket"]["Enable Cricket U"] = oldConfig.get("Enable Cricket U", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Cricket"]["Enable Cricket U P"] = oldConfig.get("Enable Cricket U P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Cricket"]["Enable Cricket R"] = oldConfig.get("Enable Cricket R", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Cricket"]["Enable Cricket R P"] = oldConfig.get("Enable Cricket R P", true).asBool();

    espConfig["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["Enable Dragonfly C"] = oldConfig.get("Enable Dragonfly C", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["Enable Dragonfly C P"] = oldConfig.get("Enable Dragonfly C P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["Enable Dragonfly U"] = oldConfig.get("Enable Dragonfly U", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["Enable Dragonfly U P"] = oldConfig.get("Enable Dragonfly U P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["Enable Dragonfly R"] = oldConfig.get("Enable Dragonfly R", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["Enable Dragonfly R P"] = oldConfig.get("Enable Dragonfly R P", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["Enable Dragonfly E"] = oldConfig.get("Enable Dragonfly E", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["Enable Dragonfly E P"] = oldConfig.get("Enable Dragonfly E P", true).asBool();

    espConfig["ESPSettings"]["Categories"]["Bugs"]["Glowbug"]["Enable Glowbug C"] = oldConfig.get("Enable Glowbug C", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Glowbug"]["Enable Glowbug C P"] = oldConfig.get("Enable Glowbug C P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Glowbug"]["Enable Glowbug U"] = oldConfig.get("Enable Glowbug U", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Glowbug"]["Enable Glowbug U P"] = oldConfig.get("Enable Glowbug U P", false).asBool();

    espConfig["ESPSettings"]["Categories"]["Bugs"]["Ladybug"]["Enable Ladybug C"] = oldConfig.get("Enable Ladybug C", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Ladybug"]["Enable Ladybug C P"] = oldConfig.get("Enable Ladybug C P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Ladybug"]["Enable Ladybug U"] = oldConfig.get("Enable Ladybug U", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Ladybug"]["Enable Ladybug U P"] = oldConfig.get("Enable Ladybug U P", false).asBool();

    espConfig["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["Enable Mantis U"] = oldConfig.get("Enable Mantis U", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["Enable Mantis U P"] = oldConfig.get("Enable Mantis U P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["Enable Mantis R"] = oldConfig.get("Enable Mantis R", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["Enable Mantis R P"] = oldConfig.get("Enable Mantis R P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["Enable Mantis R2"] = oldConfig.get("Enable Mantis R2", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["Enable Mantis R2 P"] = oldConfig.get("Enable Mantis R2 P", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["Enable Mantis E"] = oldConfig.get("Enable Mantis E", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["Enable Mantis E P"] = oldConfig.get("Enable Mantis E P", true).asBool();

    espConfig["ESPSettings"]["Categories"]["Bugs"]["Moth"]["Enable Moth C"] = oldConfig.get("Enable Moth C", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Moth"]["Enable Moth C P"] = oldConfig.get("Enable Moth C P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Moth"]["Enable Moth U"] = oldConfig.get("Enable Moth U", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Moth"]["Enable Moth U P"] = oldConfig.get("Enable Moth U P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Moth"]["Enable Moth R"] = oldConfig.get("Enable Moth R", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Moth"]["Enable Moth R P"] = oldConfig.get("Enable Moth R P", false).asBool();

    espConfig["ESPSettings"]["Categories"]["Bugs"]["Centipede"]["Enable Pede U"] = oldConfig.get("Enable Pede U", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Centipede"]["Enable Pede U P"] = oldConfig.get("Enable Pede U P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Centipede"]["Enable Pede R"] = oldConfig.get("Enable Pede R", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Centipede"]["Enable Pede R P"] = oldConfig.get("Enable Pede R P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Centipede"]["Enable Pede R2"] = oldConfig.get("Enable Pede R2", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Centipede"]["Enable Pede R2 P"] = oldConfig.get("Enable Pede R2 P", false).asBool();

    espConfig["ESPSettings"]["Categories"]["Bugs"]["Snail"]["Enable Snail U"] = oldConfig.get("Enable Snail U", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Snail"]["Enable Snail U P"] = oldConfig.get("Enable Snail U P", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Snail"]["Enable Snail R"] = oldConfig.get("Enable Snail R", true).asBool();
    espConfig["ESPSettings"]["Categories"]["Bugs"]["Snail"]["Enable Snail R P"] = oldConfig.get("Enable Snail R P", true).asBool();

    // Trees
    espConfig["ESPSettings"]["Categories"]["Trees"]["Bush"]["bEnableBushSm"] = oldConfig.get("Enable Bush Sm", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Trees"]["Sapwood"]["bEnableSapwoodSm"] = oldConfig.get("Enable Sapwood Sm", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Trees"]["Sapwood"]["bEnableSapwoodMed"] = oldConfig.get("Enable Sapwood Med", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Trees"]["Sapwood"]["bEnableSapwoodLg"] = oldConfig.get("Enable Sapwood Lg", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Trees"]["Heartwood"]["bEnableHeartwoodSm"] = oldConfig.get("Enable Heartwood Sm", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Trees"]["Heartwood"]["bEnableHeartwoodMed"] = oldConfig.get("Enable Heartwood Med", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Trees"]["Heartwood"]["bEnableHeartwoodLg"] = oldConfig.get("Enable Heartwood Lg", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Trees"]["Flow"]["bEnableFlowSm"] = oldConfig.get("Enable Flow Sm", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Trees"]["Flow"]["bEnableFlowMed"] = oldConfig.get("Enable Flow Med", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Trees"]["Flow"]["bEnableFlowLg"] = oldConfig.get("Enable Flow Lg", false).asBool();

    // Singles
    espConfig["ESPSettings"]["Categories"]["Singles"]["Players"]["bEnablePlayers"] = oldConfig.get("Enable Players", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Singles"]["NPCs"]["bEnableNPC"] = oldConfig.get("Enable NPC", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Singles"]["Fish"]["bEnableFish"] = oldConfig.get("Enable Fish", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Singles"]["Pools"]["bEnablePools"] = oldConfig.get("Enable Pools", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Singles"]["Loot"]["bEnableLoot"] = oldConfig.get("Enable Loot", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Singles"]["Quest"]["bEnableQuest"] = oldConfig.get("Enable Quest", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Singles"]["Rummage Piles"]["bEnableRummagePiles"] = oldConfig.get("Enable Rummage Piles", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Singles"]["Stables"]["bEnableStables"] = oldConfig.get("Enable Stables", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Singles"]["Treasure"]["bEnableTreasure"] = oldConfig.get("Enable Treasure", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Singles"]["Timed Drop"]["bEnableTimedDrop"] = oldConfig.get("Enable TimedDrops", false).asBool();
    espConfig["ESPSettings"]["Categories"]["Singles"]["Others"]["bEnableOthers"] = oldConfig.get("Enable Others", false).asBool();

    std::ofstream espConfigFile(espConfigFilePath);
    espConfigFile << espConfig.toStyledString();
    espConfigFile.close();

    HasMigrated = true;
}

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

        ImVec4 color = ImVec4(r, g, b, a);
        customColors[index] = color;
    }
}

void Configuration::LoadAimSettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open aim settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;

    // AIM SETTINGS
    bEnableAimbot = config["AimSettings"]["Toggles"].get("bEnableAimbot", false).asBool();
    bEnableSilentAimbot = config["AimSettings"]["Toggles"].get("bEnableSilentAimbot", false).asBool();

    FOVRadius = config["AimSettings"]["FOV"]["Radius"].get("FOVRadius", 185.0f).asFloat();
    CircleAlpha = config["AimSettings"]["FOV"]["Transparency"].get("CircleAlpha", 1.0f).asFloat();
    bDrawFOVCircle = config["AimSettings"]["FOV"]["Toggles"].get("bDrawFOVCircle", true).asBool();
    bHideFOVCircle = config["AimSettings"]["FOV"]["Toggles"].get("bHideFOVCircle", false).asBool();

    AvoidanceRadius = config["AimSettings"]["Settings"].get("AvoidanceRadius", 30).asInt();
    bDrawCrosshair = config["AimSettings"]["Settings"].get("bDrawCrosshair", true).asBool();
}

void Configuration::LoadOverlaySettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open overlay settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;

    // OVERLAY SETTINGS
    windowSizeX = config["OverlaySettings"]["MainWindow"].get("windowSizeX", 1450.0f).asFloat();
    windowSizeY = config["OverlaySettings"]["MainWindow"].get("windowSizeY", 950.0f).asFloat();
    selectedTheme = config["OverlaySettings"]["MainWindow"].get("selectedTheme", 0).asInt();

    activeWindowPosX = config["OverlaySettings"]["ModMenu"]["Position"].get("activeWindowPosX", 125.0f).asFloat();
    activeWindowPosY = config["OverlaySettings"]["ModMenu"]["Position"].get("activeWindowPosY", 5.0f).asFloat();
    activeModsOpacity = config["OverlaySettings"]["ModMenu"]["Styling"].get("activeModsOpacity", 0.70f).asFloat();
    activeModsRounding = config["OverlaySettings"]["ModMenu"]["Styling"].get("activeModsRounding", 5.0f).asFloat();
    showEnabledFeaturesWindow = config["OverlaySettings"]["ModMenu"]["Toggled"].get("showEnabledFeaturesWindow", false).asBool();
    activeModsStickToRight = config["OverlaySettings"]["ModMenu"]["Settings"].get("activeModsStickToRight", false).asBool();

    bShowWatermark = config["OverlaySettings"]["Settings"].get("bShowWatermark", true).asBool();
}

void Configuration::LoadModSettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open mod settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;

    // UNCATEGORISED MOD SETTINGS

    bFishingNoDurability = config["Mods"]["Fishing"].get("bFishingNoDurability", true).asBool();
    bFishingMultiplayerHelp = config["Mods"]["Fishing"].get("bFishingMultiplayerHelp", false).asBool();
    bFishingInstantCatch = config["Mods"]["Fishing"].get("bFishingInstantCatch", false).asBool();
    bFishingPerfectCatch = config["Mods"]["Fishing"].get("bFishingPerfectCatch", true).asBool();
    bFishingSell = config["Mods"]["Fishing"].get("bFishingSell", false).asBool();
    bFishingDiscard = config["Mods"]["Fishing"].get("bFishingDiscard", false).asBool();
    bFishingOpenStoreWaterlogged = config["Mods"]["Fishing"].get("bFishingOpenStoreWaterlogged", false).asBool();
    bRequireClickFishing = config["Mods"]["Fishing"].get("bRequireClickFishing", true).asBool();

    bEnableInteractionMods = config["Mods"]["Interaction Mods"].get("bEnableInteractionMods", false).asBool();
    InteractionRadius = config["Mods"]["Interaction Mods"].get("InteractionRadius", 500.0f).asFloat();

    bEnablePrizeWheel = config["Mods"]["PrizeWheel"]["Toggled"].get("bEnablePrizeWheel", false).asBool();
    bPrizeWheelUseLocked = config["Mods"]["PrizeWheel"]["Settings"].get("bPrizeWheelUseLocked", false).asBool();
    for (int i = 0; i < 9; ++i) {
        std::string key = "PrizeWheelSlot" + std::to_string(i);
        PrizeWheelSlots[i] = config["Mods"]["PrizeWheel"]["Settings"].get(key, false).asBool();
    }

#ifdef ENABLE_SUPPORTER_FEATURES
    bEnableFreeLandscape = config["Mods"]["Unlockables"].get("bEnableFreeLandscape", false).asBool();
    bEnableOutfitCustomization = config["Mods"]["Unlockables"].get("bEnableOutfitCustomization", false).asBool();

    bEnableAutoToolUse = config["Mods"]["Misc"].get("bEnableAutoToolUse", false).asBool();
#endif
    bEnableAutoGather = config["Mods"]["Misc"].get("bEnableAutoGather", false).asBool();
    bEnableRequestMods = config["Mods"]["Misc"].get("bEnableRequestMods", false).asBool();
    bEasyModeActive = config["Mods"]["Misc"].get("bEasyModeActive", false).asBool();
    bPlaceAnywhere = config["Mods"]["Misc"].get("bPlaceAnywhere", false).asBool();
    bEnableAntiAfk = config["Mods"]["Misc"].get("bEnableAntiAfk", false).asBool();
    bEnableUnlimitedWardrobeRespec = config["Mods"]["Misc"].get("bEnableUnlimitedWardrobeRespec", false).asBool();
    bEnableMinigameSkip = config["Mods"]["Misc"].get("bEnableMinigameSkip", false).asBool();
}

void Configuration::LoadMovementSettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open movement settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;

    // MOVEMENT SETTINGS
    CustomWalkSpeed = config["MovementSettings"]["Values"].get("CustomWalkSpeed", 565.0f).asFloat();
    CustomSprintSpeedMultiplier = config["MovementSettings"]["Values"].get("CustomSprintSpeedMultiplier", 1.65f).asFloat();
    CustomClimbingSpeed = config["MovementSettings"]["Values"].get("CustomClimbingSpeed", 80.0f).asFloat();
    CustomGlidingSpeed = config["MovementSettings"]["Values"].get("CustomGlidingSpeed", 900.0f).asFloat();
    CustomGlidingFallSpeed = config["MovementSettings"]["Values"].get("CustomGlidingFallSpeed", 250.0f).asFloat();
    CustomJumpVelocity = config["MovementSettings"]["Values"].get("CustomJumpVelocity", 700.0f).asFloat();
    CustomMaxStepHeight = config["MovementSettings"]["Values"].get("CustomMaxStepHeight", 45.0f).asFloat();

    bEnableWaypointTeleport = config["TeleportSettings"]["Toggles"].get("bEnableWaypointTeleport", false).asBool();
    bTeleportToTargeted = config["TeleportSettings"]["Toggles"].get("bTeleportToTargeted", true).asBool();
    bAvoidTeleportingToPlayers = config["TeleportSettings"]["Toggles"].get("bAvoidTeleportingToPlayers", true).asBool();
    bDoRadiusPlayersAvoidance = config["TeleportSettings"]["Values"].get("bDoRadiusPlayersAvoidance", true).asBool();
}

void Configuration::LoadESPSettings(std::ifstream& configStream) {
    if (!configStream.is_open()) {
        std::cerr << "Failed to open esp settings file." << '\n';
        return;
    }

    Json::Value config;
    configStream >> config;

    float version = config.get("Version", 1.0f).asFloat();
    if (version < ESPVersion) {
        // ESP SETTINGS
        bEnableESP = config["ESPSettings"]["Toggle"].get("bEnableESP", true).asBool();
        bESPIcons = config["ESPSettings"]["Icons"].get("bESPIcons", false).asBool();
        bESPIconDistance = config["ESPSettings"]["Icons"].get("bESPIconDistance", false).asBool();
        //bEnableESPCulling = config["ESPSettings"]["Culling"].get("bEnableESPCulling", true).asBool();
        CullDistance = config["ESPSettings"]["Culling"].get("CullDistance", 200).asInt();
        //ESPTextScale = config["ESPSettings"]["Others"].get("ESPTextScale", 1.0f).asFloat(); // DISABLED: ImGui Font-resizing not setup

        // Is there not an array to loop for this?
        // Animals
        *bEnableSernuk = config["ESPSettings"]["Categories"]["Animals"].get("bEnableSernuk", false).asBool();
        *bEnableElderSernuk = config["ESPSettings"]["Categories"]["Animals"].get("bEnableElderSernuk", false).asBool();
        *bEnableProudhornSernuk = config["ESPSettings"]["Categories"]["Animals"].get("bEnableProudhornSernuk", false).asBool();
        *bEnableChapaa = config["ESPSettings"]["Categories"]["Animals"].get("bEnableChapaa", false).asBool();
        *bEnableStripedChapaa = config["ESPSettings"]["Categories"]["Animals"].get("bEnableStripedChapaa", false).asBool();
        *bEnableAzureChapaa = config["ESPSettings"]["Categories"]["Animals"].get("bEnableAzureChapaa", false).asBool();
        *bEnableMinigameChapaa = config["ESPSettings"]["Categories"]["Animals"].get("bEnableMinigameChapaa", false).asBool();
        *bEnableMuujin = config["ESPSettings"]["Categories"]["Animals"].get("bEnableMuujin", false).asBool();
        *bEnableBandedMuujin = config["ESPSettings"]["Categories"]["Animals"].get("bEnableBandedMuujin", false).asBool();
        *bEnableBluebristleMuujin = config["ESPSettings"]["Categories"]["Animals"].get("bEnableBluebristleMuujin", false).asBool();

        // Ores
        *bEnableClayLg = config["ESPSettings"]["Categories"]["Ores"]["Clay"].get("bEnableClayLg", false).asBool();
        *bEnableStoneSm = config["ESPSettings"]["Categories"]["Ores"]["Stone"].get("bEnableStoneSm", false).asBool();
        *bEnableStoneMed = config["ESPSettings"]["Categories"]["Ores"]["Stone"].get("bEnableStoneMed", false).asBool();
        *bEnableStoneLg = config["ESPSettings"]["Categories"]["Ores"]["Stone"].get("bEnableStoneLg", false).asBool();
        *bEnableCopperSm = config["ESPSettings"]["Categories"]["Ores"]["Copper"].get("bEnableCopperSm", false).asBool();
        *bEnableCopperMed = config["ESPSettings"]["Categories"]["Ores"]["Copper"].get("bEnableCopperMed", false).asBool();
        *bEnableCopperLg = config["ESPSettings"]["Categories"]["Ores"]["Copper"].get("bEnableCopperLg", false).asBool();
        *bEnableIronSm = config["ESPSettings"]["Categories"]["Ores"]["Iron"].get("bEnableIronSm", false).asBool();
        *bEnableIronMed = config["ESPSettings"]["Categories"]["Ores"]["Iron"].get("bEnableIronMed", false).asBool();
        *bEnableIronLg = config["ESPSettings"]["Categories"]["Ores"]["Iron"].get("bEnableIronLg", false).asBool();
        *bEnablePaliumSm = config["ESPSettings"]["Categories"]["Ores"]["Palium"].get("bEnablePaliumSm", false).asBool();
        *bEnablePaliumMed = config["ESPSettings"]["Categories"]["Ores"]["Palium"].get("bEnablePaliumMed", false).asBool();
        *bEnablePaliumLg = config["ESPSettings"]["Categories"]["Ores"]["Palium"].get("bEnablePaliumLg", false).asBool();

        // Forageables
        *bEnableCoral = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableCoral", false).asBool();
        *bEnableOyster = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableOyster", false).asBool();
        *bEnableShell = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableShell", false).asBool();
        *bEnablePoisonFlower = config["ESPSettings"]["Categories"]["Forageables"].get("bEnablePoisonFlower", false).asBool();
        *bEnablePoisonFlowerP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnablePoisonFlowerP", false).asBool();
        *bEnableWaterFlower = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableWaterFlower", false).asBool();
        *bEnableWaterFlowerP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableWaterFlowerP", false).asBool();
        *bEnableHeartdrop = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableHeartdrop", false).asBool();
        *bEnableHeartdropP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableHeartdropP", false).asBool();
        *bEnableSundrop = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableSundrop", false).asBool();
        *bEnableSundropP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableSundropP", false).asBool();
        *bEnableDragonsBeard = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableDragonsBeard", false).asBool();
        *bEnableDragonsBeardP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableDragonsBeardP", false).asBool();
        *bEnableEmeraldCarpet = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableEmeraldCarpet", false).asBool();
        *bEnableEmeraldCarpetP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableEmeraldCarpetP", false).asBool();
        *bEnableMushroomBlue = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableMushroomBlue", false).asBool();
        *bEnableMushroomBlueP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableMushroomBlueP", false).asBool();
        *bEnableMushroomRed = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableMushroomRed", false).asBool();
        *bEnableMushroomRedP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableMushroomRedP", false).asBool();
        *bEnableDariCloves = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableDariCloves", false).asBool();
        *bEnableDariClovesP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableDariClovesP", false).asBool();
        *bEnableHeatRoot = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableHeatRoot", false).asBool();
        *bEnableHeatRootP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableHeatRootP", false).asBool();
        *bEnableSpicedSprouts = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableSpicedSprouts", false).asBool();
        *bEnableSpicedSproutsP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableSpicedSproutsP", false).asBool();
        *bEnableSweetLeaves = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableSweetLeaves", false).asBool();
        *bEnableSweetLeavesP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableSweetLeavesP", false).asBool();
        *bEnableGarlic = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableGarlic", false).asBool();
        *bEnableGarlicP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableGarlicP", false).asBool();
        *bEnableGinger = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableGinger", false).asBool();
        *bEnableGingerP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableGingerP", false).asBool();
        *bEnableGreenOnion = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableGreenOnion", false).asBool();
        *bEnableGreenOnionP = config["ESPSettings"]["Categories"]["Forageables"].get("bEnableGreenOnionP", false).asBool();

        // Bugs
        *bEnableBeeU = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableBeeU", false).asBool();
        *bEnableBeeUP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableBeeUP", false).asBool();
        *bEnableBeeR = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableBeeR", false).asBool();
        *bEnableBeeRP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableBeeRP", false).asBool();
        *bEnableBeetleC = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableBeetleC", false).asBool();
        *bEnableBeetleCP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableBeetleCP", false).asBool();
        *bEnableBeetleU = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableBeetleU", false).asBool();
        *bEnableBeetleUP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableBeetleUP", false).asBool();
        *bEnableBeetleR = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableBeetleR", false).asBool();
        *bEnableBeetleRP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableBeetleRP", false).asBool();
        *bEnableBeetleE = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableBeetleE", false).asBool();
        *bEnableBeetleEP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableBeetleEP", false).asBool();
        *bEnableButterflyC = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableButterflyC", false).asBool();
        *bEnableButterflyCP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableButterflyCP", false).asBool();
        *bEnableButterflyU = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableButterflyU", false).asBool();
        *bEnableButterflyUP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableButterflyUP", false).asBool();
        *bEnableButterflyR = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableButterflyR", false).asBool();
        *bEnableButterflyRP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableButterflyRP", false).asBool();
        *bEnableButterflyE = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableButterflyE", false).asBool();
        *bEnableButterflyEP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableButterflyEP", false).asBool();
        *bEnableCicadaC = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCicadaC", false).asBool();
        *bEnableCicadaCP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCicadaCP", false).asBool();
        *bEnableCicadaU = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCicadaU", false).asBool();
        *bEnableCicadaUP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCicadaUP", false).asBool();
        *bEnableCicadaR = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCicadaR", false).asBool();
        *bEnableCicadaRP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCicadaRP", false).asBool();
        *bEnableCrabC = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCrabC", false).asBool();
        *bEnableCrabCP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCrabCP", false).asBool();
        *bEnableCrabU = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCrabU", false).asBool();
        *bEnableCrabUP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCrabUP", false).asBool();
        *bEnableCrabR = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCrabR", false).asBool();
        *bEnableCrabRP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCrabRP", false).asBool();
        *bEnableCricketC = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCricketC", false).asBool();
        *bEnableCricketCP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCricketCP", false).asBool();
        *bEnableCricketU = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCricketU", false).asBool();
        *bEnableCricketUP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCricketUP", false).asBool();
        *bEnableCricketR = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCricketR", false).asBool();
        *bEnableCricketRP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableCricketRP", false).asBool();
        *bEnableDragonflyC = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableDragonflyC", false).asBool();
        *bEnableDragonflyCP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableDragonflyCP", false).asBool();
        *bEnableDragonflyU = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableDragonflyU", false).asBool();
        *bEnableDragonflyUP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableDragonflyUP", false).asBool();
        *bEnableDragonflyR = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableDragonflyR", false).asBool();
        *bEnableDragonflyRP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableDragonflyRP", false).asBool();
        *bEnableDragonflyE = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableDragonflyE", false).asBool();
        *bEnableDragonflyEP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableDragonflyEP", false).asBool();
        *bEnableGlowbugC = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableGlowbugC", false).asBool();
        *bEnableGlowbugCP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableGlowbugCP", false).asBool();
        *bEnableGlowbugU = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableGlowbugU", false).asBool();
        *bEnableGlowbugUP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableGlowbugUP", false).asBool();
        *bEnableLadybugC = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableLadybugC", false).asBool();
        *bEnableLadybugCP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableLadybugCP", false).asBool();
        *bEnableLadybugU = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableLadybugU", false).asBool();
        *bEnableLadybugUP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableLadybugUP", false).asBool();
        *bEnableMantisU = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMantisU", false).asBool();
        *bEnableMantisUP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMantisUP", false).asBool();
        *bEnableMantisR = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMantisR", false).asBool();
        *bEnableMantisRP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMantisRP", false).asBool();
        *bEnableMantisR2 = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMantisR2", false).asBool();
        *bEnableMantisR2P = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMantisR2P", false).asBool();
        *bEnableMantisE = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMantisE", false).asBool();
        *bEnableMantisEP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMantisEP", false).asBool();
        *bEnableMothC = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMothC", false).asBool();
        *bEnableMothCP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMothCP", false).asBool();
        *bEnableMothU = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMothU", false).asBool();
        *bEnableMothUP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMothUP", false).asBool();
        *bEnableMothR = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMothR", false).asBool();
        *bEnableMothRP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableMothRP", false).asBool();
        *bEnablePedeU = config["ESPSettings"]["Categories"]["Bugs"].get("bEnablePedeU", false).asBool();
        *bEnablePedeUP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnablePedeUP", false).asBool();
        *bEnablePedeR = config["ESPSettings"]["Categories"]["Bugs"].get("bEnablePedeR", false).asBool();
        *bEnablePedeRP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnablePedeRP", false).asBool();
        *bEnablePedeR2 = config["ESPSettings"]["Categories"]["Bugs"].get("bEnablePedeR2", false).asBool();
        *bEnablePedeR2P = config["ESPSettings"]["Categories"]["Bugs"].get("bEnablePedeR2P", false).asBool();
        *bEnableSnailU = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableSnailU", false).asBool();
        *bEnableSnailUP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableSnailUP", false).asBool();
        *bEnableSnailR = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableSnailR", false).asBool();
        *bEnableSnailRP = config["ESPSettings"]["Categories"]["Bugs"].get("bEnableSnailRP", false).asBool();

        // Trees
        *bEnableBushSm = config["ESPSettings"]["Categories"]["Trees"].get("bEnableBushSm", false).asBool();
        *bEnableSapwoodSm = config["ESPSettings"]["Categories"]["Trees"].get("bEnableSapwoodSm", false).asBool();
        *bEnableSapwoodMed = config["ESPSettings"]["Categories"]["Trees"].get("bEnableSapwoodMed", false).asBool();
        *bEnableSapwoodLg = config["ESPSettings"]["Categories"]["Trees"].get("bEnableSapwoodLg", false).asBool();
        *bEnableHeartwoodSm = config["ESPSettings"]["Categories"]["Trees"].get("bEnableHeartwoodSm", false).asBool();
        *bEnableHeartwoodMed = config["ESPSettings"]["Categories"]["Trees"].get("bEnableHeartwoodMed", false).asBool();
        *bEnableHeartwoodLg = config["ESPSettings"]["Categories"]["Trees"].get("bEnableHeartwoodLg", false).asBool();
        *bEnableFlowSm = config["ESPSettings"]["Categories"]["Trees"].get("bEnableFlowSm", false).asBool();
        *bEnableFlowMed = config["ESPSettings"]["Categories"]["Trees"].get("bEnableFlowMed", false).asBool();
        *bEnableFlowLg = config["ESPSettings"]["Categories"]["Trees"].get("bEnableFlowLg", false).asBool();

        // Singles
        *bEnablePlayers = config["ESPSettings"]["Categories"]["Singles"]["Players"].get("bEnablePlayers", false).asBool();
        *bEnableNPC = config["ESPSettings"]["Categories"]["Singles"]["NPCs"].get("bEnableNPC", false).asBool();
        *bEnableFish = config["ESPSettings"]["Categories"]["Singles"]["Fish"].get("bEnableFish", false).asBool();
        *bEnablePools = config["ESPSettings"]["Categories"]["Singles"]["Pools"].get("bEnablePools", false).asBool();
        *bEnableLoot = config["ESPSettings"]["Categories"]["Singles"]["Loot"].get("bEnableLoot", false).asBool();
        *bEnableQuest = config["ESPSettings"]["Categories"]["Singles"]["Quest"].get("bEnableQuest", false).asBool();
        *bEnableRummagePiles = config["ESPSettings"]["Categories"]["Singles"]["Rummage Piles"].get("bEnableRummagePiles", false).asBool();
        *bEnableStables = config["ESPSettings"]["Categories"]["Singles"]["Stables"].get("bEnableStables", false).asBool();
        *bEnableTreasure = config["ESPSettings"]["Categories"]["Singles"]["Treasure"].get("bEnableTreasure", false).asBool();
        *bEnableTimedDrop = config["ESPSettings"]["Categories"]["Singles"]["Timed Drop"].get("bEnableTimedDrop", false).asBool();
        bEnableOthers = config["ESPSettings"]["Categories"]["Singles"]["Others"].get("bEnableOthers", false).asBool();
    }
    else if (version == ESPVersion) {
        // ESP SETTINGS
        bEnableESP = config["ESPSettings"]["Toggle"].get("bEnableESP", true).asBool();
        bESPIcons = config["ESPSettings"]["Icons"].get("bESPIcons", false).asBool();
        bESPIconDistance = config["ESPSettings"]["Icons"].get("bESPIconDistance", false).asBool();
        //bEnableESPCulling = config["ESPSettings"]["Culling"].get("bEnableESPCulling", true).asBool();
        CullDistance = config["ESPSettings"]["Culling"].get("CullDistance", 200).asInt();
        //ESPTextScale = config["ESPSettings"]["Others"].get("ESPTextScale", 1.0f).asFloat(); // DISABLED: ImGui Font-resizing not setup

        // Is there not an array to loop for this?
        // Animals
        *bEnableSernuk = config["ESPSettings"]["Categories"]["Animals"]["Sernuk"].get("bEnableSernuk", false).asBool();
        *bEnableElderSernuk = config["ESPSettings"]["Categories"]["Animals"]["Sernuk"].get("bEnableElderSernuk", false).asBool();
        *bEnableProudhornSernuk = config["ESPSettings"]["Categories"]["Animals"]["Sernuk"].get("bEnableProudhornSernuk", false).asBool();
        *bEnableChapaa = config["ESPSettings"]["Categories"]["Animals"]["Chapaa"].get("bEnableChapaa", false).asBool();
        *bEnableStripedChapaa = config["ESPSettings"]["Categories"]["Animals"]["Chapaa"].get("bEnableStripedChapaa", false).asBool();
        *bEnableAzureChapaa = config["ESPSettings"]["Categories"]["Animals"]["Chapaa"].get("bEnableAzureChapaa", false).asBool();
        *bEnableMinigameChapaa = config["ESPSettings"]["Categories"]["Animals"]["Chapaa"].get("bEnableMinigameChapaa", false).asBool();
        *bEnableMuujin = config["ESPSettings"]["Categories"]["Animals"]["Muujin"].get("bEnableMuujin", false).asBool();
        *bEnableBandedMuujin = config["ESPSettings"]["Categories"]["Animals"]["Muujin"].get("bEnableBandedMuujin", false).asBool();
        *bEnableBluebristleMuujin = config["ESPSettings"]["Categories"]["Animals"]["Muujin"].get("bEnableBluebristleMuujin", false).asBool();
        // Animal Colors
        {
            for (const auto& animalTypeName : config["ESPSettings"]["Categories"]["Animals"].getMemberNames()) {
                ECreatureKind kind;

                if (animalTypeName == "Sernuk") kind = ECreatureKind::Cearnuk;
                else if (animalTypeName == "Chapaa") kind = ECreatureKind::Chapaa;
                else if (animalTypeName == "Muujin") kind = ECreatureKind::TreeClimber;
                else
                    continue;

                for (const auto& animalQuality : config["ESPSettings"]["Categories"]["Animals"][animalTypeName].getMemberNames()) {
                    ECreatureQuality quality;
                    std::string qualityStr;
                    if (animalQuality == "bEnableSernuk") { // Tier1
                        quality = ECreatureQuality::Tier1;
                        qualityStr = "Tier 1";
                    }
                    else if (animalQuality == "bEnableElderSernuk") { // Tier2
                        quality = ECreatureQuality::Tier2;
                        qualityStr = "Tier 2";
                    }
                    else if (animalQuality == "bEnableProudhornSernuk") { // Tier3
                        quality = ECreatureQuality::Tier3;
                        qualityStr = "Tier 3";
                    }

                    else if (animalQuality == "bEnableChapaa") { // Tier1
                        quality = ECreatureQuality::Tier1;
                        qualityStr = "Tier 1";
                    }
                    else if (animalQuality == "bEnableStripedChapaa") { // Tier2
                        quality = ECreatureQuality::Tier2;
                        qualityStr = "Tier 2";
                    }
                    else if (animalQuality == "bEnableAzureChapaa") { // Tier3
                        quality = ECreatureQuality::Tier3;
                        qualityStr = "Tier 3";
                    }
                    else if (animalQuality == "bEnableMinigameChapaa") { // Tier4
                        quality = ECreatureQuality::Chase;
                        qualityStr = "Chase";
                    }

                    else if (animalQuality == "bEnableMuujin") { // Tier1
                        quality = ECreatureQuality::Tier1;
                        qualityStr = "Tier 1";
                    }
                    else if (animalQuality == "bEnableBandedMuujin") { // Tier2
                        quality = ECreatureQuality::Tier2;
                        qualityStr = "Tier 2";
                    }
                    else if (animalQuality == "bEnableBluebristleMuujin") { // Tier3
                        quality = ECreatureQuality::Tier3;
                        qualityStr = "Tier 3";
                    }
                    else
                        continue;

                    if (config["ESPSettings"]["Categories"]["Animals"][animalTypeName]["Colors"].isMember(qualityStr)) {
                        ImU32 color = config["ESPSettings"]["Categories"]["Animals"][animalTypeName]["Colors"][qualityStr].asUInt();
                        AnimalColors[{kind, quality}] = color;
                    }
                }

            }
        }

        // Ores
        *bEnableClayLg = config["ESPSettings"]["Categories"]["Ores"]["Clay"].get("bEnableClayLg", false).asBool();
        *bEnableStoneSm = config["ESPSettings"]["Categories"]["Ores"]["Stone"].get("bEnableStoneSm", false).asBool();
        *bEnableStoneMed = config["ESPSettings"]["Categories"]["Ores"]["Stone"].get("bEnableStoneMed", false).asBool();
        *bEnableStoneLg = config["ESPSettings"]["Categories"]["Ores"]["Stone"].get("bEnableStoneLg", false).asBool();
        *bEnableCopperSm = config["ESPSettings"]["Categories"]["Ores"]["Copper"].get("bEnableCopperSm", false).asBool();
        *bEnableCopperMed = config["ESPSettings"]["Categories"]["Ores"]["Copper"].get("bEnableCopperMed", false).asBool();
        *bEnableCopperLg = config["ESPSettings"]["Categories"]["Ores"]["Copper"].get("bEnableCopperLg", false).asBool();
        *bEnableIronSm = config["ESPSettings"]["Categories"]["Ores"]["Iron"].get("bEnableIronSm", false).asBool();
        *bEnableIronMed = config["ESPSettings"]["Categories"]["Ores"]["Iron"].get("bEnableIronMed", false).asBool();
        *bEnableIronLg = config["ESPSettings"]["Categories"]["Ores"]["Iron"].get("bEnableIronLg", false).asBool();
        *bEnablePaliumSm = config["ESPSettings"]["Categories"]["Ores"]["Palium"].get("bEnablePaliumSm", false).asBool();
        *bEnablePaliumMed = config["ESPSettings"]["Categories"]["Ores"]["Palium"].get("bEnablePaliumMed", false).asBool();
        *bEnablePaliumLg = config["ESPSettings"]["Categories"]["Ores"]["Palium"].get("bEnablePaliumLg", false).asBool();
        // Ore Colors
        {
            for (const auto& oreTypeName : config["ESPSettings"]["Categories"]["Ores"].getMemberNames()) {
                EOreType oreType;
                if (oreTypeName == "Clay") oreType = EOreType::Clay;
                else if (oreTypeName == "Stone") oreType = EOreType::Stone;
                else if (oreTypeName == "Copper") oreType = EOreType::Copper;
                else if (oreTypeName == "Iron") oreType = EOreType::Iron;
                else if (oreTypeName == "Palium") oreType = EOreType::Palium;
                else
                    continue;

                if (config["ESPSettings"]["Categories"]["Ores"][oreTypeName].isMember("Color")) {
                    ImU32 color = config["ESPSettings"]["Categories"]["Ores"][oreTypeName]["Color"].asUInt();
                    OreColors[oreType] = color;
                }
            }
        }

        // Forageables
        *bEnableCoral = config["ESPSettings"]["Categories"]["Forageables"]["Coral"].get("bEnableCoral", false).asBool();
        *bEnableOyster = config["ESPSettings"]["Categories"]["Forageables"]["Oyster"].get("bEnableOyster", false).asBool();
        *bEnableShell = config["ESPSettings"]["Categories"]["Forageables"]["Shell"].get("bEnableShell", false).asBool();
        *bEnablePoisonFlower = config["ESPSettings"]["Categories"]["Forageables"]["Poison Flower"].get("bEnablePoisonFlower", false).asBool();
        *bEnablePoisonFlowerP = config["ESPSettings"]["Categories"]["Forageables"]["Poison Flower"].get("bEnablePoisonFlowerP", false).asBool();
        *bEnableWaterFlower = config["ESPSettings"]["Categories"]["Forageables"]["Water Flower"].get("bEnableWaterFlower", false).asBool();
        *bEnableWaterFlowerP = config["ESPSettings"]["Categories"]["Forageables"]["Water Flower"].get("bEnableWaterFlowerP", false).asBool();
        *bEnableHeartdrop = config["ESPSettings"]["Categories"]["Forageables"]["Heartdrop"].get("bEnableHeartdrop", false).asBool();
        *bEnableHeartdropP = config["ESPSettings"]["Categories"]["Forageables"]["Heartdrop"].get("bEnableHeartdropP", false).asBool();
        *bEnableSundrop = config["ESPSettings"]["Categories"]["Forageables"]["Sundrop"].get("bEnableSundrop", false).asBool();
        *bEnableSundropP = config["ESPSettings"]["Categories"]["Forageables"]["Sundrop"].get("bEnableSundropP", false).asBool();
        *bEnableDragonsBeard = config["ESPSettings"]["Categories"]["Forageables"]["Dragons Beard"].get("bEnableDragonsBeard", false).asBool();
        *bEnableDragonsBeardP = config["ESPSettings"]["Categories"]["Forageables"]["Dragons Beard"].get("bEnableDragonsBeardP", false).asBool();
        *bEnableEmeraldCarpet = config["ESPSettings"]["Categories"]["Forageables"]["Emerald Carpet"].get("bEnableEmeraldCarpet", false).asBool();
        *bEnableEmeraldCarpetP = config["ESPSettings"]["Categories"]["Forageables"]["Emerald Carpet"].get("bEnableEmeraldCarpetP", false).asBool();
        *bEnableMushroomBlue = config["ESPSettings"]["Categories"]["Forageables"]["Mushroom Blue"].get("bEnableMushroomBlue", false).asBool();
        *bEnableMushroomBlueP = config["ESPSettings"]["Categories"]["Forageables"]["Mushroom Blue"].get("bEnableMushroomBlueP", false).asBool();
        *bEnableMushroomRed = config["ESPSettings"]["Categories"]["Forageables"]["Mushroom Red"].get("bEnableMushroomRed", false).asBool();
        *bEnableMushroomRedP = config["ESPSettings"]["Categories"]["Forageables"]["Mushroom Red"].get("bEnableMushroomRedP", false).asBool();
        *bEnableDariCloves = config["ESPSettings"]["Categories"]["Forageables"]["Dari Cloves"].get("bEnableDariCloves", false).asBool();
        *bEnableDariClovesP = config["ESPSettings"]["Categories"]["Forageables"]["Dari Cloves"].get("bEnableDariClovesP", false).asBool();
        *bEnableHeatRoot = config["ESPSettings"]["Categories"]["Forageables"]["Heat Root"].get("bEnableHeatRoot", false).asBool();
        *bEnableHeatRootP = config["ESPSettings"]["Categories"]["Forageables"]["Heat Root"].get("bEnableHeatRootP", false).asBool();
        *bEnableSpicedSprouts = config["ESPSettings"]["Categories"]["Forageables"]["Spiced Sprouts"].get("bEnableSpicedSprouts", false).asBool();
        *bEnableSpicedSproutsP = config["ESPSettings"]["Categories"]["Forageables"]["Spiced Sprouts"].get("bEnableSpicedSproutsP", false).asBool();
        *bEnableSweetLeaves = config["ESPSettings"]["Categories"]["Forageables"]["Sweet Leaves"].get("bEnableSweetLeaves", false).asBool();
        *bEnableSweetLeavesP = config["ESPSettings"]["Categories"]["Forageables"]["Sweet Leaves"].get("bEnableSweetLeavesP", false).asBool();
        *bEnableGarlic = config["ESPSettings"]["Categories"]["Forageables"]["Garlic"].get("bEnableGarlic", false).asBool();
        *bEnableGarlicP = config["ESPSettings"]["Categories"]["Forageables"]["Garlic"].get("bEnableGarlicP", false).asBool();
        *bEnableGinger = config["ESPSettings"]["Categories"]["Forageables"]["Ginger"].get("bEnableGinger", false).asBool();
        *bEnableGingerP = config["ESPSettings"]["Categories"]["Forageables"]["Ginger"].get("bEnableGingerP", false).asBool();
        *bEnableGreenOnion = config["ESPSettings"]["Categories"]["Forageables"]["Green Onion"].get("bEnableGreenOnion", false).asBool();
        *bEnableGreenOnionP = config["ESPSettings"]["Categories"]["Forageables"]["Green Onion"].get("bEnableGreenOnionP", false).asBool();
        // Forageable Colors
        {
            for (const auto& forageableTypeName : config["ESPSettings"]["Categories"]["Forageables"].getMemberNames()) {
                EForageableType type;
                if (forageableTypeName == "Coral") type = EForageableType::Coral;
                else if (forageableTypeName == "Oyster") type = EForageableType::Oyster;
                else if (forageableTypeName == "Shell") type = EForageableType::Shell;
                else if (forageableTypeName == "Poison Flower") type = EForageableType::PoisonFlower;
                else if (forageableTypeName == "Water Flower") type = EForageableType::WaterFlower;
                else if (forageableTypeName == "Heartdrop") type = EForageableType::Heartdrop;
                else if (forageableTypeName == "Sundrop") type = EForageableType::Sundrop;
                else if (forageableTypeName == "Dragons Beard") type = EForageableType::DragonsBeard;
                else if (forageableTypeName == "Emerald Carpet") type = EForageableType::EmeraldCarpet;
                else if (forageableTypeName == "Mushroom Blue") type = EForageableType::MushroomBlue;
                else if (forageableTypeName == "Mushroom Red") type = EForageableType::MushroomRed;
                else if (forageableTypeName == "Dari Cloves") type = EForageableType::DariCloves;
                else if (forageableTypeName == "Heat Root") type = EForageableType::HeatRoot;
                else if (forageableTypeName == "Spiced Sprouts") type = EForageableType::SpicedSprouts;
                else if (forageableTypeName == "Sweet Leaves") type = EForageableType::SweetLeaves;
                else if (forageableTypeName == "Garlic") type = EForageableType::Garlic;
                else if (forageableTypeName == "Ginger") type = EForageableType::Ginger;
                else if (forageableTypeName == "Green Onion") type = EForageableType::GreenOnion;
                else
                    continue;

                if (config["ESPSettings"]["Categories"]["Forageables"][forageableTypeName].isMember("Color")) {
                    ImU32 color = config["ESPSettings"]["Categories"]["Forageables"][forageableTypeName]["Color"].asUInt();
                    ForageableColors[type] = color;
                }
            }
        }

        // Bugs
        *bEnableBeeU = config["ESPSettings"]["Categories"]["Bugs"]["Bee"].get("bEnableBeeU", false).asBool();
        *bEnableBeeUP = config["ESPSettings"]["Categories"]["Bugs"]["Bee"].get("bEnableBeeUP", false).asBool();
        *bEnableBeeR = config["ESPSettings"]["Categories"]["Bugs"]["Bee"].get("bEnableBeeR", false).asBool();
        *bEnableBeeRP = config["ESPSettings"]["Categories"]["Bugs"]["Bee"].get("bEnableBeeRP", false).asBool();

        *bEnableBeetleC = config["ESPSettings"]["Categories"]["Bugs"]["Beetle"].get("bEnableBeetleC", false).asBool();
        *bEnableBeetleCP = config["ESPSettings"]["Categories"]["Bugs"]["Beetle"].get("bEnableBeetleCP", false).asBool();
        *bEnableBeetleU = config["ESPSettings"]["Categories"]["Bugs"]["Beetle"].get("bEnableBeetleU", false).asBool();
        *bEnableBeetleUP = config["ESPSettings"]["Categories"]["Bugs"]["Beetle"].get("bEnableBeetleUP", false).asBool();
        *bEnableBeetleR = config["ESPSettings"]["Categories"]["Bugs"]["Beetle"].get("bEnableBeetleR", false).asBool();
        *bEnableBeetleRP = config["ESPSettings"]["Categories"]["Bugs"]["Beetle"].get("bEnableBeetleRP", false).asBool();
        *bEnableBeetleE = config["ESPSettings"]["Categories"]["Bugs"]["Beetle"].get("bEnableBeetleE", false).asBool();
        *bEnableBeetleEP = config["ESPSettings"]["Categories"]["Bugs"]["Beetle"].get("bEnableBeetleEP", false).asBool();

        *bEnableButterflyC = config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"].get("bEnableButterflyC", false).asBool();
        *bEnableButterflyCP = config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"].get("bEnableButterflyCP", false).asBool();
        *bEnableButterflyU = config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"].get("bEnableButterflyU", false).asBool();
        *bEnableButterflyUP = config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"].get("bEnableButterflyUP", false).asBool();
        *bEnableButterflyR = config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"].get("bEnableButterflyR", false).asBool();
        *bEnableButterflyRP = config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"].get("bEnableButterflyRP", false).asBool();
        *bEnableButterflyE = config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"].get("bEnableButterflyE", false).asBool();
        *bEnableButterflyEP = config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"].get("bEnableButterflyEP", false).asBool();

        *bEnableCicadaC = config["ESPSettings"]["Categories"]["Bugs"]["Cicada"].get("bEnableCicadaC", false).asBool();
        *bEnableCicadaCP = config["ESPSettings"]["Categories"]["Bugs"]["Cicada"].get("bEnableCicadaCP", false).asBool();
        *bEnableCicadaU = config["ESPSettings"]["Categories"]["Bugs"]["Cicada"].get("bEnableCicadaU", false).asBool();
        *bEnableCicadaUP = config["ESPSettings"]["Categories"]["Bugs"]["Cicada"].get("bEnableCicadaUP", false).asBool();
        *bEnableCicadaR = config["ESPSettings"]["Categories"]["Bugs"]["Cicada"].get("bEnableCicadaR", false).asBool();
        *bEnableCicadaRP = config["ESPSettings"]["Categories"]["Bugs"]["Cicada"].get("bEnableCicadaRP", false).asBool();

        *bEnableCrabC = config["ESPSettings"]["Categories"]["Bugs"]["Crab"].get("bEnableCrabC", false).asBool();
        *bEnableCrabCP = config["ESPSettings"]["Categories"]["Bugs"]["Crab"].get("bEnableCrabCP", false).asBool();
        *bEnableCrabU = config["ESPSettings"]["Categories"]["Bugs"]["Crab"].get("bEnableCrabU", false).asBool();
        *bEnableCrabUP = config["ESPSettings"]["Categories"]["Bugs"]["Crab"].get("bEnableCrabUP", false).asBool();
        *bEnableCrabR = config["ESPSettings"]["Categories"]["Bugs"]["Crab"].get("bEnableCrabR", false).asBool();
        *bEnableCrabRP = config["ESPSettings"]["Categories"]["Bugs"]["Crab"].get("bEnableCrabRP", false).asBool();

        *bEnableCricketC = config["ESPSettings"]["Categories"]["Bugs"]["Cricket"].get("bEnableCricketC", false).asBool();
        *bEnableCricketCP = config["ESPSettings"]["Categories"]["Bugs"]["Cricket"].get("bEnableCricketCP", false).asBool();
        *bEnableCricketU = config["ESPSettings"]["Categories"]["Bugs"]["Cricket"].get("bEnableCricketU", false).asBool();
        *bEnableCricketUP = config["ESPSettings"]["Categories"]["Bugs"]["Cricket"].get("bEnableCricketUP", false).asBool();
        *bEnableCricketR = config["ESPSettings"]["Categories"]["Bugs"]["Cricket"].get("bEnableCricketR", false).asBool();
        *bEnableCricketRP = config["ESPSettings"]["Categories"]["Bugs"]["Cricket"].get("bEnableCricketRP", false).asBool();

        *bEnableDragonflyC = config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"].get("bEnableDragonflyC", false).asBool();
        *bEnableDragonflyCP = config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"].get("bEnableDragonflyCP", false).asBool();
        *bEnableDragonflyU = config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"].get("bEnableDragonflyU", false).asBool();
        *bEnableDragonflyUP = config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"].get("bEnableDragonflyUP", false).asBool();
        *bEnableDragonflyR = config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"].get("bEnableDragonflyR", false).asBool();
        *bEnableDragonflyRP = config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"].get("bEnableDragonflyRP", false).asBool();
        *bEnableDragonflyE = config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"].get("bEnableDragonflyE", false).asBool();
        *bEnableDragonflyEP = config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"].get("bEnableDragonflyEP", false).asBool();

        *bEnableGlowbugC = config["ESPSettings"]["Categories"]["Bugs"]["Glowbug"].get("bEnableGlowbugC", false).asBool();
        *bEnableGlowbugCP = config["ESPSettings"]["Categories"]["Bugs"]["Glowbug"].get("bEnableGlowbugCP", false).asBool();
        *bEnableGlowbugU = config["ESPSettings"]["Categories"]["Bugs"]["Glowbug"].get("bEnableGlowbugU", false).asBool();
        *bEnableGlowbugUP = config["ESPSettings"]["Categories"]["Bugs"]["Glowbug"].get("bEnableGlowbugUP", false).asBool();

        *bEnableLadybugC = config["ESPSettings"]["Categories"]["Bugs"]["Ladybug"].get("bEnableLadybugC", false).asBool();
        *bEnableLadybugCP = config["ESPSettings"]["Categories"]["Bugs"]["Ladybug"].get("bEnableLadybugCP", false).asBool();
        *bEnableLadybugU = config["ESPSettings"]["Categories"]["Bugs"]["Ladybug"].get("bEnableLadybugU", false).asBool();
        *bEnableLadybugUP = config["ESPSettings"]["Categories"]["Bugs"]["Ladybug"].get("bEnableLadybugUP", false).asBool();

        *bEnableMantisU = config["ESPSettings"]["Categories"]["Bugs"]["Mantis"].get("bEnableMantisU", false).asBool();
        *bEnableMantisUP = config["ESPSettings"]["Categories"]["Bugs"]["Mantis"].get("bEnableMantisUP", false).asBool();
        *bEnableMantisR = config["ESPSettings"]["Categories"]["Bugs"]["Mantis"].get("bEnableMantisR", false).asBool();
        *bEnableMantisRP = config["ESPSettings"]["Categories"]["Bugs"]["Mantis"].get("bEnableMantisRP", false).asBool();
        *bEnableMantisR2 = config["ESPSettings"]["Categories"]["Bugs"]["Mantis"].get("bEnableMantisR2", false).asBool();
        *bEnableMantisR2P = config["ESPSettings"]["Categories"]["Bugs"]["Mantis"].get("bEnableMantisR2P", false).asBool();
        *bEnableMantisE = config["ESPSettings"]["Categories"]["Bugs"]["Mantis"].get("bEnableMantisE", false).asBool();
        *bEnableMantisEP = config["ESPSettings"]["Categories"]["Bugs"]["Mantis"].get("bEnableMantisEP", false).asBool();

        *bEnableMothC = config["ESPSettings"]["Categories"]["Bugs"]["Moth"].get("bEnableMothC", false).asBool();
        *bEnableMothCP = config["ESPSettings"]["Categories"]["Bugs"]["Moth"].get("bEnableMothCP", false).asBool();
        *bEnableMothU = config["ESPSettings"]["Categories"]["Bugs"]["Moth"].get("bEnableMothU", false).asBool();
        *bEnableMothUP = config["ESPSettings"]["Categories"]["Bugs"]["Moth"].get("bEnableMothUP", false).asBool();
        *bEnableMothR = config["ESPSettings"]["Categories"]["Bugs"]["Moth"].get("bEnableMothR", false).asBool();
        *bEnableMothRP = config["ESPSettings"]["Categories"]["Bugs"]["Moth"].get("bEnableMothRP", false).asBool();

        *bEnablePedeU = config["ESPSettings"]["Categories"]["Bugs"]["Centipede"].get("bEnablePedeU", false).asBool();
        *bEnablePedeUP = config["ESPSettings"]["Categories"]["Bugs"]["Centipede"].get("bEnablePedeUP", false).asBool();
        *bEnablePedeR = config["ESPSettings"]["Categories"]["Bugs"]["Centipede"].get("bEnablePedeR", false).asBool();
        *bEnablePedeRP = config["ESPSettings"]["Categories"]["Bugs"]["Centipede"].get("bEnablePedeRP", false).asBool();
        *bEnablePedeR2 = config["ESPSettings"]["Categories"]["Bugs"]["Centipede"].get("bEnablePedeR2", false).asBool();
        *bEnablePedeR2P = config["ESPSettings"]["Categories"]["Bugs"]["Centipede"].get("bEnablePedeR2P", false).asBool();

        *bEnableSnailU = config["ESPSettings"]["Categories"]["Bugs"]["Snail"].get("bEnableSnailU", false).asBool();
        *bEnableSnailUP = config["ESPSettings"]["Categories"]["Bugs"]["Snail"].get("bEnableSnailUP", false).asBool();
        *bEnableSnailR = config["ESPSettings"]["Categories"]["Bugs"]["Snail"].get("bEnableSnailR", false).asBool();
        *bEnableSnailRP = config["ESPSettings"]["Categories"]["Bugs"]["Snail"].get("bEnableSnailRP", false).asBool();
        // Bugs Colors
        {
            for (const auto& bugTypeName : config["ESPSettings"]["Categories"]["Bugs"].getMemberNames()) {
                EBugKind kind;

                if (bugTypeName == "Bee") kind = EBugKind::Bee;
                else if (bugTypeName == "Beetle") kind = EBugKind::Beetle;
                else if (bugTypeName == "Butterfly") kind = EBugKind::Butterfly;
                else if (bugTypeName == "Cicada") kind = EBugKind::Cicada;
                else if (bugTypeName == "Crab") kind = EBugKind::Crab;
                else if (bugTypeName == "Cricket") kind = EBugKind::Cricket;
                else if (bugTypeName == "Dragonfly") kind = EBugKind::Dragonfly;
                else if (bugTypeName == "Glowbug") kind = EBugKind::Glowbug;
                else if (bugTypeName == "Ladybug") kind = EBugKind::Ladybug;
                else if (bugTypeName == "Mantis") kind = EBugKind::Mantis;
                else if (bugTypeName == "Moth") kind = EBugKind::Moth;
                else if (bugTypeName == "Centipede") kind = EBugKind::Pede;
                else if (bugTypeName == "Snail") kind = EBugKind::Snail;
                else
                    continue;

                for (const auto& bugQuality : config["ESPSettings"]["Categories"]["Bugs"][bugTypeName].getMemberNames()) {
                    EBugQuality quality;
                    std::string qualityStr;

                    switch (kind) {
                    case EBugKind::Bee:
                        if (bugQuality == "bEnableBeeU" || bugQuality == "bEnableBeeUP") {
                            quality = EBugQuality::Uncommon;
                            qualityStr = "Uncommon";
                        }
                        else if (bugQuality == "bEnableBeeR" || bugQuality == "bEnableBeeRP") {
                            quality = EBugQuality::Rare;
                            qualityStr = "Rare";
                        }
                        break;
                    case EBugKind::Beetle:
                        if (bugQuality == "bEnableBeetleC" || bugQuality == "bEnableBeetleCP") {
                            quality = EBugQuality::Common;
                            qualityStr = "Common";
                        }
                        else if (bugQuality == "bEnableBeetleU" || bugQuality == "bEnableBeetleUP") {
                            quality = EBugQuality::Uncommon;
                            qualityStr = "Uncommon";
                        }
                        else if (bugQuality == "bEnableBeetleR" || bugQuality == "bEnableBeetleRP") {
                            quality = EBugQuality::Rare;
                            qualityStr = "Rare";
                        }
                        else if (bugQuality == "bEnableBeetleE" || bugQuality == "bEnableBeetleEP") {
                            quality = EBugQuality::Epic;
                            qualityStr = "Epic";
                        }
                        break;
                    case EBugKind::Butterfly:
                        if (bugQuality == "bEnableButterflyC" || bugQuality == "bEnableButterflyCP") {
                            quality = EBugQuality::Common;
                            qualityStr = "Common";
                        }
                        else if (bugQuality == "bEnableButterflyU" || bugQuality == "bEnableButterflyUP") {
                            quality = EBugQuality::Uncommon;
                            qualityStr = "Uncommon";
                        }
                        else if (bugQuality == "bEnableButterflyR" || bugQuality == "bEnableButterflyRP") {
                            quality = EBugQuality::Rare;
                            qualityStr = "Rare";
                        }
                        else if (bugQuality == "bEnableButterflyE" || bugQuality == "bEnableButterflyEP") {
                            quality = EBugQuality::Epic;
                            qualityStr = "Epic";
                        }
                        break;
                    case EBugKind::Cicada:
                        if (bugQuality == "bEnableCicadaC" || bugQuality == "bEnableCicadaCP") {
                            quality = EBugQuality::Common;
                            qualityStr = "Common";
                        }
                        else if (bugQuality == "bEnableCicadaU" || bugQuality == "bEnableCicadaUP") {
                            quality = EBugQuality::Uncommon;
                            qualityStr = "Uncommon";
                        }
                        else if (bugQuality == "bEnableCicadaR" || bugQuality == "bEnableCicadaRP") {
                            quality = EBugQuality::Rare;
                            qualityStr = "Rare";
                        }
                        break;
                    case EBugKind::Crab:
                        if (bugQuality == "bEnableCrabC" || bugQuality == "bEnableCrabCP") {
                            quality = EBugQuality::Common;
                            qualityStr = "Common";
                        }
                        else if (bugQuality == "bEnableCrabU" || bugQuality == "bEnableCrabUP") {
                            quality = EBugQuality::Uncommon;
                            qualityStr = "Uncommon";
                        }
                        else if (bugQuality == "bEnableCrabR" || bugQuality == "bEnableCrabRP") {
                            quality = EBugQuality::Rare;
                            qualityStr = "Rare";
                        }
                        break;
                    case EBugKind::Cricket:
                        if (bugQuality == "bEnableCricketC" || bugQuality == "bEnableCricketCP") {
                            quality = EBugQuality::Common;
                            qualityStr = "Common";
                        }
                        else if (bugQuality == "bEnableCricketU" || bugQuality == "bEnableCricketUP") {
                            quality = EBugQuality::Uncommon;
                            qualityStr = "Uncommon";
                        }
                        else if (bugQuality == "bEnableCricketR" || bugQuality == "bEnableCricketRP") {
                            quality = EBugQuality::Rare;
                            qualityStr = "Rare";
                        }
                        break;
                    case EBugKind::Dragonfly:
                        if (bugQuality == "bEnableDragonflyC" || bugQuality == "bEnableDragonflyCP") {
                            quality = EBugQuality::Common;
                            qualityStr = "Common";
                        }
                        else if (bugQuality == "bEnableDragonflyU" || bugQuality == "bEnableDragonflyUP") {
                            quality = EBugQuality::Uncommon;
                            qualityStr = "Uncommon";
                        }
                        else if (bugQuality == "bEnableDragonflyR" || bugQuality == "bEnableDragonflyRP") {
                            quality = EBugQuality::Rare;
                            qualityStr = "Rare";
                        }
                        else if (bugQuality == "bEnableDragonflyE" || bugQuality == "bEnableDragonflyEP") {
                            quality = EBugQuality::Epic;
                            qualityStr = "Epic";
                        }
                        break;
                    case EBugKind::Glowbug:
                        if (bugQuality == "bEnableGlowbugC" || bugQuality == "bEnableGlowbugCP") {
                            quality = EBugQuality::Common;
                            qualityStr = "Common";
                        }
                        else if (bugQuality == "bEnableGlowbugU" || bugQuality == "bEnableGlowbugUP") {
                            quality = EBugQuality::Uncommon;
                            qualityStr = "Uncommon";
                        }
                        break;
                    case EBugKind::Ladybug:
                        if (bugQuality == "bEnableLadybugC" || bugQuality == "bEnableLadybugCP") {
                            quality = EBugQuality::Common;
                            qualityStr = "Common";
                        }
                        else if (bugQuality == "bEnableLadybugU" || bugQuality == "bEnableLadybugUP") {
                            quality = EBugQuality::Uncommon;
                            qualityStr = "Uncommon";
                        }
                        break;
                    case EBugKind::Mantis:
                        if (bugQuality == "bEnableMantisU" || bugQuality == "bEnableMantisUP") {
                            quality = EBugQuality::Uncommon;
                            qualityStr = "Uncommon";
                        }
                        else if (bugQuality == "bEnableMantisR" || bugQuality == "bEnableMantisRP") {
                            quality = EBugQuality::Rare;
                            qualityStr = "Rare";
                        }
                        else if (bugQuality == "bEnableMantisR2" || bugQuality == "bEnableMantisR2P") {
                            quality = EBugQuality::Rare2;
                            qualityStr = "Rare2";
                        }
                        else if (bugQuality == "bEnableMantisE" || bugQuality == "bEnableMantisEP") {
                            quality = EBugQuality::Epic;
                            qualityStr = "Epic";
                        }
                        break;
                    case EBugKind::Moth:
                        if (bugQuality == "bEnableMothC" || bugQuality == "bEnableMothCP") {
                            quality = EBugQuality::Common;
                            qualityStr = "Common";
                        }
                        else if (bugQuality == "bEnableMothU" || bugQuality == "bEnableMothUP") {
                            quality = EBugQuality::Uncommon;
                            qualityStr = "Uncommon";
                        }
                        else if (bugQuality == "bEnableMothR" || bugQuality == "bEnableMothRP") {
                            quality = EBugQuality::Rare;
                            qualityStr = "Rare";
                        }
                        break;
                    case EBugKind::Pede:
                        if (bugQuality == "bEnablePedeU" || bugQuality == "bEnablePedeUP") {
                            quality = EBugQuality::Uncommon;
                            qualityStr = "Uncommon";
                        }
                        else if (bugQuality == "bEnablePedeR" || bugQuality == "bEnablePedeRP") {
                            quality = EBugQuality::Rare;
                            qualityStr = "Rare";
                        }
                        else if (bugQuality == "bEnablePedeR2" || bugQuality == "bEnablePedeR2P") {
                            quality = EBugQuality::Rare2;
                            qualityStr = "Rare2";
                        }
                        break;
                    case EBugKind::Snail:
                        if (bugQuality == "bEnableSnailU" || bugQuality == "bEnableSnailUP") {
                            quality = EBugQuality::Uncommon;
                            qualityStr = "Uncommon";
                        }
                        else if (bugQuality == "bEnableSnailR" || bugQuality == "bEnableSnailRP") {
                            quality = EBugQuality::Rare;
                            qualityStr = "Rare";
                        }
                        break;
                    default:
                        continue;
                    }

                    if (config["ESPSettings"]["Categories"]["Bugs"][bugTypeName]["Colors"].isMember(qualityStr)) {
                        ImU32 color = config["ESPSettings"]["Categories"]["Bugs"][bugTypeName]["Colors"][qualityStr].asUInt();
                        BugColors[{kind, quality}] = color;
                    }
                }
            }
        }

        // Trees
        *bEnableBushSm = config["ESPSettings"]["Categories"]["Trees"]["Bush"].get("bEnableBushSm", false).asBool();
        *bEnableSapwoodSm = config["ESPSettings"]["Categories"]["Trees"]["Sapwood"].get("bEnableSapwoodSm", false).asBool();
        *bEnableSapwoodMed = config["ESPSettings"]["Categories"]["Trees"]["Sapwood"].get("bEnableSapwoodMed", false).asBool();
        *bEnableSapwoodLg = config["ESPSettings"]["Categories"]["Trees"]["Sapwood"].get("bEnableSapwoodLg", false).asBool();
        *bEnableHeartwoodSm = config["ESPSettings"]["Categories"]["Trees"]["Heartwood"].get("bEnableHeartwoodSm", false).asBool();
        *bEnableHeartwoodMed = config["ESPSettings"]["Categories"]["Trees"]["Heartwood"].get("bEnableHeartwoodMed", false).asBool();
        *bEnableHeartwoodLg = config["ESPSettings"]["Categories"]["Trees"]["Heartwood"].get("bEnableHeartwoodLg", false).asBool();
        *bEnableFlowSm = config["ESPSettings"]["Categories"]["Trees"]["Flow"].get("bEnableFlowSm", false).asBool();
        *bEnableFlowMed = config["ESPSettings"]["Categories"]["Trees"]["Flow"].get("bEnableFlowMed", false).asBool();
        *bEnableFlowLg = config["ESPSettings"]["Categories"]["Trees"]["Flow"].get("bEnableFlowLg", false).asBool();
        // Trees Colors
        {
            for (const auto& treeTypeName : config["ESPSettings"]["Categories"]["Trees"].getMemberNames()) {
                ETreeType treeType;
                if (treeTypeName == "Bush") treeType = ETreeType::Bush;
                else if (treeTypeName == "Sapwood") treeType = ETreeType::Sapwood;
                else if (treeTypeName == "Heartwood") treeType = ETreeType::Heartwood;
                else if (treeTypeName == "Flow") treeType = ETreeType::Flow;
                else
                    continue;

                if (config["ESPSettings"]["Categories"]["Trees"][treeTypeName].isMember("Color")) {
                    ImU32 color = config["ESPSettings"]["Categories"]["Trees"][treeTypeName]["Color"].asUInt();
                    TreeColors[treeType] = color;
                }
            }
        }

        // Singles
        *bEnablePlayers = config["ESPSettings"]["Categories"]["Singles"]["Players"].get("bEnablePlayers", false).asBool();
        *bEnableNPC = config["ESPSettings"]["Categories"]["Singles"]["NPCs"].get("bEnableNPC", false).asBool();
        *bEnableFish = config["ESPSettings"]["Categories"]["Singles"]["Fish"].get("bEnableFish", false).asBool();
        *bEnablePools = config["ESPSettings"]["Categories"]["Singles"]["Pools"].get("bEnablePools", false).asBool();
        *bEnableLoot = config["ESPSettings"]["Categories"]["Singles"]["Loot"].get("bEnableLoot", false).asBool();
        *bEnableQuest = config["ESPSettings"]["Categories"]["Singles"]["Quest"].get("bEnableQuest", false).asBool();
        *bEnableRummagePiles = config["ESPSettings"]["Categories"]["Singles"]["Rummage Piles"].get("bEnableRummagePiles", false).asBool();
        *bEnableStables = config["ESPSettings"]["Categories"]["Singles"]["Stables"].get("bEnableStables", false).asBool();
        *bEnableTreasure = config["ESPSettings"]["Categories"]["Singles"]["Treasure"].get("bEnableTreasure", false).asBool();
        *bEnableTimedDrop = config["ESPSettings"]["Categories"]["Singles"]["Timed Drop"].get("bEnableTimedDrop", false).asBool();
        bEnableOthers = config["ESPSettings"]["Categories"]["Singles"]["Others"].get("bEnableOthers", false).asBool();
        // Singles Colors
        {
            for (const auto& singlesTypeName : config["ESPSettings"]["Categories"]["Singles"].getMemberNames()) {
                EOneOffs type;
                if (singlesTypeName == "Players") type = EOneOffs::Player;
                else if (singlesTypeName == "NPCs") type = EOneOffs::NPC;
                else if (singlesTypeName == "Loot") type = EOneOffs::Loot;
                else if (singlesTypeName == "Quest") type = EOneOffs::Quest;
                else if (singlesTypeName == "Rummage Piles") type = EOneOffs::RummagePiles;
                else if (singlesTypeName == "Treasure") type = EOneOffs::Treasure;
                else if (singlesTypeName == "Timed Drop") type = EOneOffs::TimedDrop;
                else if (singlesTypeName == "Others") type = EOneOffs::Others;
                else if (singlesTypeName == "Stables") type = EOneOffs::Stables;
                else
                    continue;

                if (config["ESPSettings"]["Categories"]["Singles"][singlesTypeName].isMember("Color")) {
                    ImU32 color = config["ESPSettings"]["Categories"]["Singles"][singlesTypeName]["Color"].asUInt();
                    SingleColors[type] = color;
                }
            }
            for (const auto& fishTypeName : config["ESPSettings"]["Categories"]["Singles"].getMemberNames()) {
                EFishType type;
                if (fishTypeName == "Fish") type = EFishType::Hook;
                else if (fishTypeName == "Pools") type = EFishType::Node;
                else
                    continue;

                if (config["ESPSettings"]["Categories"]["Singles"][fishTypeName].isMember("Color")) {
                    ImU32 color = config["ESPSettings"]["Categories"]["Singles"][fishTypeName]["Color"].asUInt();
                    FishColors[type] = color;
                }
            }
        }
    }
}

void Configuration::Load(PaliaOverlay* Overlay) {
    if (ConfigLoaded) return;

    CheckAndMigrateOldConfig();

    LoadESPArraysPtr(Overlay);

    std::ifstream aimConfigStream(aimConfigFilePath, std::ifstream::binary);
    LoadAimSettings(aimConfigStream);
    aimConfigStream.close();

    std::ifstream overlayConfigStream(overlayConfigFilePath, std::ifstream::binary);
    LoadOverlaySettings(overlayConfigStream);
    overlayConfigStream.close();

    std::ifstream modConfigStream(modConfigFilePath, std::ifstream::binary);
    LoadModSettings(modConfigStream);
    modConfigStream.close();

    std::ifstream movementConfigStream(movementConfigFilePath, std::ifstream::binary);
    LoadMovementSettings(movementConfigStream);
    movementConfigStream.close();

    std::ifstream espConfigStream(espConfigFilePath, std::ifstream::binary);
    LoadESPSettings(espConfigStream);
    espConfigStream.close();

    std::ifstream customThemeStream(customThemeConfigFilePath, std::ifstream::binary);
    LoadCustomThemeSettings(customThemeStream);
    customThemeStream.close();

    // Set ConfigLoaded to true after loading all the configurations
    ConfigLoaded = true;
}

void Configuration::LoadESPArraysPtr(PaliaOverlay* Overlay) {
    bEnablePlayers = &(HUDDetours::Singles[static_cast<int>(EOneOffs::Player)]);
    bEnableNPC = &(HUDDetours::Singles[static_cast<int>(EOneOffs::NPC)]);
    bEnableFish = &(HUDDetours::Fish[static_cast<int>(EFishType::Hook)]);
    bEnablePools = &(HUDDetours::Fish[static_cast<int>(EFishType::Node)]);
    bEnableLoot = &(HUDDetours::Singles[static_cast<int>(EOneOffs::Loot)]);
    bEnableQuest = &(HUDDetours::Singles[static_cast<int>(EOneOffs::Quest)]);
    bEnableRummagePiles = &(HUDDetours::Singles[static_cast<int>(EOneOffs::RummagePiles)]);
    bEnableStables = &(HUDDetours::Singles[static_cast<int>(EOneOffs::Stables)]);
    bEnableTreasure = &(HUDDetours::Singles[static_cast<int>(EOneOffs::Treasure)]);
    bEnableTimedDrop = &(HUDDetours::Singles[static_cast<int>(EOneOffs::TimedDrop)]);

    bEnableSernuk = &(HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)]);
    bEnableElderSernuk = &(HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier2)]);
    bEnableProudhornSernuk = &(HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier3)]);
    bEnableChapaa = &(HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)]);
    bEnableStripedChapaa = &(HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier2)]);
    bEnableAzureChapaa = &(HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier3)]);
    bEnableMinigameChapaa = &(HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Chase)]);
    bEnableMuujin = &(HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier1)]);
    bEnableBandedMuujin = &(HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier2)]);
    bEnableBluebristleMuujin = &(HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier3)]);

    bEnableClayLg = &(HUDDetours::Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(EGatherableSize::Large)]);
    bEnableStoneSm = &(HUDDetours::Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Small)]);
    bEnableStoneMed = &(HUDDetours::Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Medium)]);
    bEnableStoneLg = &(HUDDetours::Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Large)]);

    bEnableCopperSm = &(HUDDetours::Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Small)]);
    bEnableCopperMed = &(HUDDetours::Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Medium)]);
    bEnableCopperLg = &(HUDDetours::Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Large)]);

    bEnableIronSm = &(HUDDetours::Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Small)]);
    bEnableIronMed = &(HUDDetours::Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Medium)]);
    bEnableIronLg = &(HUDDetours::Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Large)]);

    bEnablePaliumSm = &(HUDDetours::Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Small)]);
    bEnablePaliumMed = &(HUDDetours::Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Medium)]);
    bEnablePaliumLg = &(HUDDetours::Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Large)]);

    bEnableCoral = &(HUDDetours::Forageables[static_cast<int>(EForageableType::Coral)][0]);
    bEnableOyster = &(HUDDetours::Forageables[static_cast<int>(EForageableType::Oyster)][0]);
    bEnableShell = &(HUDDetours::Forageables[static_cast<int>(EForageableType::Shell)][0]);
    bEnablePoisonFlower = &(HUDDetours::Forageables[static_cast<int>(EForageableType::PoisonFlower)][0]);
    bEnablePoisonFlowerP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::PoisonFlower)][1]);
    bEnableWaterFlower = &(HUDDetours::Forageables[static_cast<int>(EForageableType::WaterFlower)][0]);
    bEnableWaterFlowerP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::WaterFlower)][1]);
    bEnableHeartdrop = &(HUDDetours::Forageables[static_cast<int>(EForageableType::Heartdrop)][0]);
    bEnableHeartdropP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::Heartdrop)][1]);
    bEnableSundrop = &(HUDDetours::Forageables[static_cast<int>(EForageableType::Sundrop)][0]);
    bEnableSundropP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::Sundrop)][1]);
    bEnableDragonsBeard = &(HUDDetours::Forageables[static_cast<int>(EForageableType::DragonsBeard)][0]);
    bEnableDragonsBeardP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::DragonsBeard)][1]);
    bEnableEmeraldCarpet = &(HUDDetours::Forageables[static_cast<int>(EForageableType::EmeraldCarpet)][0]);
    bEnableEmeraldCarpetP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::EmeraldCarpet)][1]);
    bEnableMushroomBlue = &(HUDDetours::Forageables[static_cast<int>(EForageableType::MushroomBlue)][0]);
    bEnableMushroomBlueP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::MushroomBlue)][1]);
    bEnableMushroomRed = &(HUDDetours::Forageables[static_cast<int>(EForageableType::MushroomRed)][0]);
    bEnableMushroomRedP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::MushroomRed)][1]);
    bEnableDariCloves = &(HUDDetours::Forageables[static_cast<int>(EForageableType::DariCloves)][0]);
    bEnableDariClovesP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::DariCloves)][1]);
    bEnableHeatRoot = &(HUDDetours::Forageables[static_cast<int>(EForageableType::HeatRoot)][0]);
    bEnableHeatRootP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::HeatRoot)][1]);
    bEnableSpicedSprouts = &(HUDDetours::Forageables[static_cast<int>(EForageableType::SpicedSprouts)][0]);
    bEnableSpicedSproutsP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::SpicedSprouts)][1]);
    bEnableSweetLeaves = &(HUDDetours::Forageables[static_cast<int>(EForageableType::SweetLeaves)][0]);
    bEnableSweetLeavesP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::SweetLeaves)][1]);
    bEnableGarlic = &(HUDDetours::Forageables[static_cast<int>(EForageableType::Garlic)][0]);
    bEnableGarlicP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::Garlic)][1]);
    bEnableGinger = &(HUDDetours::Forageables[static_cast<int>(EForageableType::Ginger)][0]);
    bEnableGingerP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::Ginger)][1]);
    bEnableGreenOnion = &(HUDDetours::Forageables[static_cast<int>(EForageableType::GreenOnion)][0]);
    bEnableGreenOnionP = &(HUDDetours::Forageables[static_cast<int>(EForageableType::GreenOnion)][1]);

    bEnableBeeU = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableBeeUP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableBeeR = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableBeeRP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnableBeetleC = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableBeetleCP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableBeetleU = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableBeetleUP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableBeetleR = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableBeetleRP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnableBeetleE = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Epic)][0]);
    bEnableBeetleEP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Epic)][1]);
    bEnableButterflyC = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableButterflyCP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableButterflyU = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableButterflyUP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableButterflyR = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableButterflyRP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnableButterflyE = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Epic)][0]);
    bEnableButterflyEP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Epic)][1]);
    bEnableCicadaC = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableCicadaCP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableCicadaU = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableCicadaUP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableCicadaR = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableCicadaRP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnableCrabC = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableCrabCP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableCrabU = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableCrabUP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableCrabR = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableCrabRP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)][1]);

    bEnableCricketC = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableCricketCP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableCricketU = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableCricketUP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableCricketR = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableCricketRP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Rare)][1]);

    bEnableDragonflyC = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableDragonflyCP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableDragonflyU = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableDragonflyUP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableDragonflyR = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableDragonflyRP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnableDragonflyE = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Epic)][0]);
    bEnableDragonflyEP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Epic)][1]);

    bEnableGlowbugC = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableGlowbugCP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableGlowbugU = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableGlowbugUP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Uncommon)][1]);

    bEnableLadybugC = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableLadybugCP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableLadybugU = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableLadybugUP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Uncommon)][1]);

    bEnableMantisU = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableMantisUP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableMantisR = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableMantisRP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnableMantisR2 = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare2)][0]);
    bEnableMantisR2P = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare2)][1]);
    bEnableMantisE = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Epic)][0]);
    bEnableMantisEP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Epic)][1]);

    bEnableMothC = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableMothCP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableMothU = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableMothUP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableMothR = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableMothRP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)][1]);

    bEnablePedeU = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnablePedeUP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnablePedeR = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnablePedeRP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnablePedeR2 = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)][0]);
    bEnablePedeR2P = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)][1]);

    bEnableSnailU = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableSnailUP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableSnailR = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableSnailRP = &(HUDDetours::Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)][1]);

    bEnableBushSm = &(HUDDetours::Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Bush)]);

    bEnableSapwoodSm = &(HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Small)]);
    bEnableSapwoodMed = &(HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Medium)]);
    bEnableSapwoodLg = &(HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Large)]);

    bEnableHeartwoodSm = &(HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Small)]);
    bEnableHeartwoodMed = &(HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Medium)]);
    bEnableHeartwoodLg = &(HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Large)]);

    bEnableFlowSm = &(HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Small)]);
    bEnableFlowMed = &(HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Medium)]);
    bEnableFlowLg = &(HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Large)]);

    bEnableWheelSlot0 = &PrizeWheelSlots[0];
    bEnableWheelSlot1 = &PrizeWheelSlots[1];
    bEnableWheelSlot2 = &PrizeWheelSlots[2];
    bEnableWheelSlot3 = &PrizeWheelSlots[3];
    bEnableWheelSlot4 = &PrizeWheelSlots[4];
    bEnableWheelSlot5 = &PrizeWheelSlots[5];
    bEnableWheelSlot6 = &PrizeWheelSlots[6];
    bEnableWheelSlot7 = &PrizeWheelSlots[7];
    bEnableWheelSlot8 = &PrizeWheelSlots[8];
}

void Configuration::SaveCustomThemeSettings() {
    Json::Value config;

    config["Version"] = CustomThemeVersion;

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

void Configuration::SaveAimSettings() {
    Json::Value config;

    config["Version"] = AimVersion;

    // AIM SETTINGS
    config["AimSettings"]["Toggles"]["bEnableAimbot"] = bEnableAimbot;
    config["AimSettings"]["Toggles"]["bEnableSilentAimbot"] = bEnableSilentAimbot;

    config["AimSettings"]["FOV"]["Radius"]["FOVRadius"] = FOVRadius;
    config["AimSettings"]["FOV"]["Transparency"]["CircleAlpha"] = CircleAlpha;
    config["AimSettings"]["FOV"]["Toggles"]["bDrawFOVCircle"] = bDrawFOVCircle;
    config["AimSettings"]["FOV"]["Toggles"]["bHideFOVCircle"] = bHideFOVCircle;

    config["AimSettings"]["Settings"]["AvoidanceRadius"] = AvoidanceRadius;
    config["AimSettings"]["Settings"]["bDrawCrosshair"] = bDrawCrosshair;

    std::ofstream configFile(aimConfigFilePath);
    if (configFile.is_open()) {
        configFile << config.toStyledString();
        configFile.close();
    }
    else {
        std::cerr << "Failed to open Aim settings file for writing." << '\n';
    }
}

void Configuration::SaveOverlaySettings() {
    Json::Value config;

    config["Version"] = OverlayVersion;

    // OVERLAY SETTINGS
    config["OverlaySettings"]["MainWindow"]["windowSizeX"] = windowSizeX;
    config["OverlaySettings"]["MainWindow"]["windowSizeY"] = windowSizeY;
    config["OverlaySettings"]["MainWindow"]["selectedTheme"] = selectedTheme;

    config["OverlaySettings"]["ModMenu"]["Position"]["activeWindowPosX"] = activeWindowPosX;
    config["OverlaySettings"]["ModMenu"]["Position"]["activeWindowPosY"] = activeWindowPosY;
    config["OverlaySettings"]["ModMenu"]["Styling"]["activeModsOpacity"] = activeModsOpacity;
    config["OverlaySettings"]["ModMenu"]["Styling"]["activeModsRounding"] = activeModsRounding;
    config["OverlaySettings"]["ModMenu"]["Toggled"]["showEnabledFeaturesWindow"] = showEnabledFeaturesWindow;
    config["OverlaySettings"]["ModMenu"]["Settings"]["activeModsStickToRight"] = activeModsStickToRight;

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

void Configuration::SaveModSettings() {
    Json::Value config;

    config["Version"] = ModVersion;

    // UNCATEGORISED MODS SETTINGS
    

    config["Mods"]["Fishing"]["bFishingNoDurability"] = bFishingNoDurability;
    config["Mods"]["Fishing"]["bFishingMultiplayerHelp"] = bFishingMultiplayerHelp;
    config["Mods"]["Fishing"]["bFishingInstantCatch"] = bFishingInstantCatch;
    config["Mods"]["Fishing"]["bFishingPerfectCatch"] = bFishingPerfectCatch;
    config["Mods"]["Fishing"]["bFishingSell"] = bFishingSell;
    config["Mods"]["Fishing"]["bFishingDiscard"] = bFishingDiscard;
    config["Mods"]["Fishing"]["bFishingOpenStoreWaterlogged"] = bFishingOpenStoreWaterlogged;
    config["Mods"]["Fishing"]["bRequireClickFishing"] = bRequireClickFishing;

    config["Mods"]["Interaction Mods"]["bEnableInteractionMods"] = bEnableInteractionMods;
    config["Mods"]["Interaction Mods"]["InteractionRadius"] = InteractionRadius;

    config["Mods"]["PrizeWheel"]["Toggled"]["bEnablePrizeWheel"] = bEnablePrizeWheel;
    config["Mods"]["PrizeWheel"]["Settings"]["bPrizeWheelUseLocked"] = bPrizeWheelUseLocked;
    for (int i = 0; i < 9; ++i) {
        std::string key = "PrizeWheelSlot" + std::to_string(i);
        config["Mods"]["PrizeWheel"]["Settings"][key] = PrizeWheelSlots[i];
    }

#ifdef ENABLE_SUPPORTER_FEATURES
    config["Mods"]["Unlockables"]["bEnableFreeLandscape"] = bEnableFreeLandscape;
    config["Mods"]["Unlockables"]["bEnableOutfitCustomization"] = bEnableOutfitCustomization;

    config["Mods"]["Misc"]["bEnableAutoToolUse"] = bEnableAutoToolUse;
#endif
    config["Mods"]["Misc"]["bEnableRequestMods"] = bEnableRequestMods;
    config["Mods"]["Misc"]["bEnableAutoGather"] = bEnableAutoGather;
    config["Mods"]["Misc"]["bEasyModeActive"] = bEasyModeActive;
    config["Mods"]["Misc"]["bPlaceAnywhere"] = bPlaceAnywhere;
    config["Mods"]["Misc"]["bEnableAntiAfk"] = bEnableAntiAfk;
    config["Mods"]["Misc"]["bEnableUnlimitedWardrobeRespec"] = bEnableUnlimitedWardrobeRespec;
    config["Mods"]["Misc"]["bEnableMinigameSkip"] = bEnableMinigameSkip;

    std::ofstream configFile(modConfigFilePath);
    if (configFile.is_open()) {
        configFile << config.toStyledString();
        configFile.close();
    }
    else {
        std::cerr << "Failed to open Mod settings file for writing." << '\n';
    }
}

void Configuration::SaveMovementSettings() {
    Json::Value config;

    config["Version"] = MovementVersion;

    // MOVEMENT SETTINGS
    config["MovementSettings"]["Values"]["CustomWalkSpeed"] = CustomWalkSpeed;
    config["MovementSettings"]["Values"]["CustomSprintSpeedMultiplier"] = CustomSprintSpeedMultiplier;
    config["MovementSettings"]["Values"]["CustomClimbingSpeed"] = CustomClimbingSpeed;
    config["MovementSettings"]["Values"]["CustomGlidingSpeed"] = CustomGlidingSpeed;
    config["MovementSettings"]["Values"]["CustomGlidingFallSpeed"] = CustomGlidingFallSpeed;
    config["MovementSettings"]["Values"]["CustomJumpVelocity"] = CustomJumpVelocity;
    config["MovementSettings"]["Values"]["CustomMaxStepHeight"] = CustomMaxStepHeight;

    config["TeleportSettings"]["Toggles"]["bEnableWaypointTeleport"] = bEnableWaypointTeleport;
    config["TeleportSettings"]["Toggles"]["bTeleportToTargeted"] = bTeleportToTargeted;
    config["TeleportSettings"]["Toggles"]["bAvoidTeleportingToPlayers"] = bAvoidTeleportingToPlayers;
    config["TeleportSettings"]["Values"]["bDoRadiusPlayersAvoidance"] = bDoRadiusPlayersAvoidance;

    std::ofstream configFile(movementConfigFilePath);
    if (configFile.is_open()) {
        configFile << config.toStyledString();
        configFile.close();
    }
    else {
        std::cerr << "Failed to open Movement settings file for writing." << '\n';
    }
}

void Configuration::SaveESPSettings() {
    Json::Value config;

    config["Version"] = ESPVersion;

    // ESP SETTINGS
    config["ESPSettings"]["Toggle"]["bEnableESP"] = bEnableESP;
    {
        config["ESPSettings"]["Icons"]["bESPIcons"] = bESPIcons;
        config["ESPSettings"]["Icons"]["bESPIconDistance"] = bESPIconDistance;
        //config["ESPSettings"]["Culling"]["bEnableESPCulling"] = bEnableESPCulling;
        config["ESPSettings"]["Culling"]["CullDistance"] = CullDistance;
        //config["ESPSettings"]["Others"]["ESPTextScale"] = ESPTextScale; // DISABLED: ImGui Font-resizing not setup
    }

    // I beg theres an array for this.
    // Animals
    {
        config["ESPSettings"]["Categories"]["Animals"]["Sernuk"]["bEnableSernuk"] = *bEnableSernuk;
        config["ESPSettings"]["Categories"]["Animals"]["Sernuk"]["bEnableElderSernuk"] = *bEnableElderSernuk;
        config["ESPSettings"]["Categories"]["Animals"]["Sernuk"]["bEnableProudhornSernuk"] = *bEnableProudhornSernuk;
        config["ESPSettings"]["Categories"]["Animals"]["Chapaa"]["bEnableChapaa"] = *bEnableChapaa;
        config["ESPSettings"]["Categories"]["Animals"]["Chapaa"]["bEnableStripedChapaa"] = *bEnableStripedChapaa;
        config["ESPSettings"]["Categories"]["Animals"]["Chapaa"]["bEnableAzureChapaa"] = *bEnableAzureChapaa;
        config["ESPSettings"]["Categories"]["Animals"]["Chapaa"]["bEnableMinigameChapaa"] = *bEnableMinigameChapaa;
        config["ESPSettings"]["Categories"]["Animals"]["Muujin"]["bEnableMuujin"] = *bEnableMuujin;
        config["ESPSettings"]["Categories"]["Animals"]["Muujin"]["bEnableBandedMuujin"] = *bEnableBandedMuujin;
        config["ESPSettings"]["Categories"]["Animals"]["Muujin"]["bEnableBluebristleMuujin"] = *bEnableBluebristleMuujin;
        // Animal Colors
        {
            for (const auto& entry : Configuration::AnimalColors) {
                ECreatureKind kind = entry.first.Kind;
                ECreatureQuality quality = entry.first.Quality;
                ImU32 color = entry.second;

                std::string animalName;
                switch (kind) {
                case ECreatureKind::Cearnuk:
                    animalName = "Sernuk";
                    break;
                case ECreatureKind::Chapaa:
                    animalName = "Chapaa";
                    break;
                case ECreatureKind::TreeClimber:
                    animalName = "Muujin";
                    break;
                default:
                    continue;
                }

                std::string qualityStr;
                switch (quality) {
                case ECreatureQuality::Tier1:
                    if (kind == ECreatureKind::Cearnuk)
                        qualityStr = "Tier 1";
                    else if (kind == ECreatureKind::Chapaa)
                        qualityStr = "Tier 1";
                    else if (kind == ECreatureKind::TreeClimber)
                        qualityStr = "Tier 1";
                    break;
                case ECreatureQuality::Tier2:
                    if (kind == ECreatureKind::Cearnuk)
                        qualityStr = "Tier 2";
                    else if (kind == ECreatureKind::Chapaa)
                        qualityStr = "Tier 2";
                    else if (kind == ECreatureKind::TreeClimber)
                        qualityStr = "Tier 2";
                    break;
                case ECreatureQuality::Tier3:
                    if (kind == ECreatureKind::Cearnuk)
                        qualityStr = "Tier 3";
                    else if (kind == ECreatureKind::Chapaa)
                        qualityStr = "Tier 3";
                    else if (kind == ECreatureKind::TreeClimber)
                        qualityStr = "Tier 3";
                    break;
                case ECreatureQuality::Chase:
                    if (kind == ECreatureKind::Chapaa)
                        qualityStr = "Chase";
                    break;
                default:
                    continue;
                }

                if (!animalName.empty() || !qualityStr.empty())
                    config["ESPSettings"]["Categories"]["Animals"][animalName]["Colors"][qualityStr] = Json::UInt(color);
            }
        }
    }

    // Ores
    {
        config["ESPSettings"]["Categories"]["Ores"]["Clay"]["bEnableClayLg"] = *bEnableClayLg;
        config["ESPSettings"]["Categories"]["Ores"]["Stone"]["bEnableStoneSm"] = *bEnableStoneSm;
        config["ESPSettings"]["Categories"]["Ores"]["Stone"]["bEnableStoneMed"] = *bEnableStoneMed;
        config["ESPSettings"]["Categories"]["Ores"]["Stone"]["bEnableStoneLg"] = *bEnableStoneLg;
        config["ESPSettings"]["Categories"]["Ores"]["Copper"]["bEnableCopperSm"] = *bEnableCopperSm;
        config["ESPSettings"]["Categories"]["Ores"]["Copper"]["bEnableCopperMed"] = *bEnableCopperMed;
        config["ESPSettings"]["Categories"]["Ores"]["Copper"]["bEnableCopperLg"] = *bEnableCopperLg;
        config["ESPSettings"]["Categories"]["Ores"]["Iron"]["bEnableIronSm"] = *bEnableIronSm;
        config["ESPSettings"]["Categories"]["Ores"]["Iron"]["bEnableIronMed"] = *bEnableIronMed;
        config["ESPSettings"]["Categories"]["Ores"]["Iron"]["bEnableIronLg"] = *bEnableIronLg;
        config["ESPSettings"]["Categories"]["Ores"]["Palium"]["bEnablePaliumSm"] = *bEnablePaliumSm;
        config["ESPSettings"]["Categories"]["Ores"]["Palium"]["bEnablePaliumMed"] = *bEnablePaliumMed;
        config["ESPSettings"]["Categories"]["Ores"]["Palium"]["bEnablePaliumLg"] = *bEnablePaliumLg;

        // Ore Colors
        {
            for (const auto& [oreType, color] : OreColors) {
                std::string oreTypeName = "";
                switch (oreType) {
                case EOreType::Clay:
                    oreTypeName = "Clay";
                    break;
                case EOreType::Stone:
                    oreTypeName = "Stone";
                    break;
                case EOreType::Copper:
                    oreTypeName = "Copper";
                    break;
                case EOreType::Iron:
                    oreTypeName = "Iron";
                    break;
                case EOreType::Palium:
                    oreTypeName = "Palium";
                    break;
                default:
                    continue;
                }

                if (!oreTypeName.empty())
                    config["ESPSettings"]["Categories"]["Ores"][oreTypeName]["Color"] = Json::UInt(color);
            }
        }
    }

    // Forageables
    {
        config["ESPSettings"]["Categories"]["Forageables"]["Coral"]["bEnableCoral"] = *bEnableCoral;
        config["ESPSettings"]["Categories"]["Forageables"]["Oyster"]["bEnableOyster"] = *bEnableOyster;
        config["ESPSettings"]["Categories"]["Forageables"]["Shell"]["bEnableShell"] = *bEnableShell;
        config["ESPSettings"]["Categories"]["Forageables"]["Poison Flower"]["bEnablePoisonFlower"] = *bEnablePoisonFlower;
        config["ESPSettings"]["Categories"]["Forageables"]["Poison Flower"]["bEnablePoisonFlowerP"] = *bEnablePoisonFlowerP;
        config["ESPSettings"]["Categories"]["Forageables"]["Water Flower"]["bEnableWaterFlower"] = *bEnableWaterFlower;
        config["ESPSettings"]["Categories"]["Forageables"]["Water Flower"]["bEnableWaterFlowerP"] = *bEnableWaterFlowerP;
        config["ESPSettings"]["Categories"]["Forageables"]["Heartdrop"]["bEnableHeartdrop"] = *bEnableHeartdrop;
        config["ESPSettings"]["Categories"]["Forageables"]["Heartdrop"]["bEnableHeartdropP"] = *bEnableHeartdropP;
        config["ESPSettings"]["Categories"]["Forageables"]["Sundrop"]["bEnableSundrop"] = *bEnableSundrop;
        config["ESPSettings"]["Categories"]["Forageables"]["Sundrop"]["bEnableSundropP"] = *bEnableSundropP;
        config["ESPSettings"]["Categories"]["Forageables"]["Dragons Beard"]["bEnableDragonsBeard"] = *bEnableDragonsBeard;
        config["ESPSettings"]["Categories"]["Forageables"]["Dragons Beard"]["bEnableDragonsBeardP"] = *bEnableDragonsBeardP;
        config["ESPSettings"]["Categories"]["Forageables"]["Emerald Carpet"]["bEnableEmeraldCarpet"] = *bEnableEmeraldCarpet;
        config["ESPSettings"]["Categories"]["Forageables"]["Emerald Carpet"]["bEnableEmeraldCarpetP"] = *bEnableEmeraldCarpetP;
        config["ESPSettings"]["Categories"]["Forageables"]["Mushroom Blue"]["bEnableMushroomBlue"] = *bEnableMushroomBlue;
        config["ESPSettings"]["Categories"]["Forageables"]["Mushroom Blue"]["bEnableMushroomBlueP"] = *bEnableMushroomBlueP;
        config["ESPSettings"]["Categories"]["Forageables"]["Mushroom Red"]["bEnableMushroomRed"] = *bEnableMushroomRed;
        config["ESPSettings"]["Categories"]["Forageables"]["Mushroom Red"]["bEnableMushroomRedP"] = *bEnableMushroomRedP;
        config["ESPSettings"]["Categories"]["Forageables"]["Dari Cloves"]["bEnableDariCloves"] = *bEnableDariCloves;
        config["ESPSettings"]["Categories"]["Forageables"]["Dari Cloves"]["bEnableDariClovesP"] = *bEnableDariClovesP;
        config["ESPSettings"]["Categories"]["Forageables"]["Heat Root"]["bEnableHeatRoot"] = *bEnableHeatRoot;
        config["ESPSettings"]["Categories"]["Forageables"]["Heat Root"]["bEnableHeatRootP"] = *bEnableHeatRootP;
        config["ESPSettings"]["Categories"]["Forageables"]["Spiced Sprouts"]["bEnableSpicedSprouts"] = *bEnableSpicedSprouts;
        config["ESPSettings"]["Categories"]["Forageables"]["Spiced Sprouts"]["bEnableSpicedSproutsP"] = *bEnableSpicedSproutsP;
        config["ESPSettings"]["Categories"]["Forageables"]["Sweet Leaves"]["bEnableSweetLeaves"] = *bEnableSweetLeaves;
        config["ESPSettings"]["Categories"]["Forageables"]["Sweet Leaves"]["bEnableSweetLeavesP"] = *bEnableSweetLeavesP;
        config["ESPSettings"]["Categories"]["Forageables"]["Garlic"]["bEnableGarlic"] = *bEnableGarlic;
        config["ESPSettings"]["Categories"]["Forageables"]["Garlic"]["bEnableGarlicP"] = *bEnableGarlicP;
        config["ESPSettings"]["Categories"]["Forageables"]["Ginger"]["bEnableGinger"] = *bEnableGinger;
        config["ESPSettings"]["Categories"]["Forageables"]["Ginger"]["bEnableGingerP"] = *bEnableGingerP;
        config["ESPSettings"]["Categories"]["Forageables"]["Green Onion"]["bEnableGreenOnion"] = *bEnableGreenOnion;
        config["ESPSettings"]["Categories"]["Forageables"]["Green Onion"]["bEnableGreenOnionP"] = *bEnableGreenOnionP;
        // Forageable Colors
        {
            for (const auto& [forageable, color] : ForageableColors) {
                std::string forageableTypeName;
                switch (forageable) {
                case EForageableType::Coral:
                    forageableTypeName = "Coral";
                    break;
                case EForageableType::Oyster:
                    forageableTypeName = "Oyster";
                    break;
                case EForageableType::Shell:
                    forageableTypeName = "Shell";
                    break;
                case EForageableType::PoisonFlower:
                    forageableTypeName = "Poison Flower";
                    break;
                case EForageableType::WaterFlower:
                    forageableTypeName = "Water Flower";
                    break;
                case EForageableType::Heartdrop:
                    forageableTypeName = "Heartdrop";
                    break;
                case EForageableType::Sundrop:
                    forageableTypeName = "Sundrop";
                    break;
                case EForageableType::DragonsBeard:
                    forageableTypeName = "Dragons Beard";
                    break;
                case EForageableType::EmeraldCarpet:
                    forageableTypeName = "Emerald Carpet";
                    break;
                case EForageableType::MushroomBlue:
                    forageableTypeName = "Mushroom Blue";
                    break;
                case EForageableType::MushroomRed:
                    forageableTypeName = "Mushroom Red";
                    break;
                case EForageableType::DariCloves:
                    forageableTypeName = "Dari Cloves";
                    break;
                case EForageableType::HeatRoot:
                    forageableTypeName = "Heat Root";
                    break;
                case EForageableType::SpicedSprouts:
                    forageableTypeName = "Spiced Sprouts";
                    break;
                case EForageableType::SweetLeaves:
                    forageableTypeName = "Sweet Leaves";
                    break;
                case EForageableType::Garlic:
                    forageableTypeName = "Garlic";
                    break;
                case EForageableType::Ginger:
                    forageableTypeName = "Ginger";
                    break;
                case EForageableType::GreenOnion:
                    forageableTypeName = "Green Onion";
                    break;
                default:
                    continue;
                }

                if (!forageableTypeName.empty())
                    config["ESPSettings"]["Categories"]["Forageables"][forageableTypeName]["Color"] = Json::UInt(color);
            }
        }
    }

    // Bugs
    {
        config["ESPSettings"]["Categories"]["Bugs"]["Bee"]["bEnableBeeU"] = *bEnableBeeU;
        config["ESPSettings"]["Categories"]["Bugs"]["Bee"]["bEnableBeeUP"] = *bEnableBeeUP;
        config["ESPSettings"]["Categories"]["Bugs"]["Bee"]["bEnableBeeR"] = *bEnableBeeR;
        config["ESPSettings"]["Categories"]["Bugs"]["Bee"]["bEnableBeeRP"] = *bEnableBeeRP;

        config["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["bEnableBeetleC"] = *bEnableBeetleC;
        config["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["bEnableBeetleCP"] = *bEnableBeetleCP;
        config["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["bEnableBeetleU"] = *bEnableBeetleU;
        config["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["bEnableBeetleUP"] = *bEnableBeetleUP;
        config["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["bEnableBeetleR"] = *bEnableBeetleR;
        config["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["bEnableBeetleRP"] = *bEnableBeetleRP;
        config["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["bEnableBeetleE"] = *bEnableBeetleE;
        config["ESPSettings"]["Categories"]["Bugs"]["Beetle"]["bEnableBeetleEP"] = *bEnableBeetleEP;

        config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["bEnableButterflyC"] = *bEnableButterflyC;
        config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["bEnableButterflyCP"] = *bEnableButterflyCP;
        config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["bEnableButterflyU"] = *bEnableButterflyU;
        config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["bEnableButterflyUP"] = *bEnableButterflyUP;
        config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["bEnableButterflyR"] = *bEnableButterflyR;
        config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["bEnableButterflyRP"] = *bEnableButterflyRP;
        config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["bEnableButterflyE"] = *bEnableButterflyE;
        config["ESPSettings"]["Categories"]["Bugs"]["Butterfly"]["bEnableButterflyEP"] = *bEnableButterflyEP;

        config["ESPSettings"]["Categories"]["Bugs"]["Cicada"]["bEnableCicadaC"] = *bEnableCicadaC;
        config["ESPSettings"]["Categories"]["Bugs"]["Cicada"]["bEnableCicadaCP"] = *bEnableCicadaCP;
        config["ESPSettings"]["Categories"]["Bugs"]["Cicada"]["bEnableCicadaU"] = *bEnableCicadaU;
        config["ESPSettings"]["Categories"]["Bugs"]["Cicada"]["bEnableCicadaUP"] = *bEnableCicadaUP;
        config["ESPSettings"]["Categories"]["Bugs"]["Cicada"]["bEnableCicadaR"] = *bEnableCicadaR;
        config["ESPSettings"]["Categories"]["Bugs"]["Cicada"]["bEnableCicadaRP"] = *bEnableCicadaRP;

        config["ESPSettings"]["Categories"]["Bugs"]["Crab"]["bEnableCrabC"] = *bEnableCrabC;
        config["ESPSettings"]["Categories"]["Bugs"]["Crab"]["bEnableCrabCP"] = *bEnableCrabCP;
        config["ESPSettings"]["Categories"]["Bugs"]["Crab"]["bEnableCrabU"] = *bEnableCrabU;
        config["ESPSettings"]["Categories"]["Bugs"]["Crab"]["bEnableCrabUP"] = *bEnableCrabUP;
        config["ESPSettings"]["Categories"]["Bugs"]["Crab"]["bEnableCrabR"] = *bEnableCrabR;
        config["ESPSettings"]["Categories"]["Bugs"]["Crab"]["bEnableCrabRP"] = *bEnableCrabRP;

        config["ESPSettings"]["Categories"]["Bugs"]["Cricket"]["bEnableCricketC"] = *bEnableCricketC;
        config["ESPSettings"]["Categories"]["Bugs"]["Cricket"]["bEnableCricketCP"] = *bEnableCricketCP;
        config["ESPSettings"]["Categories"]["Bugs"]["Cricket"]["bEnableCricketU"] = *bEnableCricketU;
        config["ESPSettings"]["Categories"]["Bugs"]["Cricket"]["bEnableCricketUP"] = *bEnableCricketUP;
        config["ESPSettings"]["Categories"]["Bugs"]["Cricket"]["bEnableCricketR"] = *bEnableCricketR;
        config["ESPSettings"]["Categories"]["Bugs"]["Cricket"]["bEnableCricketRP"] = *bEnableCricketRP;

        config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["bEnableDragonflyC"] = *bEnableDragonflyC;
        config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["bEnableDragonflyCP"] = *bEnableDragonflyCP;
        config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["bEnableDragonflyU"] = *bEnableDragonflyU;
        config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["bEnableDragonflyUP"] = *bEnableDragonflyUP;
        config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["bEnableDragonflyR"] = *bEnableDragonflyR;
        config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["bEnableDragonflyRP"] = *bEnableDragonflyRP;
        config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["bEnableDragonflyE"] = *bEnableDragonflyE;
        config["ESPSettings"]["Categories"]["Bugs"]["Dragonfly"]["bEnableDragonflyEP"] = *bEnableDragonflyEP;

        config["ESPSettings"]["Categories"]["Bugs"]["Glowbug"]["bEnableGlowbugC"] = *bEnableGlowbugC;
        config["ESPSettings"]["Categories"]["Bugs"]["Glowbug"]["bEnableGlowbugCP"] = *bEnableGlowbugCP;
        config["ESPSettings"]["Categories"]["Bugs"]["Glowbug"]["bEnableGlowbugU"] = *bEnableGlowbugU;
        config["ESPSettings"]["Categories"]["Bugs"]["Glowbug"]["bEnableGlowbugUP"] = *bEnableGlowbugUP;

        config["ESPSettings"]["Categories"]["Bugs"]["Ladybug"]["bEnableLadybugC"] = *bEnableLadybugC;
        config["ESPSettings"]["Categories"]["Bugs"]["Ladybug"]["bEnableLadybugCP"] = *bEnableLadybugCP;
        config["ESPSettings"]["Categories"]["Bugs"]["Ladybug"]["bEnableLadybugU"] = *bEnableLadybugU;
        config["ESPSettings"]["Categories"]["Bugs"]["Ladybug"]["bEnableLadybugUP"] = *bEnableLadybugUP;

        config["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["bEnableMantisU"] = *bEnableMantisU;
        config["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["bEnableMantisUP"] = *bEnableMantisUP;
        config["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["bEnableMantisR"] = *bEnableMantisR;
        config["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["bEnableMantisRP"] = *bEnableMantisRP;
        config["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["bEnableMantisR2"] = *bEnableMantisR2;
        config["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["bEnableMantisR2P"] = *bEnableMantisR2P;
        config["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["bEnableMantisE"] = *bEnableMantisE;
        config["ESPSettings"]["Categories"]["Bugs"]["Mantis"]["bEnableMantisEP"] = *bEnableMantisEP;

        config["ESPSettings"]["Categories"]["Bugs"]["Moth"]["bEnableMothC"] = *bEnableMothC;
        config["ESPSettings"]["Categories"]["Bugs"]["Moth"]["bEnableMothCP"] = *bEnableMothCP;
        config["ESPSettings"]["Categories"]["Bugs"]["Moth"]["bEnableMothU"] = *bEnableMothU;
        config["ESPSettings"]["Categories"]["Bugs"]["Moth"]["bEnableMothUP"] = *bEnableMothUP;
        config["ESPSettings"]["Categories"]["Bugs"]["Moth"]["bEnableMothR"] = *bEnableMothR;
        config["ESPSettings"]["Categories"]["Bugs"]["Moth"]["bEnableMothRP"] = *bEnableMothRP;

        config["ESPSettings"]["Categories"]["Bugs"]["Centipede"]["bEnablePedeU"] = *bEnablePedeU;
        config["ESPSettings"]["Categories"]["Bugs"]["Centipede"]["bEnablePedeUP"] = *bEnablePedeUP;
        config["ESPSettings"]["Categories"]["Bugs"]["Centipede"]["bEnablePedeR"] = *bEnablePedeR;
        config["ESPSettings"]["Categories"]["Bugs"]["Centipede"]["bEnablePedeRP"] = *bEnablePedeRP;
        config["ESPSettings"]["Categories"]["Bugs"]["Centipede"]["bEnablePedeR2"] = *bEnablePedeR2;
        config["ESPSettings"]["Categories"]["Bugs"]["Centipede"]["bEnablePedeR2P"] = *bEnablePedeR2P;

        config["ESPSettings"]["Categories"]["Bugs"]["Snail"]["bEnableSnailU"] = *bEnableSnailU;
        config["ESPSettings"]["Categories"]["Bugs"]["Snail"]["bEnableSnailUP"] = *bEnableSnailUP;
        config["ESPSettings"]["Categories"]["Bugs"]["Snail"]["bEnableSnailR"] = *bEnableSnailR;
        config["ESPSettings"]["Categories"]["Bugs"]["Snail"]["bEnableSnailRP"] = *bEnableSnailRP;
        // Bugs Colors
        {
            for (const auto& entry : BugColors) {
                EBugKind kind = entry.first.Kind;
                EBugQuality quality = entry.first.Quality;
                ImU32 color = entry.second;

                std::string bugName;
                switch (kind) {
                case EBugKind::Bee:
                    bugName = "Bee";
                    break;
                case EBugKind::Beetle:
                    bugName = "Beetle";
                    break;
                case EBugKind::Butterfly:
                    bugName = "Butterfly";
                    break;
                case EBugKind::Cicada:
                    bugName = "Cicada";
                    break;
                case EBugKind::Crab:
                    bugName = "Crab";
                    break;
                case EBugKind::Cricket:
                    bugName = "Cricket";
                    break;
                case EBugKind::Dragonfly:
                    bugName = "Dragonfly";
                    break;
                case EBugKind::Glowbug:
                    bugName = "Glowbug";
                    break;
                case EBugKind::Ladybug:
                    bugName = "Ladybug";
                    break;
                case EBugKind::Mantis:
                    bugName = "Mantis";
                    break;
                case EBugKind::Moth:
                    bugName = "Moth";
                    break;
                case EBugKind::Pede:
                    bugName = "Centipede";
                    break;
                case EBugKind::Snail:
                    bugName = "Snail";
                    break;
                default:
                    continue;
                }

                std::string qualityStr;
                switch (quality) {
                case EBugQuality::Common:
                    if (kind == EBugKind::Beetle ||
                        kind == EBugKind::Butterfly ||
                        kind == EBugKind::Cicada ||
                        kind == EBugKind::Crab ||
                        kind == EBugKind::Cricket ||
                        kind == EBugKind::Dragonfly ||
                        kind == EBugKind::Glowbug ||
                        kind == EBugKind::Ladybug ||
                        kind == EBugKind::Moth)
                        qualityStr = "Common";
                    break;
                case EBugQuality::Uncommon:
                    if (kind == EBugKind::Bee ||
                        kind == EBugKind::Beetle ||
                        kind == EBugKind::Butterfly ||
                        kind == EBugKind::Cicada ||
                        kind == EBugKind::Crab ||
                        kind == EBugKind::Cricket ||
                        kind == EBugKind::Dragonfly ||
                        kind == EBugKind::Glowbug ||
                        kind == EBugKind::Ladybug ||
                        kind == EBugKind::Mantis ||
                        kind == EBugKind::Moth ||
                        kind == EBugKind::Pede ||
                        kind == EBugKind::Snail)
                        qualityStr = "Uncommon";
                    break;
                case EBugQuality::Rare:
                    if (kind == EBugKind::Bee ||
                        kind == EBugKind::Beetle ||
                        kind == EBugKind::Butterfly ||
                        kind == EBugKind::Cicada ||
                        kind == EBugKind::Crab ||
                        kind == EBugKind::Cricket ||
                        kind == EBugKind::Dragonfly ||
                        kind == EBugKind::Mantis ||
                        kind == EBugKind::Moth ||
                        kind == EBugKind::Pede ||
                        kind == EBugKind::Snail)
                        qualityStr = "Rare";
                    break;
                case EBugQuality::Rare2:
                    if (kind == EBugKind::Mantis || kind == EBugKind::Pede)
                        qualityStr = "Rare2";
                    break;
                case EBugQuality::Epic:
                    if (kind == EBugKind::Beetle ||
                        kind == EBugKind::Butterfly ||
                        kind == EBugKind::Dragonfly ||
                        kind == EBugKind::Mantis)
                        qualityStr = "Epic";
                    break;
                default:
                    continue;
                }

                if (!bugName.empty() || !qualityStr.empty())
                    config["ESPSettings"]["Categories"]["Bugs"][bugName]["Colors"][qualityStr] = Json::UInt(color);
            }
        }
    }

    // Trees
    {
        config["ESPSettings"]["Categories"]["Trees"]["Bush"]["bEnableBushSm"] = *bEnableBushSm;
        config["ESPSettings"]["Categories"]["Trees"]["Sapwood"]["bEnableSapwoodSm"] = *bEnableSapwoodSm;
        config["ESPSettings"]["Categories"]["Trees"]["Sapwood"]["bEnableSapwoodMed"] = *bEnableSapwoodMed;
        config["ESPSettings"]["Categories"]["Trees"]["Sapwood"]["bEnableSapwoodLg"] = *bEnableSapwoodLg;
        config["ESPSettings"]["Categories"]["Trees"]["Heartwood"]["bEnableHeartwoodSm"] = *bEnableHeartwoodSm;
        config["ESPSettings"]["Categories"]["Trees"]["Heartwood"]["bEnableHeartwoodMed"] = *bEnableHeartwoodMed;
        config["ESPSettings"]["Categories"]["Trees"]["Heartwood"]["bEnableHeartwoodLg"] = *bEnableHeartwoodLg;
        config["ESPSettings"]["Categories"]["Trees"]["Flow"]["bEnableFlowSm"] = *bEnableFlowSm;
        config["ESPSettings"]["Categories"]["Trees"]["Flow"]["bEnableFlowMed"] = *bEnableFlowMed;
        config["ESPSettings"]["Categories"]["Trees"]["Flow"]["bEnableFlowLg"] = *bEnableFlowLg;
        // Tree Colors
        {
            for (const auto& [treeType, color] : TreeColors) {
                std::string treeTypeName = "";
                switch (treeType) {
                case ETreeType::Bush:
                    treeTypeName = "Bush";
                    break;
                case ETreeType::Sapwood:
                    treeTypeName = "Sapwood";
                    break;
                case ETreeType::Heartwood:
                    treeTypeName = "Heartwood";
                    break;
                case ETreeType::Flow:
                    treeTypeName = "Flow";
                    break;
                default:
                    continue;
                }

                if (!treeTypeName.empty())
                    config["ESPSettings"]["Categories"]["Trees"][treeTypeName]["Color"] = Json::UInt(color);
            }
        }
    }

    // Singles
    {
        config["ESPSettings"]["Categories"]["Singles"]["Players"]["bEnablePlayers"] = *bEnablePlayers;
        config["ESPSettings"]["Categories"]["Singles"]["NPCs"]["bEnableNPC"] = *bEnableNPC;
        config["ESPSettings"]["Categories"]["Singles"]["Fish"]["bEnableFish"] = *bEnableFish;
        config["ESPSettings"]["Categories"]["Singles"]["Pools"]["bEnablePools"] = *bEnablePools;
        config["ESPSettings"]["Categories"]["Singles"]["Loot"]["bEnableLoot"] = *bEnableLoot;
        config["ESPSettings"]["Categories"]["Singles"]["Quest"]["bEnableQuest"] = *bEnableQuest;
        config["ESPSettings"]["Categories"]["Singles"]["Rummage Piles"]["bEnableRummagePiles"] = *bEnableRummagePiles;
        config["ESPSettings"]["Categories"]["Singles"]["Stables"]["bEnableStables"] = *bEnableStables;
        config["ESPSettings"]["Categories"]["Singles"]["Treasure"]["bEnableTreasure"] = *bEnableTreasure;
        config["ESPSettings"]["Categories"]["Singles"]["Timed Drop"]["bEnableTimedDrop"] = *bEnableTimedDrop;
        config["ESPSettings"]["Categories"]["Singles"]["Others"]["bEnableOthers"] = bEnableOthers;
        // Singles Colors
        {
            for (const auto& [singleType, color] : SingleColors) {
                std::string singleTypeName = "";
                switch (singleType) {
                case EOneOffs::Player:
                    singleTypeName = "Players";
                    break;
                case EOneOffs::NPC:
                    singleTypeName = "NPCs";
                    break;
                case EOneOffs::Loot:
                    singleTypeName = "Loot";
                    break;
                case EOneOffs::Quest:
                    singleTypeName = "Quest";
                    break;
                case EOneOffs::RummagePiles:
                    singleTypeName = "Rummage Piles";
                    break;
                case EOneOffs::Treasure:
                    singleTypeName = "Treasure";
                    break;
                case EOneOffs::TimedDrop:
                    singleTypeName = "Timed Drop";
                    break;
                case EOneOffs::Others:
                    singleTypeName = "Others";
                    break;
                case EOneOffs::Stables:
                    singleTypeName = "Stables";
                    break;
                default:
                    continue;
                }

                if (!singleTypeName.empty())
                    config["ESPSettings"]["Categories"]["Singles"][singleTypeName]["Color"] = Json::UInt(color);
            }
            for (const auto& [fishType, color] : FishColors) {
                std::string fishTypeName = "";
                switch (fishType) {
                case EFishType::Hook:
                    fishTypeName = "Fish";
                    break;
                case EFishType::Node:
                    fishTypeName = "Pools";
                    break;
                default:
                    continue;
                }

                if (!fishTypeName.empty())
                    config["ESPSettings"]["Categories"]["Singles"][fishTypeName]["Color"] = Json::UInt(color);
            }
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

void Configuration::Save() {
    EnsureDirectoryExists(configDirectory);

    SaveAimSettings();
    SaveOverlaySettings();
    SaveModSettings();
    SaveMovementSettings();
    SaveESPSettings();
    SaveCustomThemeSettings();
}