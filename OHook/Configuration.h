#pragma once
#include <json/json.h>
#include <imgui.h>
#include <map>

#include "ActorHandler.h"
#include "DetourManager.h"

struct FCreatureType {
	ECreatureKind Kind;
	ECreatureQuality Quality;

	bool operator<(const FCreatureType& other) const {
		return std::tie(Kind, Quality) < std::tie(other.Kind, other.Quality);
	}
};

struct FBugType {
	EBugKind Kind;
	EBugQuality Quality;
	bool Starred;

	bool operator<(const FBugType& other) const {
		return std::tie(Kind, Quality, Starred) < std::tie(other.Kind, other.Quality, other.Starred);
	}
};

// Forward declaration of PaliaOverlay class
class PaliaOverlay;

class Configuration final {
	static void LoadESPArraysPtr(PaliaOverlay* Overlay);

	static void CheckAndMigrateOldConfig();
	static void MigrateOldConfig(const Json::Value& oldConfig);

	static bool ConfigLoaded;
	static bool HasMigrated;

	static void LoadCustomThemeSettings(std::ifstream& configStream);
	static void LoadAimSettings(std::ifstream& configStream);
	static void LoadOverlaySettings(std::ifstream& configStream);
	static void LoadModSettings(std::ifstream& configStream);
	static void LoadMovementSettings(std::ifstream& configStream);
	static void LoadESPSettings(std::ifstream& configStream);

	static void SaveCustomThemeSettings();
	static void SaveAimSettings();
	static void SaveOverlaySettings();
	static void SaveModSettings();
	static void SaveMovementSettings();
	static void SaveESPSettings();

public:
	static void Load(PaliaOverlay* Overlay);
	static void Save();

	static void ApplyCustomTheme();

#pragma region ConfigVars

	// Config Version
	const static float Version, AimVersion, ESPVersion, ModVersion, MovementVersion, OverlayVersion, CustomThemeVersion;

	// Crafting Cooking Booleans
	// bool bEnableInstantCraftingCooking;

	// Window configs
	static float windowSizeX;
	static float windowSizeY;
	static int selectedTheme;
	static std::map<int, ImVec4> customColors;

	static float activeWindowPosX;
	static float activeWindowPosY;
	static float activeModsOpacity;
	static float activeModsRounding;

	static bool showEnabledFeaturesWindow;
	static bool activeModsStickToRight;

	// Aimbots
	static bool bEnableAimbot;
	static bool bEnableSilentAimbot;

	// Tools
#ifdef ENABLE_SUPPORTER_FEATURES
	static bool bEnableAutoToolUse;
#endif

	// Game Modifiers
	static float CustomWalkSpeed; // Custom, Dynamic walk speed
	static float CustomSprintSpeedMultiplier; // Custom, Dynamic sprint speed multiplier
	static float CustomClimbingSpeed; // Custom, Dynamic climbing speed
	static float CustomGlidingSpeed; // Custom, Dynamic gliding speed
	static float CustomGlidingFallSpeed; // Custom, Dynamic gliding fall speed
	static float CustomJumpVelocity; // Custom, Dynamic jump velocity
	static float CustomMaxStepHeight; // Custom, Dynamic maximum step height

	//static bool bAutoGatherOnTeleport;
	static bool bEnableAutoGather;
	
	// Fishing Options
	static bool bFishingNoDurability;
	static bool bFishingMultiplayerHelp;
	static bool bFishingInstantCatch;
	static bool bFishingPerfectCatch;
	static bool bFishingSell;
	static bool bFishingDiscard;
	static bool bFishingOpenStoreWaterlogged;
	static bool bRequireClickFishing;

	// Item Booleans
	static bool bEasyModeActive;
	static bool bEnableLootbagTeleportation;
	static bool bEnableWaypointTeleport;

	// Housing
	static bool bPlaceAnywhere;
#ifdef ENABLE_SUPPORTER_FEATURES
	static bool bEnableFreeLandscape;
#endif
	static bool bManualPositionAdjustment;

	// Gardening
	static bool bInfiniteWateringCan;

	// Quicksell Hotkeys
	static bool bEnableQuicksellHotkeys;

	// Fun Mods
	static bool bEnableAntiAfk;
	static bool bEnableUnlimitedWardrobeRespec;
	static bool bEnableMinigameSkip;
	static bool bEnableInteractionMods;
#ifdef ENABLE_SUPPORTER_FEATURES
	static bool bEnableOutfitCustomization;
#endif
	static bool bEnableRequestMods;

	static bool bEnablePrizeWheel;
	static bool bPrizeWheelUseLocked;
	static bool PrizeWheelSlots[9];
	static bool* bEnableWheelSlot0;
	static bool* bEnableWheelSlot1;
	static bool* bEnableWheelSlot2;
	static bool* bEnableWheelSlot3;
	static bool* bEnableWheelSlot4;
	static bool* bEnableWheelSlot5;
	static bool* bEnableWheelSlot6;
	static bool* bEnableWheelSlot7;
	static bool* bEnableWheelSlot8;

	static float FOVRadius;
	static float InteractionRadius;
	static int AvoidanceRadius;
	static float CircleAlpha;

#pragma region ESP

	// ESP Colors
	static std::map<EOreType, ImU32> OreColors;
	static std::map<FCreatureType, ImU32> AnimalColors;
	static std::map<EOneOffs, ImU32> SingleColors;
	static std::map<ETreeType, ImU32> TreeColors;
	static std::map<EFishType, ImU32> FishColors;
	static std::map<EForageableType, ImU32> ForageableColors;
	static std::map<FBugType, ImU32> BugColors;

	// ESP Booleans

	static bool bShowWatermark;

	static bool bEnableESP;
	static bool bESPIcons;
	static bool bESPIconDistance;
	static bool bDrawFOVCircle;
	static bool bHideFOVCircle;
	static bool bDrawCrosshair;
	static bool bTeleportToTargeted;
	static bool bAvoidTeleportingToPlayers;
	static bool bDoRadiusPlayersAvoidance;
	static bool bEnableESPCulling;
	// static bool bEnableDespawnTimer; // TODO: Still crashing

	// ESP Numericals

	//static float ESPTextScale;
	static int CullDistance;

	// ESP Animals

	static bool* bEnableSernuk;
	static bool* bEnableElderSernuk;
	static bool* bEnableProudhornSernuk;
	static bool* bEnableChapaa;
	static bool* bEnableStripedChapaa;
	static bool* bEnableAzureChapaa;
	static bool* bEnableMinigameChapaa;
	static bool* bEnableMuujin;
	static bool* bEnableBandedMuujin;
	static bool* bEnableBluebristleMuujin;

	// ESP Ores

	static bool* bEnableClayLg;

	static bool* bEnableStoneSm;
	static bool* bEnableStoneMed;
	static bool* bEnableStoneLg;

	static bool* bEnableCopperSm;
	static bool* bEnableCopperMed;
	static bool* bEnableCopperLg;

	static bool* bEnableIronSm;
	static bool* bEnableIronMed;
	static bool* bEnableIronLg;

	static bool* bEnablePaliumSm;
	static bool* bEnablePaliumMed;
	static bool* bEnablePaliumLg;

	// ESP Forage Types

	static bool* bEnableCoral;
	static bool* bEnableOyster;
	static bool* bEnableShell;

	static bool* bEnablePoisonFlower;
	static bool* bEnablePoisonFlowerP;

	static bool* bEnableWaterFlower;
	static bool* bEnableWaterFlowerP;

	static bool* bEnableHeartdrop;
	static bool* bEnableHeartdropP;

	static bool* bEnableSundrop;
	static bool* bEnableSundropP;

	static bool* bEnableDragonsBeard;
	static bool* bEnableDragonsBeardP;

	static bool* bEnableEmeraldCarpet;
	static bool* bEnableEmeraldCarpetP;

	static bool* bEnableMushroomBlue;
	static bool* bEnableMushroomBlueP;

	static bool* bEnableMushroomRed;
	static bool* bEnableMushroomRedP;

	static bool* bEnableDariCloves;
	static bool* bEnableDariClovesP;

	static bool* bEnableHeatRoot;
	static bool* bEnableHeatRootP;

	static bool* bEnableSpicedSprouts;
	static bool* bEnableSpicedSproutsP;

	static bool* bEnableSweetLeaves;
	static bool* bEnableSweetLeavesP;

	static bool* bEnableGarlic;
	static bool* bEnableGarlicP;

	static bool* bEnableGinger;
	static bool* bEnableGingerP;

	static bool* bEnableGreenOnion;
	static bool* bEnableGreenOnionP;

	// ESP Bug Types

	static bool* bEnableBeeU;
	static bool* bEnableBeeUP;

	static bool* bEnableBeeR;
	static bool* bEnableBeeRP;

	static bool* bEnableBeetleC;
	static bool* bEnableBeetleCP;

	static bool* bEnableBeetleU;
	static bool* bEnableBeetleUP;

	static bool* bEnableBeetleR;
	static bool* bEnableBeetleRP;

	static bool* bEnableBeetleE;
	static bool* bEnableBeetleEP;

	static bool* bEnableButterflyC;
	static bool* bEnableButterflyCP;

	static bool* bEnableButterflyU;
	static bool* bEnableButterflyUP;

	static bool* bEnableButterflyR;
	static bool* bEnableButterflyRP;

	static bool* bEnableButterflyE;
	static bool* bEnableButterflyEP;

	static bool* bEnableCicadaC;
	static bool* bEnableCicadaCP;

	static bool* bEnableCicadaU;
	static bool* bEnableCicadaUP;

	static bool* bEnableCicadaR;
	static bool* bEnableCicadaRP;

	static bool* bEnableCrabC;
	static bool* bEnableCrabCP;

	static bool* bEnableCrabU;
	static bool* bEnableCrabUP;

	static bool* bEnableCrabR;
	static bool* bEnableCrabRP;

	static bool* bEnableCricketC;
	static bool* bEnableCricketCP;

	static bool* bEnableCricketU;
	static bool* bEnableCricketUP;

	static bool* bEnableCricketR;
	static bool* bEnableCricketRP;

	static bool* bEnableDragonflyC;
	static bool* bEnableDragonflyCP;

	static bool* bEnableDragonflyU;
	static bool* bEnableDragonflyUP;

	static bool* bEnableDragonflyR;
	static bool* bEnableDragonflyRP;

	static bool* bEnableDragonflyE;
	static bool* bEnableDragonflyEP;

	static bool* bEnableGlowbugC;
	static bool* bEnableGlowbugCP;

	static bool* bEnableGlowbugU;
	static bool* bEnableGlowbugUP;

	static bool* bEnableLadybugC;
	static bool* bEnableLadybugCP;

	static bool* bEnableLadybugU;
	static bool* bEnableLadybugUP;

	static bool* bEnableMantisU;
	static bool* bEnableMantisUP;

	static bool* bEnableMantisR;
	static bool* bEnableMantisRP;

	static bool* bEnableMantisR2;
	static bool* bEnableMantisR2P;

	static bool* bEnableMantisE;
	static bool* bEnableMantisEP;

	static bool* bEnableMothC;
	static bool* bEnableMothCP;

	static bool* bEnableMothU;
	static bool* bEnableMothUP;

	static bool* bEnableMothR;
	static bool* bEnableMothRP;

	static bool* bEnablePedeU;
	static bool* bEnablePedeUP;

	static bool* bEnablePedeR;
	static bool* bEnablePedeRP;

	static bool* bEnablePedeR2;
	static bool* bEnablePedeR2P;

	static bool* bEnableSnailU;
	static bool* bEnableSnailUP;

	static bool* bEnableSnailR;
	static bool* bEnableSnailRP;

	// ESP Trees

	static bool* bEnableBushSm;

	static bool* bEnableSapwoodSm;
	static bool* bEnableSapwoodMed;
	static bool* bEnableSapwoodLg;

	static bool* bEnableHeartwoodSm;
	static bool* bEnableHeartwoodMed;
	static bool* bEnableHeartwoodLg;

	static bool* bEnableFlowSm;
	static bool* bEnableFlowMed;
	static bool* bEnableFlowLg;

	// ESP Player & Entities

	static bool* bEnablePlayers;
	static bool* bEnableNPC;
	static bool* bEnableFish;
	static bool* bEnablePools;
	static bool* bEnableLoot;
	static bool* bEnableQuest;
	static bool* bEnableRummagePiles;
	static bool* bEnableStables;
	static bool* bEnableTreasure;
	static bool* bEnableTimedDrop;
	static bool  bEnableOthers;

#pragma endregion
#pragma endregion
};