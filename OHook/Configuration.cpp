#include "Configuration.h"
#include "PaliaOverlay.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <type_traits>

namespace fs = std::filesystem;

#pragma region VarsInit

std::string Configuration::JsonConfigFile = "";
bool Configuration::ConfigLoaded = false;

// Config Version
// Will be used in the future for migrations
int Configuration::Version = 1;

// Crafting Cooking Booleans
// bool bEnableInstantCraftingCooking = false;

// Window configs
float Configuration::windowSizeX = 1450.0f;
float Configuration::windowSizeY = 950.0f;

//Aimbots
bool Configuration::bEnableAimbot = false;
bool Configuration::bEnableSilentAimbot = false;

// Tools
bool Configuration::bEnableAutoToolUse = false;

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
bool Configuration::bEnableVanishMode = false;

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
bool Configuration::bManualPositionAdjustment = false;

// Gardening
bool Configuration::bInfiniteWateringCan = false;

// Quicksell Hotkeys
bool Configuration::bEnableQuicksellHotkeys = false;

// Fun Mods
bool Configuration::bEnableAntiAfk = false;
bool Configuration::bEnableMinigameSkip = false;
bool Configuration::bEnableInteractionMods = false;

float Configuration::FOVRadius = 185.0f;
float Configuration::InteractionRadius = 500.0f;
int Configuration::AvoidanceRadius = 30;

#pragma region ESP

// ESP Booleans

bool Configuration::bShowWatermark = true;

bool Configuration::bEnableESP = true;
//bool Configuration::bESPIcons = false;
//bool Configuration::bESPIconDistance = false;
bool Configuration::bDrawFOVCircle = true;
bool Configuration::bTeleportToTargeted = true;
bool Configuration::bAvoidTeleportingToPlayers = true;
bool Configuration::bDoRadiusPlayersAvoidance = true;
bool Configuration::bEnableESPCulling = true;
bool Configuration::bEnableDespawnTimer = false;

// ESP Numericals

float Configuration::ESPTextScale = 1.0f;
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
bool Configuration::bEnableOthers = false;

#pragma endregion
#pragma endregion

// Path for the configuration file
static const std::string configDirectory = "C:\\ProgramData\\OriginPalia\\config";
static const std::string configFileName = "overlay_config.json";
static const std::string configFilePath = configDirectory + "\\" + configFileName;

// Functions for saving and loading config data
static void EnsureDirectoryExists(const std::string& path) {
    fs::path dir(path);
    if (!fs::exists(dir)) {
        fs::create_directories(dir);
    }
}

void Configuration::ParseString(std::string Key, std::string& Var) {
    if (JsonConfigFile.empty()) return;

    auto pos = JsonConfigFile.find("\"" + Key + "\":");
    if (pos == std::string::npos) return;

    std::size_t start = JsonConfigFile.find_first_of("\"", pos) + 1;
    std::size_t end = JsonConfigFile.find_first_of("\"", start);
    Var = JsonConfigFile.substr(start, end - start);
}

void Configuration::ParseBool(std::string Key, bool& Var) {
    if (JsonConfigFile.empty()) return;

    if (JsonConfigFile.find(Key) == std::string::npos) return;

    Var = (JsonConfigFile.find("\"" + Key + "\": true") != std::string::npos);
}

void Configuration::ParseBool(std::string Key, bool& Var, bool DefaultValue) {
    Var = DefaultValue;

    ParseBool(Key, Var);
}

template<typename T>
void Configuration::ParseNumber(std::string Key, T& Var) {
    static_assert(std::is_arithmetic<T>::value, "Template parameter must be a numeric type");

    if (JsonConfigFile.empty()) return;

    auto pos = JsonConfigFile.find("\"" + Key + "\":");
    if (pos == std::string::npos) return;

    std::size_t start = JsonConfigFile.find_first_of("0123456789.", pos);
    std::size_t end = JsonConfigFile.find_first_not_of("0123456789.", start);
    Var = std::stof(JsonConfigFile.substr(start, end - start));
}

bool Configuration::ReadJsonFile() {
    if (!fs::exists(configFilePath)) {
        return false; // No config file to load
    }

    std::ifstream configFile(configFilePath);
    std::string line;
    if (configFile.is_open()) {
        while (getline(configFile, line)) {
            JsonConfigFile += line;
        }
        configFile.close();
        return true;
    }

    return false;
}

void Configuration::Load(PaliaOverlay* Overlay) {
    if (ConfigLoaded) return;

    LoadESPArraysPtr(Overlay);

    if (!ReadJsonFile()) return;

    ParseBool("Enable Vanish Mode", bEnableVanishMode);
    ParseBool("Enable AutoTools", bEnableAutoToolUse);
    ParseBool("Enable Minigame Skip", bEnableMinigameSkip);
    ParseBool("Enable AntiAFK", bEnableAntiAfk);
    ParseBool("Enable Interaction Mods", bEnableInteractionMods);
    ParseNumber("Interaction Max Radius", InteractionRadius);
    ParseBool("Enable Silent Aimbot", bEnableSilentAimbot);
    ParseBool("Enable Legacy Aimbot", bEnableAimbot);
    ParseBool("Teleport to Targeted", bTeleportToTargeted);
    ParseBool("Teleport to Waypoint", bEnableWaypointTeleport);
    ParseBool("Avoid Teleporting To Targeted Players", bAvoidTeleportingToPlayers);
    ParseNumber("Avoidance Radius", AvoidanceRadius);
    ParseBool("Teleport Dropped Loot to Player", bEnableLootbagTeleportation);
    ParseBool("Enable AutoGather", bEnableAutoGather);
    ParseBool("Enable ESP", bEnableESP);
    //ParseBool("ESP Icons", bESPIcons);
    //ParseBool("ESP Icon Distance", bESPIconDistance);
    ParseBool("Show Watermark", bShowWatermark);
    ParseBool("Enable ESP Despawn Timer", bEnableDespawnTimer);
    //ParseBool("Limit Distance", bEnableESPCulling);
    ParseBool("Enable InteliAim Circle", bDrawFOVCircle);
    ParseBool("No Fishing Rod Durability", bFishingNoDurability);
    ParseBool("Enable Fishing Multiplayer Help", bFishingMultiplayerHelp);
    ParseBool("Enable Fishing Perfect Catch", bFishingPerfectCatch);
    ParseBool("Enable Fishing Instant Catch", bFishingInstantCatch);
    ParseBool("Enable Sell All Fish", bFishingSell);
    ParseBool("Enable Discarding Fishing Junk", bFishingDiscard);
    ParseBool("Fishing Open Store Waterlogged", bFishingOpenStoreWaterlogged);
    ParseBool("Require Left Click Fishing", bRequireClickFishing);
    ParseBool("Place Items Anywhere", bPlaceAnywhere);

    ParseBool("Enable Sernuk", *bEnableSernuk, false);
    ParseBool("Enable Elder Sernuk", *bEnableElderSernuk, false);
    ParseBool("Enable Proudhorn Sernuk", *bEnableProudhornSernuk, false);
    ParseBool("Enable Chapaa", *bEnableChapaa, false);
    ParseBool("Enable Striped Chapaa", *bEnableStripedChapaa, false);
    ParseBool("Enable Azure Chapaa", *bEnableAzureChapaa, false);
    ParseBool("Enable Minigame Chapaa", *bEnableMinigameChapaa, false);
    ParseBool("Enable Muujin", *bEnableMuujin, false);
    ParseBool("Enable Banded Muujin", *bEnableBandedMuujin, false);
    ParseBool("Enable Bluebristle Muujin", *bEnableBluebristleMuujin, false);
    ParseBool("Enable Clay Lg", *bEnableClayLg, false);
    ParseBool("Enable Stone Sm", *bEnableStoneSm, false);
    ParseBool("Enable Stone Med", *bEnableStoneMed, false);
    ParseBool("Enable Stone Lg", *bEnableStoneLg, false);
    ParseBool("Enable Copper Sm", *bEnableCopperSm, false);
    ParseBool("Enable Copper Med", *bEnableCopperMed, false);
    ParseBool("Enable Copper Lg", *bEnableCopperLg, false);
    ParseBool("Enable Iron Sm", *bEnableIronSm, false);
    ParseBool("Enable Iron Med", *bEnableIronMed, false);
    ParseBool("Enable Iron Lg", *bEnableIronLg, false);
    ParseBool("Enable Palium Sm", *bEnablePaliumSm, false);
    ParseBool("Enable Palium Med", *bEnablePaliumMed, false);
    ParseBool("Enable Palium Lg", *bEnablePaliumLg, false);
    ParseBool("Enable Coral", *bEnableCoral, false);
    ParseBool("Enable Oyster", *bEnableOyster, false);
    ParseBool("Enable Shell", *bEnableShell, false);
    ParseBool("Enable Poison Flower", *bEnablePoisonFlower, false);
    ParseBool("Enable Poison Flower P", *bEnablePoisonFlowerP, false);
    ParseBool("Enable Water Flower", *bEnableWaterFlower, false);
    ParseBool("Enable Water Flower P", *bEnableWaterFlowerP, false);
    ParseBool("Enable Heartdrop", *bEnableHeartdrop, false);
    ParseBool("Enable Heartdrop P", *bEnableHeartdropP, false);
    ParseBool("Enable Sundrop", *bEnableSundrop, false);
    ParseBool("Enable Sundrop P", *bEnableSundropP, false);
    ParseBool("Enable Dragons Beard", *bEnableDragonsBeard, false);
    ParseBool("Enable Dragons Beard P", *bEnableDragonsBeardP, false);
    ParseBool("Enable Emerald Carpet", *bEnableEmeraldCarpet, false);
    ParseBool("Enable Emerald Carpet P", *bEnableEmeraldCarpetP, false);
    ParseBool("Enable Mushroom Blue", *bEnableMushroomBlue, false);
    ParseBool("Enable Mushroom Blue P", *bEnableMushroomBlueP, false);
    ParseBool("Enable Mushroom Red", *bEnableMushroomRed, false);
    ParseBool("Enable Mushroom Red P", *bEnableMushroomRedP, false);
    ParseBool("Enable Dari Cloves", *bEnableDariCloves, false);
    ParseBool("Enable Dari Cloves P", *bEnableDariClovesP, false);
    ParseBool("Enable Heat Root", *bEnableHeatRoot, false);
    ParseBool("Enable Heat Root P", *bEnableHeatRootP, false);
    ParseBool("Enable Spiced Sprouts", *bEnableSpicedSprouts, false);
    ParseBool("Enable Spiced Sprouts P", *bEnableSpicedSproutsP, false);
    ParseBool("Enable Sweet Leaves", *bEnableSweetLeaves, false);
    ParseBool("Enable Sweet Leaves P", *bEnableSweetLeavesP, false);
    ParseBool("Enable Garlic", *bEnableGarlic, false);
    ParseBool("Enable Garlic P", *bEnableGarlicP, false);
    ParseBool("Enable Ginger", *bEnableGinger, false);
    ParseBool("Enable Ginger P", *bEnableGingerP, false);
    ParseBool("Enable Green Onion", *bEnableGreenOnion, false);
    ParseBool("Enable Green Onion P", *bEnableGreenOnionP, false);
    ParseBool("Enable Bee U", *bEnableBeeU, false);
    ParseBool("Enable Bee U P", *bEnableBeeUP, false);
    ParseBool("Enable Bee R", *bEnableBeeR, false);
    ParseBool("Enable Bee R P", *bEnableBeeRP, false);
    ParseBool("Enable Beetle C", *bEnableBeetleC, false);
    ParseBool("Enable Beetle C P", *bEnableBeetleCP, false);
    ParseBool("Enable Beetle U", *bEnableBeetleU, false);
    ParseBool("Enable Beetle U P", *bEnableBeetleUP, false);
    ParseBool("Enable Beetle R", *bEnableBeetleR, false);
    ParseBool("Enable Beetle R P", *bEnableBeetleRP, false);
    ParseBool("Enable Beetle E", *bEnableBeetleE, false);
    ParseBool("Enable Beetle E P", *bEnableBeetleEP, false);
    ParseBool("Enable Butterfly C", *bEnableButterflyC, false);
    ParseBool("Enable Butterfly C P", *bEnableButterflyCP, false);
    ParseBool("Enable Butterfly U", *bEnableButterflyU, false);
    ParseBool("Enable Butterfly U P", *bEnableButterflyUP, false);
    ParseBool("Enable Butterfly R", *bEnableButterflyR, false);
    ParseBool("Enable Butterfly R P", *bEnableButterflyRP, false);
    ParseBool("Enable Butterfly E", *bEnableButterflyE, false);
    ParseBool("Enable Butterfly E P", *bEnableButterflyEP, false);
    ParseBool("Enable Cicada C", *bEnableCicadaC, false);
    ParseBool("Enable Cicada C P", *bEnableCicadaCP, false);
    ParseBool("Enable Cicada U", *bEnableCicadaU, false);
    ParseBool("Enable Cicada U P", *bEnableCicadaUP, false);
    ParseBool("Enable Cicada R", *bEnableCicadaR, false);
    ParseBool("Enable Cicada R P", *bEnableCicadaRP, false);
    ParseBool("Enable Crab C", *bEnableCrabC, false);
    ParseBool("Enable Crab C P", *bEnableCrabCP, false);
    ParseBool("Enable Crab U", *bEnableCrabU, false);
    ParseBool("Enable Crab U P", *bEnableCrabUP, false);
    ParseBool("Enable Crab R", *bEnableCrabR, false);
    ParseBool("Enable Crab R P", *bEnableCrabRP, false);
    ParseBool("Enable Cricket C", *bEnableCricketC, false);
    ParseBool("Enable Cricket C P", *bEnableCricketCP, false);
    ParseBool("Enable Cricket U", *bEnableCricketU, false);
    ParseBool("Enable Cricket U P", *bEnableCricketUP, false);
    ParseBool("Enable Cricket R", *bEnableCricketR, false);
    ParseBool("Enable Cricket R P", *bEnableCricketRP, false);
    ParseBool("Enable Dragonfly C", *bEnableDragonflyC, false);
    ParseBool("Enable Dragonfly C P", *bEnableDragonflyCP, false);
    ParseBool("Enable Dragonfly U", *bEnableDragonflyU, false);
    ParseBool("Enable Dragonfly U P", *bEnableDragonflyUP, false);
    ParseBool("Enable Dragonfly R", *bEnableDragonflyR, false);
    ParseBool("Enable Dragonfly R P", *bEnableDragonflyRP, false);
    ParseBool("Enable Dragonfly E", *bEnableDragonflyE, false);
    ParseBool("Enable Dragonfly E P", *bEnableDragonflyEP, false);
    ParseBool("Enable Glowbug C", *bEnableGlowbugC, false);
    ParseBool("Enable Glowbug C P", *bEnableGlowbugCP, false);
    ParseBool("Enable Glowbug U", *bEnableGlowbugU, false);
    ParseBool("Enable Glowbug U P", *bEnableGlowbugUP, false);
    ParseBool("Enable Ladybug C", *bEnableLadybugC, false);
    ParseBool("Enable Ladybug C P", *bEnableLadybugCP, false);
    ParseBool("Enable Ladybug U", *bEnableLadybugU, false);
    ParseBool("Enable Ladybug U P", *bEnableLadybugUP, false);
    ParseBool("Enable Mantis U", *bEnableMantisU, false);
    ParseBool("Enable Mantis U P", *bEnableMantisUP, false);
    ParseBool("Enable Mantis R", *bEnableMantisR, false);
    ParseBool("Enable Mantis R P", *bEnableMantisRP, false);
    ParseBool("Enable Mantis R2", *bEnableMantisR2, false);
    ParseBool("Enable Mantis R2 P", *bEnableMantisR2P, false);
    ParseBool("Enable Mantis E", *bEnableMantisE, false);
    ParseBool("Enable Mantis E P", *bEnableMantisEP, false);
    ParseBool("Enable Moth C", *bEnableMothC, false);
    ParseBool("Enable Moth C P", *bEnableMothCP, false);
    ParseBool("Enable Moth U", *bEnableMothU, false);
    ParseBool("Enable Moth U P", *bEnableMothUP, false);
    ParseBool("Enable Moth R", *bEnableMothR, false);
    ParseBool("Enable Moth R P", *bEnableMothRP, false);
    ParseBool("Enable Pede U", *bEnablePedeU, false);
    ParseBool("Enable Pede U P", *bEnablePedeUP, false);
    ParseBool("Enable Pede R", *bEnablePedeR, false);
    ParseBool("Enable Pede R P", *bEnablePedeRP, false);
    ParseBool("Enable Pede R2", *bEnablePedeR2, false);
    ParseBool("Enable Pede R2 P", *bEnablePedeR2P, false);
    ParseBool("Enable Snail U", *bEnableSnailU, false);
    ParseBool("Enable Snail U P", *bEnableSnailUP, false);
    ParseBool("Enable Snail R", *bEnableSnailR, false);
    ParseBool("Enable Snail R P", *bEnableSnailRP, false);
    ParseBool("Enable Bush Sm", *bEnableBushSm, false);
    ParseBool("Enable Sapwood Sm", *bEnableSapwoodSm, false);
    ParseBool("Enable Sapwood Med", *bEnableSapwoodMed, false);
    ParseBool("Enable Sapwood Lg", *bEnableSapwoodLg, false);
    ParseBool("Enable Heartwood Sm", *bEnableHeartwoodSm, false);
    ParseBool("Enable Heartwood Med", *bEnableHeartwoodMed, false);
    ParseBool("Enable Heartwood Lg", *bEnableHeartwoodLg, false);
    ParseBool("Enable Flow Sm", *bEnableFlowSm, false);
    ParseBool("Enable Flow Med", *bEnableFlowMed, false);
    ParseBool("Enable Flow Lg", *bEnableFlowLg, false);
    ParseBool("Enable Players", *bEnablePlayers, false);
    ParseBool("Enable NPC", *bEnableNPC, false);
    ParseBool("Enable Fish", *bEnableFish, false);
    ParseBool("Enable Pools", *bEnablePools, false);
    ParseBool("Enable Loot", *bEnableLoot, false);
    ParseBool("Enable Quest", *bEnableQuest, false);
    ParseBool("Enable Rummage Piles", *bEnableRummagePiles, false);
    ParseBool("Enable Stables", *bEnableStables, false);
    ParseBool("Enable Treasure", *bEnableTreasure, false);
    ParseBool("Enable Others", bEnableOthers);

    ParseNumber("Distance", CullDistance);
    ParseNumber("ESP Text Scale", ESPTextScale);
    ParseNumber("InteliAim Radius", FOVRadius);

    ParseNumber("Custom Walk Speed", CustomWalkSpeed);
    ParseNumber("Custom Sprint Speed Multiplier", CustomSprintSpeedMultiplier);
    ParseNumber("Custom Climbing Speed", CustomClimbingSpeed);
    ParseNumber("Custom Gliding Speed", CustomGlidingSpeed);
    ParseNumber("Custom Gliding Fall Speed", CustomGlidingFallSpeed);
    ParseNumber("Custom Jump Velocity", CustomJumpVelocity);
    ParseNumber("Custom Max Step Height", CustomMaxStepHeight);

    ParseNumber("Menu Size X", windowSizeX);
    ParseNumber("Menu Size Y", windowSizeY);

    ConfigLoaded = true;
}

void Configuration::LoadESPArraysPtr(PaliaOverlay* Overlay) {
    bEnablePlayers = &(Overlay->Singles[static_cast<int>(EOneOffs::Player)]);
    bEnableNPC = &(Overlay->Singles[static_cast<int>(EOneOffs::NPC)]);
    bEnableFish = &(Overlay->Fish[static_cast<int>(EFishType::Hook)]);
    bEnablePools = &(Overlay->Fish[static_cast<int>(EFishType::Node)]);
    bEnableLoot = &(Overlay->Singles[static_cast<int>(EOneOffs::Loot)]);
    bEnableQuest = &(Overlay->Singles[static_cast<int>(EOneOffs::Quest)]);
    bEnableRummagePiles = &(Overlay->Singles[static_cast<int>(EOneOffs::RummagePiles)]);
    bEnableStables = &(Overlay->Singles[static_cast<int>(EOneOffs::Stables)]);
    bEnableTreasure = &(Overlay->Singles[static_cast<int>(EOneOffs::Treasure)]);

    bEnableSernuk = &(Overlay->Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)]);
    bEnableElderSernuk = &(Overlay->Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier2)]);
    bEnableProudhornSernuk = &(Overlay->Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier3)]);
    bEnableChapaa = &(Overlay->Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)]);
    bEnableStripedChapaa = &(Overlay->Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier2)]);
    bEnableAzureChapaa = &(Overlay->Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier3)]);
    bEnableMinigameChapaa = &(Overlay->Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Chase)]);
    bEnableMuujin = &(Overlay->Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier1)]);
    bEnableBandedMuujin = &(Overlay->Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier2)]);
    bEnableBluebristleMuujin = &(Overlay->Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier3)]);

    bEnableClayLg = &(Overlay->Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(EGatherableSize::Large)]);
    bEnableStoneSm = &(Overlay->Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Small)]);
    bEnableStoneMed = &(Overlay->Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Medium)]);
    bEnableStoneLg = &(Overlay->Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Large)]);

    bEnableCopperSm = &(Overlay->Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Small)]);
    bEnableCopperMed = &(Overlay->Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Medium)]);
    bEnableCopperLg = &(Overlay->Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Large)]);

    bEnableIronSm = &(Overlay->Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Small)]);
    bEnableIronMed = &(Overlay->Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Medium)]);
    bEnableIronLg = &(Overlay->Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Large)]);

    bEnablePaliumSm = &(Overlay->Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Small)]);
    bEnablePaliumMed = &(Overlay->Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Medium)]);
    bEnablePaliumLg = &(Overlay->Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Large)]);

    bEnableCoral = &(Overlay->Forageables[static_cast<int>(EForageableType::Coral)][0]);
    bEnableOyster = &(Overlay->Forageables[static_cast<int>(EForageableType::Oyster)][0]);
    bEnableShell = &(Overlay->Forageables[static_cast<int>(EForageableType::Shell)][0]);
    bEnablePoisonFlower = &(Overlay->Forageables[static_cast<int>(EForageableType::PoisonFlower)][0]);
    bEnablePoisonFlowerP = &(Overlay->Forageables[static_cast<int>(EForageableType::PoisonFlower)][1]);
    bEnableWaterFlower = &(Overlay->Forageables[static_cast<int>(EForageableType::WaterFlower)][0]);
    bEnableWaterFlowerP = &(Overlay->Forageables[static_cast<int>(EForageableType::WaterFlower)][1]);
    bEnableHeartdrop = &(Overlay->Forageables[static_cast<int>(EForageableType::Heartdrop)][0]);
    bEnableHeartdropP = &(Overlay->Forageables[static_cast<int>(EForageableType::Heartdrop)][1]);
    bEnableSundrop = &(Overlay->Forageables[static_cast<int>(EForageableType::Sundrop)][0]);
    bEnableSundropP = &(Overlay->Forageables[static_cast<int>(EForageableType::Sundrop)][1]);
    bEnableDragonsBeard = &(Overlay->Forageables[static_cast<int>(EForageableType::DragonsBeard)][0]);
    bEnableDragonsBeardP = &(Overlay->Forageables[static_cast<int>(EForageableType::DragonsBeard)][1]);
    bEnableEmeraldCarpet = &(Overlay->Forageables[static_cast<int>(EForageableType::EmeraldCarpet)][0]);
    bEnableEmeraldCarpetP = &(Overlay->Forageables[static_cast<int>(EForageableType::EmeraldCarpet)][1]);
    bEnableMushroomBlue = &(Overlay->Forageables[static_cast<int>(EForageableType::MushroomBlue)][0]);
    bEnableMushroomBlueP = &(Overlay->Forageables[static_cast<int>(EForageableType::MushroomBlue)][1]);
    bEnableMushroomRed = &(Overlay->Forageables[static_cast<int>(EForageableType::MushroomRed)][0]);
    bEnableMushroomRedP = &(Overlay->Forageables[static_cast<int>(EForageableType::MushroomRed)][1]);
    bEnableDariCloves = &(Overlay->Forageables[static_cast<int>(EForageableType::DariCloves)][0]);
    bEnableDariClovesP = &(Overlay->Forageables[static_cast<int>(EForageableType::DariCloves)][1]);
    bEnableHeatRoot = &(Overlay->Forageables[static_cast<int>(EForageableType::HeatRoot)][0]);
    bEnableHeatRootP = &(Overlay->Forageables[static_cast<int>(EForageableType::HeatRoot)][1]);
    bEnableSpicedSprouts = &(Overlay->Forageables[static_cast<int>(EForageableType::SpicedSprouts)][0]);
    bEnableSpicedSproutsP = &(Overlay->Forageables[static_cast<int>(EForageableType::SpicedSprouts)][1]);
    bEnableSweetLeaves = &(Overlay->Forageables[static_cast<int>(EForageableType::SweetLeaves)][0]);
    bEnableSweetLeavesP = &(Overlay->Forageables[static_cast<int>(EForageableType::SweetLeaves)][1]);
    bEnableGarlic = &(Overlay->Forageables[static_cast<int>(EForageableType::Garlic)][0]);
    bEnableGarlicP = &(Overlay->Forageables[static_cast<int>(EForageableType::Garlic)][1]);
    bEnableGinger = &(Overlay->Forageables[static_cast<int>(EForageableType::Ginger)][0]);
    bEnableGingerP = &(Overlay->Forageables[static_cast<int>(EForageableType::Ginger)][1]);
    bEnableGreenOnion = &(Overlay->Forageables[static_cast<int>(EForageableType::GreenOnion)][0]);
    bEnableGreenOnionP = &(Overlay->Forageables[static_cast<int>(EForageableType::GreenOnion)][1]);

    bEnableBeeU = &(Overlay->Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableBeeUP = &(Overlay->Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableBeeR = &(Overlay->Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableBeeRP = &(Overlay->Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnableBeetleC = &(Overlay->Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableBeetleCP = &(Overlay->Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableBeetleU = &(Overlay->Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableBeetleUP = &(Overlay->Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableBeetleR = &(Overlay->Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableBeetleRP = &(Overlay->Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnableBeetleE = &(Overlay->Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Epic)][0]);
    bEnableBeetleEP = &(Overlay->Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Epic)][1]);
    bEnableButterflyC = &(Overlay->Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableButterflyCP = &(Overlay->Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableButterflyU = &(Overlay->Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableButterflyUP = &(Overlay->Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableButterflyR = &(Overlay->Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableButterflyRP = &(Overlay->Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnableButterflyE = &(Overlay->Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Epic)][0]);
    bEnableButterflyEP = &(Overlay->Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Epic)][1]);
    bEnableCicadaC = &(Overlay->Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableCicadaCP = &(Overlay->Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableCicadaU = &(Overlay->Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableCicadaUP = &(Overlay->Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableCicadaR = &(Overlay->Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableCicadaRP = &(Overlay->Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnableCrabC = &(Overlay->Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableCrabCP = &(Overlay->Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableCrabU = &(Overlay->Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableCrabUP = &(Overlay->Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableCrabR = &(Overlay->Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableCrabRP = &(Overlay->Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)][1]);

    bEnableCricketC = &(Overlay->Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableCricketCP = &(Overlay->Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableCricketU = &(Overlay->Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableCricketUP = &(Overlay->Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableCricketR = &(Overlay->Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableCricketRP = &(Overlay->Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Rare)][1]);

    bEnableDragonflyC = &(Overlay->Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableDragonflyCP = &(Overlay->Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableDragonflyU = &(Overlay->Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableDragonflyUP = &(Overlay->Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableDragonflyR = &(Overlay->Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableDragonflyRP = &(Overlay->Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnableDragonflyE = &(Overlay->Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Epic)][0]);
    bEnableDragonflyEP = &(Overlay->Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Epic)][1]);

    bEnableGlowbugC = &(Overlay->Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableGlowbugCP = &(Overlay->Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableGlowbugU = &(Overlay->Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableGlowbugUP = &(Overlay->Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Uncommon)][1]);

    bEnableLadybugC = &(Overlay->Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableLadybugCP = &(Overlay->Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableLadybugU = &(Overlay->Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableLadybugUP = &(Overlay->Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Uncommon)][1]);

    bEnableMantisU = &(Overlay->Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableMantisUP = &(Overlay->Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableMantisR = &(Overlay->Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableMantisRP = &(Overlay->Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnableMantisR2 = &(Overlay->Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare2)][0]);
    bEnableMantisR2P = &(Overlay->Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare2)][1]);
    bEnableMantisE = &(Overlay->Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Epic)][0]);
    bEnableMantisEP = &(Overlay->Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Epic)][1]);

    bEnableMothC = &(Overlay->Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)][0]);
    bEnableMothCP = &(Overlay->Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)][1]);
    bEnableMothU = &(Overlay->Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableMothUP = &(Overlay->Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableMothR = &(Overlay->Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableMothRP = &(Overlay->Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)][1]);

    bEnablePedeU = &(Overlay->Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnablePedeUP = &(Overlay->Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnablePedeR = &(Overlay->Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnablePedeRP = &(Overlay->Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)][1]);
    bEnablePedeR2 = &(Overlay->Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)][0]);
    bEnablePedeR2P = &(Overlay->Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)][1]);

    bEnableSnailU = &(Overlay->Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)][0]);
    bEnableSnailUP = &(Overlay->Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)][1]);
    bEnableSnailR = &(Overlay->Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)][0]);
    bEnableSnailRP = &(Overlay->Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)][1]);

    bEnableBushSm = &(Overlay->Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Bush)]);

    bEnableSapwoodSm = &(Overlay->Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Small)]);
    bEnableSapwoodMed = &(Overlay->Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Medium)]);
    bEnableSapwoodLg = &(Overlay->Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Large)]);

    bEnableHeartwoodSm = &(Overlay->Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Small)]);
    bEnableHeartwoodMed = &(Overlay->Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Medium)]);
    bEnableHeartwoodLg = &(Overlay->Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Large)]);

    bEnableFlowSm = &(Overlay->Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Small)]);
    bEnableFlowMed = &(Overlay->Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Medium)]);
    bEnableFlowLg = &(Overlay->Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Large)]);
}

void Configuration::Save() {
    EnsureDirectoryExists(configDirectory);

    std::ofstream configFile(configFilePath);
    if (configFile.is_open()) {
        configFile << "{\n";
        configFile << "    \"Version\": " << Version << ",\n";
        configFile << "    \"Enable Vanish Mode\": " << (bEnableVanishMode ? "true" : "false") << ",\n";
        configFile << "    \"Enable AutoTools\": " << (bEnableAutoToolUse ? "true" : "false") << ",\n";
        configFile << "    \"Enable Minigame Skip\": " << (bEnableMinigameSkip ? "true" : "false") << ",\n";
        configFile << "    \"Enable AntiAFK\": " << (bEnableAntiAfk ? "true" : "false") << ",\n";
        configFile << "    \"Enable Interaction Mods\": " << (bEnableInteractionMods ? "true" : "false") << ",\n";
        configFile << "    \"Interaction Max Radius\": " << InteractionRadius << ",\n";
        configFile << "    \"Enable ESP\": " << (bEnableESP ? "true" : "false") << ",\n";
        //configFile << "    \"ESP Icons\": " << (bESPIcons ? "true" : "false") << ",\n";
        //configFile << "    \"ESP Icon Distance\": " << (bESPIconDistance ? "true" : "false") << ",\n";
        configFile << "    \"Show Watermark\": " << (bShowWatermark ? "true" : "false") << ",\n";
        configFile << "    \"Enable ESP Despawn Timer\": " << (bEnableDespawnTimer ? "true" : "false") << ",\n";
        configFile << "    \"ESP Text Scale\": " << ESPTextScale << ",\n";
        //configFile << "    \"Limit Distance\": " << (bEnableESPCulling ? "true" : "false") << ",\n";
        configFile << "    \"Distance\": " << CullDistance << ",\n";
        configFile << "    \"Enable InteliAim Circle\": " << (bDrawFOVCircle ? "true" : "false") << ",\n";
        configFile << "    \"InteliAim Radius\": " << FOVRadius << ",\n";
        configFile << "    \"Enable Silent Aimbot\": " << (bEnableSilentAimbot ? "true" : "false") << ",\n";
        configFile << "    \"Enable Legacy Aimbot\": " << (bEnableAimbot ? "true" : "false") << ",\n";
        configFile << "    \"Teleport to Targeted\": " << (bTeleportToTargeted ? "true" : "false") << ",\n";
        configFile << "    \"Teleport to Waypoint\": " << (bEnableWaypointTeleport ? "true" : "false") << ",\n";
        configFile << "    \"Avoid Teleporting To Targeted Players\": " << (bAvoidTeleportingToPlayers ? "true" : "false") << ",\n";
        configFile << "    \"Avoidance Radius\": " << AvoidanceRadius << ",\n";
        configFile << "    \"Teleport Dropped Loot to Player\": " << (bEnableLootbagTeleportation ? "true" : "false") << ",\n";
        configFile << "    \"Enable AutoGather\": " << (bEnableAutoGather ? "true" : "false") << ",\n";
        configFile << "    \"No Fishing Rod Durability\": " << (bFishingNoDurability ? "true" : "false") << ",\n";
        configFile << "    \"Enable Fishing Multiplayer Help\": " << (bFishingMultiplayerHelp ? "true" : "false") << ",\n";
        configFile << "    \"Enable Fishing Perfect Catch\": " << (bFishingPerfectCatch ? "true" : "false") << ",\n";
        configFile << "    \"Enable Fishing Instant Catch\": " << (bFishingInstantCatch ? "true" : "false") << ",\n";
        configFile << "    \"Enable Sell All Fish\": " << (bFishingSell ? "true" : "false") << ",\n";
        configFile << "    \"Enable Discarding Fishing Junk\": " << (bFishingDiscard ? "true" : "false") << ",\n";
        configFile << "    \"Fishing Open Store Waterlogged\": " << (bFishingOpenStoreWaterlogged ? "true" : "false") << ",\n";
        configFile << "    \"Require Left Click Fishing\": " << (bRequireClickFishing ? "true" : "false") << ",\n";
        configFile << "    \"Custom Walk Speed\": " << CustomWalkSpeed << ",\n";
        configFile << "    \"Custom Sprint Speed Multiplier\": " << CustomSprintSpeedMultiplier << ",\n";
        configFile << "    \"Custom Climbing Speed\": " << CustomClimbingSpeed << ",\n";
        configFile << "    \"Custom Gliding Speed\": " << CustomGlidingSpeed << ",\n";
        configFile << "    \"Custom Gliding Fall Speed\": " << CustomGlidingFallSpeed << ",\n";
        configFile << "    \"Custom Jump Velocity\": " << CustomJumpVelocity << ",\n";
        configFile << "    \"Custom Max Step Height\": " << CustomMaxStepHeight << ",\n";
        configFile << "    \"Place Items Anywhere\": " << (bPlaceAnywhere ? "true" : "false") << ",\n";
        configFile << "    \"Enable Sernuk\": " << (*bEnableSernuk ? "true" : "false") << ",\n";
        configFile << "    \"Enable Elder Sernuk\": " << (*bEnableElderSernuk ? "true" : "false") << ",\n";
        configFile << "    \"Enable Proudhorn Sernuk\": " << (*bEnableProudhornSernuk ? "true" : "false") << ",\n";
        configFile << "    \"Enable Chapaa\": " << (*bEnableChapaa ? "true" : "false") << ",\n";
        configFile << "    \"Enable Striped Chapaa\": " << (*bEnableStripedChapaa ? "true" : "false") << ",\n";
        configFile << "    \"Enable Azure Chapaa\": " << (*bEnableAzureChapaa ? "true" : "false") << ",\n";
        configFile << "    \"Enable Minigame Chapaa\": " << (*bEnableMinigameChapaa ? "true" : "false") << ",\n";
        configFile << "    \"Enable Muujin\": " << (*bEnableMuujin ? "true" : "false") << ",\n";
        configFile << "    \"Enable Banded Muujin\": " << (*bEnableBandedMuujin ? "true" : "false") << ",\n";
        configFile << "    \"Enable Bluebristle Muujin\": " << (*bEnableBluebristleMuujin ? "true" : "false") << ",\n";
        configFile << "    \"Enable Clay Lg\": " << (*bEnableClayLg ? "true" : "false") << ",\n";
        configFile << "    \"Enable Stone Sm\": " << (*bEnableStoneSm ? "true" : "false") << ",\n";
        configFile << "    \"Enable Stone Med\": " << (*bEnableStoneMed ? "true" : "false") << ",\n";
        configFile << "    \"Enable Stone Lg\": " << (*bEnableStoneLg ? "true" : "false") << ",\n";
        configFile << "    \"Enable Copper Sm\": " << (*bEnableCopperSm ? "true" : "false") << ",\n";
        configFile << "    \"Enable Copper Med\": " << (*bEnableCopperMed ? "true" : "false") << ",\n";
        configFile << "    \"Enable Copper Lg\": " << (*bEnableCopperLg ? "true" : "false") << ",\n";
        configFile << "    \"Enable Iron Sm\": " << (*bEnableIronSm ? "true" : "false") << ",\n";
        configFile << "    \"Enable Iron Med\": " << (*bEnableIronMed ? "true" : "false") << ",\n";
        configFile << "    \"Enable Iron Lg\": " << (*bEnableIronLg ? "true" : "false") << ",\n";
        configFile << "    \"Enable Palium Sm\": " << (*bEnablePaliumSm ? "true" : "false") << ",\n";
        configFile << "    \"Enable Palium Med\": " << (*bEnablePaliumMed ? "true" : "false") << ",\n";
        configFile << "    \"Enable Palium Lg\": " << (*bEnablePaliumLg ? "true" : "false") << ",\n";
        configFile << "    \"Enable Coral\": " << (*bEnableCoral ? "true" : "false") << ",\n";
        configFile << "    \"Enable Oyster\": " << (*bEnableOyster ? "true" : "false") << ",\n";
        configFile << "    \"Enable Shell\": " << (*bEnableShell ? "true" : "false") << ",\n";
        configFile << "    \"Enable Poison Flower\": " << (*bEnablePoisonFlower ? "true" : "false") << ",\n";
        configFile << "    \"Enable Poison Flower P\": " << (*bEnablePoisonFlowerP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Water Flower\": " << (*bEnableWaterFlower ? "true" : "false") << ",\n";
        configFile << "    \"Enable Water Flower P\": " << (*bEnableWaterFlowerP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Heartdrop\": " << (*bEnableHeartdrop ? "true" : "false") << ",\n";
        configFile << "    \"Enable Heartdrop P\": " << (*bEnableHeartdropP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Sundrop\": " << (*bEnableSundrop ? "true" : "false") << ",\n";
        configFile << "    \"Enable Sundrop P\": " << (*bEnableSundropP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Dragons Beard\": " << (*bEnableDragonsBeard ? "true" : "false") << ",\n";
        configFile << "    \"Enable Dragons Beard P\": " << (*bEnableDragonsBeardP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Emerald Carpet\": " << (*bEnableEmeraldCarpet ? "true" : "false") << ",\n";
        configFile << "    \"Enable Emerald Carpet P\": " << (*bEnableEmeraldCarpetP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Mushroom Blue\": " << (*bEnableMushroomBlue ? "true" : "false") << ",\n";
        configFile << "    \"Enable Mushroom Blue P\": " << (*bEnableMushroomBlueP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Mushroom Red\": " << (*bEnableMushroomRed ? "true" : "false") << ",\n";
        configFile << "    \"Enable Mushroom Red P\": " << (*bEnableMushroomRedP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Dari Cloves\": " << (*bEnableDariCloves ? "true" : "false") << ",\n";
        configFile << "    \"Enable Dari Cloves P\": " << (*bEnableDariClovesP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Heat Root\": " << (*bEnableHeatRoot ? "true" : "false") << ",\n";
        configFile << "    \"Enable Heat Root P\": " << (*bEnableHeatRootP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Spiced Sprouts\": " << (*bEnableSpicedSprouts ? "true" : "false") << ",\n";
        configFile << "    \"Enable Spiced Sprouts P\": " << (*bEnableSpicedSproutsP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Sweet Leaves\": " << (*bEnableSweetLeaves ? "true" : "false") << ",\n";
        configFile << "    \"Enable Sweet Leaves P\": " << (*bEnableSweetLeavesP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Garlic\": " << (*bEnableGarlic ? "true" : "false") << ",\n";
        configFile << "    \"Enable Garlic P\": " << (*bEnableGarlicP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Ginger\": " << (*bEnableGinger ? "true" : "false") << ",\n";
        configFile << "    \"Enable Ginger P\": " << (*bEnableGingerP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Green Onion\": " << (*bEnableGreenOnion ? "true" : "false") << ",\n";
        configFile << "    \"Enable Green Onion P\": " << (*bEnableGreenOnionP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Bee U\": " << (*bEnableBeeU ? "true" : "false") << ",\n";
        configFile << "    \"Enable Bee U P\": " << (*bEnableBeeUP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Bee R\": " << (*bEnableBeeR ? "true" : "false") << ",\n";
        configFile << "    \"Enable Bee R P\": " << (*bEnableBeeRP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Beetle C\": " << (*bEnableBeetleC ? "true" : "false") << ",\n";
        configFile << "    \"Enable Beetle C P\": " << (*bEnableBeetleCP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Beetle U\": " << (*bEnableBeetleU ? "true" : "false") << ",\n";
        configFile << "    \"Enable Beetle U P\": " << (*bEnableBeetleUP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Beetle R\": " << (*bEnableBeetleR ? "true" : "false") << ",\n";
        configFile << "    \"Enable Beetle R P\": " << (*bEnableBeetleRP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Beetle E\": " << (*bEnableBeetleE ? "true" : "false") << ",\n";
        configFile << "    \"Enable Beetle E P\": " << (*bEnableBeetleEP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Butterfly C\": " << (*bEnableButterflyC ? "true" : "false") << ",\n";
        configFile << "    \"Enable Butterfly C P\": " << (*bEnableButterflyCP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Butterfly U\": " << (*bEnableButterflyU ? "true" : "false") << ",\n";
        configFile << "    \"Enable Butterfly U P\": " << (*bEnableButterflyUP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Butterfly R\": " << (*bEnableButterflyR ? "true" : "false") << ",\n";
        configFile << "    \"Enable Butterfly R P\": " << (*bEnableButterflyRP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Butterfly E\": " << (*bEnableButterflyE ? "true" : "false") << ",\n";
        configFile << "    \"Enable Butterfly E P\": " << (*bEnableButterflyEP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Cicada C\": " << (*bEnableCicadaC ? "true" : "false") << ",\n";
        configFile << "    \"Enable Cicada C P\": " << (*bEnableCicadaCP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Cicada U\": " << (*bEnableCicadaU ? "true" : "false") << ",\n";
        configFile << "    \"Enable Cicada U P\": " << (*bEnableCicadaUP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Cicada R\": " << (*bEnableCicadaR ? "true" : "false") << ",\n";
        configFile << "    \"Enable Cicada R P\": " << (*bEnableCicadaRP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Crab C\": " << (*bEnableCrabC ? "true" : "false") << ",\n";
        configFile << "    \"Enable Crab C P\": " << (*bEnableCrabCP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Crab U\": " << (*bEnableCrabU ? "true" : "false") << ",\n";
        configFile << "    \"Enable Crab U P\": " << (*bEnableCrabUP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Crab R\": " << (*bEnableCrabR ? "true" : "false") << ",\n";
        configFile << "    \"Enable Crab R P\": " << (*bEnableCrabRP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Cricket C\": " << (*bEnableCricketC ? "true" : "false") << ",\n";
        configFile << "    \"Enable Cricket C P\": " << (*bEnableCricketCP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Cricket U\": " << (*bEnableCricketU ? "true" : "false") << ",\n";
        configFile << "    \"Enable Cricket U P\": " << (*bEnableCricketUP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Cricket R\": " << (*bEnableCricketR ? "true" : "false") << ",\n";
        configFile << "    \"Enable Cricket R P\": " << (*bEnableCricketRP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Dragonfly C\": " << (*bEnableDragonflyC ? "true" : "false") << ",\n";
        configFile << "    \"Enable Dragonfly C P\": " << (*bEnableDragonflyCP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Dragonfly U\": " << (*bEnableDragonflyU ? "true" : "false") << ",\n";
        configFile << "    \"Enable Dragonfly U P\": " << (*bEnableDragonflyUP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Dragonfly R\": " << (*bEnableDragonflyR ? "true" : "false") << ",\n";
        configFile << "    \"Enable Dragonfly R P\": " << (*bEnableDragonflyRP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Dragonfly E\": " << (*bEnableDragonflyE ? "true" : "false") << ",\n";
        configFile << "    \"Enable Dragonfly E P\": " << (*bEnableDragonflyEP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Glowbug C\": " << (*bEnableGlowbugC ? "true" : "false") << ",\n";
        configFile << "    \"Enable Glowbug C P\": " << (*bEnableGlowbugCP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Glowbug U\": " << (*bEnableGlowbugU ? "true" : "false") << ",\n";
        configFile << "    \"Enable Glowbug U P\": " << (*bEnableGlowbugUP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Ladybug C\": " << (*bEnableLadybugC ? "true" : "false") << ",\n";
        configFile << "    \"Enable Ladybug C P\": " << (*bEnableLadybugCP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Ladybug U\": " << (*bEnableLadybugU ? "true" : "false") << ",\n";
        configFile << "    \"Enable Ladybug U P\": " << (*bEnableLadybugUP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Mantis U\": " << (*bEnableMantisU ? "true" : "false") << ",\n";
        configFile << "    \"Enable Mantis U P\": " << (*bEnableMantisUP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Mantis R\": " << (*bEnableMantisR ? "true" : "false") << ",\n";
        configFile << "    \"Enable Mantis R P\": " << (*bEnableMantisRP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Mantis R2\": " << (*bEnableMantisR2 ? "true" : "false") << ",\n";
        configFile << "    \"Enable Mantis R2 P\": " << (*bEnableMantisR2P ? "true" : "false") << ",\n";
        configFile << "    \"Enable Mantis E\": " << (*bEnableMantisE ? "true" : "false") << ",\n";
        configFile << "    \"Enable Mantis E P\": " << (*bEnableMantisEP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Moth C\": " << (*bEnableMothC ? "true" : "false") << ",\n";
        configFile << "    \"Enable Moth C P\": " << (*bEnableMothCP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Moth U\": " << (*bEnableMothU ? "true" : "false") << ",\n";
        configFile << "    \"Enable Moth U P\": " << (*bEnableMothUP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Moth R\": " << (*bEnableMothR ? "true" : "false") << ",\n";
        configFile << "    \"Enable Moth R P\": " << (*bEnableMothRP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Pede U\": " << (*bEnablePedeU ? "true" : "false") << ",\n";
        configFile << "    \"Enable Pede U P\": " << (*bEnablePedeUP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Pede R\": " << (*bEnablePedeR ? "true" : "false") << ",\n";
        configFile << "    \"Enable Pede R P\": " << (*bEnablePedeRP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Pede R2\": " << (*bEnablePedeR2 ? "true" : "false") << ",\n";
        configFile << "    \"Enable Pede R2 P\": " << (*bEnablePedeR2P ? "true" : "false") << ",\n";
        configFile << "    \"Enable Snail U\": " << (*bEnableSnailU ? "true" : "false") << ",\n";
        configFile << "    \"Enable Snail U P\": " << (*bEnableSnailUP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Snail R\": " << (*bEnableSnailR ? "true" : "false") << ",\n";
        configFile << "    \"Enable Snail R P\": " << (*bEnableSnailRP ? "true" : "false") << ",\n";
        configFile << "    \"Enable Bush Sm\": " << (*bEnableBushSm ? "true" : "false") << ",\n";
        configFile << "    \"Enable Sapwood Sm\": " << (*bEnableSapwoodSm ? "true" : "false") << ",\n";
        configFile << "    \"Enable Sapwood Med\": " << (*bEnableSapwoodMed ? "true" : "false") << ",\n";
        configFile << "    \"Enable Sapwood Lg\": " << (*bEnableSapwoodLg ? "true" : "false") << ",\n";
        configFile << "    \"Enable Heartwood Sm\": " << (*bEnableHeartwoodSm ? "true" : "false") << ",\n";
        configFile << "    \"Enable Heartwood Med\": " << (*bEnableHeartwoodMed ? "true" : "false") << ",\n";
        configFile << "    \"Enable Heartwood Lg\": " << (*bEnableHeartwoodLg ? "true" : "false") << ",\n";
        configFile << "    \"Enable Flow Sm\": " << (*bEnableFlowSm ? "true" : "false") << ",\n";
        configFile << "    \"Enable Flow Med\": " << (*bEnableFlowMed ? "true" : "false") << ",\n";
        configFile << "    \"Enable Flow Lg\": " << (*bEnableFlowLg ? "true" : "false") << ",\n";
        configFile << "    \"Enable Players\": " << (*bEnablePlayers ? "true" : "false") << ",\n";
        configFile << "    \"Enable NPC\": " << (*bEnableNPC ? "true" : "false") << ",\n";
        configFile << "    \"Enable Fish\": " << (*bEnableFish ? "true" : "false") << ",\n";
        configFile << "    \"Enable Pools\": " << (*bEnablePools ? "true" : "false") << ",\n";
        configFile << "    \"Enable Loot\": " << (*bEnableLoot ? "true" : "false") << ",\n";
        configFile << "    \"Enable Quest\": " << (*bEnableQuest ? "true" : "false") << ",\n";
        configFile << "    \"Enable Rummage Piles\": " << (*bEnableRummagePiles ? "true" : "false") << ",\n";
        configFile << "    \"Enable Stables\": " << (*bEnableStables ? "true" : "false") << ",\n";
        configFile << "    \"Enable Treasure\": " << (*bEnableTreasure ? "true" : "false") << ",\n";
        configFile << "    \"Enable Others\": " << (bEnableOthers ? "true" : "false") << ",\n";
        configFile << "    \"Menu Size X\": " << windowSizeX << ",\n";
        configFile << "    \"Menu Size Y\": " << windowSizeY << "\n";
        configFile << "}";
        configFile.close();
    }
}