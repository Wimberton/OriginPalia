#pragma once

#include "Core/Handlers/ActorHandler.h"
#include <json/json.h>
#include <imgui.h>
#include <map>

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
	bool Starred = false;

	bool operator<(const FBugType& other) const {
		return std::tie(Kind, Quality, Starred) < std::tie(other.Kind, other.Quality, other.Starred);
	}
};

enum ESaveFile {
	NONE,
	OverlaySettings,
	ESPSettings,
	AimSettings,
	MovementSettings,
	ToolSkillsSettings,
	SellItemsSettings,
	ModSettings,
	CustomThemeSettings,
	HotkeySettings,
	SAVE_ALL
};

// Forward declaration of PaliaOverlay class
class PaliaOverlay;

class Configuration final {
	static bool ConfigLoaded;

	static void LoadOverlaySettings(std::ifstream& configStream);
	static void LoadESPSettings(std::ifstream& configStream);
	static void LoadAimSettings(std::ifstream& configStream);
	static void LoadMovementSettings(std::ifstream& configStream);
	static void LoadToolSkillsSettings(std::ifstream& configStream);
	static void LoadSellItemsSettings(std::ifstream& configStream);
	static void LoadModSettings(std::ifstream& configStream);
	static void LoadCustomThemeSettings(std::ifstream& configStream);
	static void LoadHotkeySettings(std::ifstream& configStream);
	
	static void SaveOverlaySettings();
	static void SaveESPSettings();
	static void SaveAimSettings();
	static void SaveMovementSettings();
	static void SaveToolSkillsSettings();
	static void SaveSellItemsSettings();
	static void SaveModSettings();
	static void SaveCustomThemeSettings();
	static void SaveHotkeySettings();
	
public:
	static void Load();
	static void Save(ESaveFile save_file = ESaveFile::SAVE_ALL);
	static void ApplyCustomTheme();
	
#pragma region ConfigVars

	// ESP

	static bool bEnableESP;
	static double CullDistance;
	static float ESPTextScale;
	static bool bEnableESPDistance;
	static bool bEnableESPDespawnTimer;
	static bool bEnableMapESP;
	static bool bEnableESPIcons;
	
	#pragma region ESP

	// ESP Colors
	static std::map<EOreType, ImU32> OreColors;
	static std::map<FCreatureType, ImU32> AnimalColors;
	static std::map<EOneOffs, ImU32> SingleColors;
	static std::map<ETreeType, ImU32> TreeColors;
	static std::map<EForageableType, ImU32> ForageableColors;
	static std::map<FBugType, ImU32> BugColors;

#pragma endregion

	// Aim

	// InteliAim
	static bool bEnableInteliAim;
	static float FOVRadius;
	static bool bHideFOVCircle;
	static float CircleAlpha;
	static bool bDrawCrosshair;

	// InteliTarget
	static bool bEnableSilentAimbot;
	static bool bTeleportToTargeted;
	static bool bAvoidTeleportingToPlayers;
	static bool bDoRadiusPlayersAvoidance;
	static int TeleportPlayerAvoidanceRadius;
	

	
	// Movement
	static float CustomWalkSpeed;
	static float CustomSprintSpeed;
	static float CustomClimbingSpeed;
	static float CustomGlidingSpeed;
	static float CustomGlidingFallSpeed;
	static float CustomJumpVelocity;
	static int CustomMaxJumps;
	static float CustomMaxStepHeight;

	

	// ToolSkills

	// Tool Swinging
	static bool bEnableAutoSwinging;
	static bool bResourceAutoEquipTool;
	static bool bAvoidGroveChopping;
	
	// Bug Catching
	static bool bEnableBugCatching;
	static bool bBugAutoEquipTool;
	static bool bBugUseRandomDelay;
	static float BugCatchingDistance;
	static int BugSpeedPreset;
	static int BugCatchingDelay;

	// Animal Hunting
	static bool bEnableAnimalHunting;
	static bool bAnimalAutoEquipTool;
	static bool bAnimalUseRandomDelay;
	static float AnimalHuntingDistance;
	static int AnimalSpeedPreset;
	static int AnimalHuntingDelay;

	// Fishing Options
	static bool bFishingNoDurability;
	static bool bFishingMultiplayerHelp;
	static bool bFishingInstantCatch;
	static bool bFishingPerfectCatch;
	static bool bFishingDiscard;
	static bool bFishingOpenStoreWaterlogged;
	static bool bRequireClickFishing;

	// Others
	static bool bEnableAutoGather;
	static bool bEnableCookingMinigameSkip;



	// SellItems

	// Buying
	static bool bEnableBuyMultiplier;
	static bool bEnableCtrlClickBuy;
	static int buyMultiplierValue;
	
	// AutoSell
	static bool bFishingSell;
	static bool bFishingSellSQ;
	static bool bFishingSellRarity[7];
	static bool bBugSell;
	static bool bBugSellSQ;
	static bool bBugSellRarity[7];
	static bool bHuntingSell;
	static bool bHuntingSellSQ;
	static bool bHuntingSellRarity[7];
	static bool bFlowerSell;
	static bool bForageableSell;
	static bool bForageableSellSQ;
	static bool bForageableSellRarity[7];
	static bool bTreeSell;
	static bool bTreeSellRarity[4];

	

	// Mods

	// Character
	static bool bEnableAntiAfk;
	static bool bEnableUnlimitedWardrobeRespec;
	static bool bEnableInteractionMods;
	static float InteractionRadius;
	static bool bEnableWaypointTeleport;
	static bool bEasyModeActive;
	
	// Fun
	static bool bPlaceAnywhere;
	static double PlacementRotation;
	static bool bEnableOutfitCustomization;
	static bool bEnableContentUnlocker;
	static bool bEnableFreeLandscape;
	static bool bEnableShowWeeklyWants;
	static bool bEnableToolBricker;
	static bool bEnableRequestMods;
	static bool bEnableUnseenItems;

	static bool bEnablePrizeWheel;
	static bool bPrizeWheelUseLocked;
	static bool PrizeWheelSlots[9];



	// Origin
	static bool bFirstUse;
	static bool bShowWatermark;
	static bool showEnabledFeaturesWindow;
	static int selectedTheme;
	static std::map<int, ImVec4> customColors;
	
	// Window configs
	static float windowSizeX;
	static float windowSizeY;
	static float activeWindowPosX;
	static float activeWindowPosY;
};

#pragma endregion