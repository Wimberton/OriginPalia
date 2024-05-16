﻿#pragma once
#include <OverlayBase.h>
#include <map>
#include <vector>
#include <string>
#include <SDK.hpp>
#include <imgui.h>
#include <sstream>
#include <fstream>
#include <functional>
#include <cmath>
#include <Windows.h>
#include <ShlObj.h>
#include <chrono>

using namespace SDK;

enum class EType
{
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
	MAX
};

enum class ECreatureKind
{
	Unknown,
	Chapaa,
	Cearnuk,
	TreeClimber,
	MAX
};

enum class ECreatureQuality
{
	Unknown,
	Tier1,
	Tier2,
	Tier3,
	Chase,
	MAX
};

enum class EBugKind
{
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

enum class EBugQuality
{
	Unknown,
	Common,
	Uncommon,
	Rare,
	Rare2,
	Epic,
	MAX
};

enum class EGatherableSize
{
	Unknown,
	Small,
	Medium,
	Large,
	Bush,
	MAX
};

enum class ETreeType
{
	Unknown,
	Flow,
	Heartwood,
	Sapwood,
	Bush,
	MAX
};

enum class EOreType
{
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

enum class EFishType
{
	Unknown,
	Node,
	Hook,
	MAX
};

enum class EForageableType
{
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

enum class EOneOffs
{
	Player,
	NPC,
	FishHook,
	FishPool,
	Loot,
	Quest,
	RummagePiles,
	Stables,
	Others,
	MAX
};

struct FEntry
{
	AActor* Actor;
	FVector WorldPosition;
	std::string DisplayName;
	EType ActorType;
	int Type;
	int Quality;
	int Variant;
	bool shouldAdd;
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

const char* const STools[] = {
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

enum class ELocation
{
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

struct FLocation
{
	std::string MapName;
	ELocation Type;
	std::string Name;
	FVector Location;
	FRotator Rotate;
};

class PaliaOverlay : public OverlayBase
{
protected:
	virtual void DrawHUD();
	virtual void DrawOverlay();

public:
	void SetupColors();
	void ProcessActors(int);

public:
	static std::map<int, std::string> CreatureQualityNames;
	static std::map<int, std::string> BugQualityNames;
	static std::map<int, std::string> GatherableSizeNames;
	static std::map<int, std::string> TypeIcons;

	static std::string GetQualityName(int quality, EType type) {
		switch (type) {
		case EType::Animal:
		case EType::Bug:
			return BugQualityNames[quality];
		case EType::Forage:
			return GatherableSizeNames[quality];
		default:
			return CreatureQualityNames[quality];
		}
	}

	// Last time ESPs were cached
	double LastCachedTime;
	int ActorStep;

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
		{ "UserDefined", ELocation::Global_Home, "[Global] Housing Plot", FVector(00000.000f, 00000.000f, 00000.000f), FRotator(0.000f, 0.00f, 0.000f)},
		// Kilima Entrances
		{ "Village_Root", ELocation::Kilima_HousingPlot, "[Kilima] Housing Plot Entrance", FVector(20204.810f, -15375.620f, 2252.460f), FRotator(0.000f, -6.24f, 0.000f)},
		{ "Village_Root", ELocation::Kilima_ToBahariBay, "[Kilima] Bahari Bay Entrance", FVector(50623.753f, -5403.530f, 1312.610f), FRotator(0.000f, -24.72f, 0.000f)},
		// Kilima Stables
		{ "Village_Root", ELocation::Kilima_CentralStables, "[Kilima] Central Stables", FVector(9746.436f, 11380.740f, -139.406f), FRotator(0.000f, 145.62f,-0.000f)},
		{ "Village_Root", ELocation::Kilima_MirrorFields, "[Kilima] Mirror Fields", FVector(-6586.737f, -24412.872f, 1627.018f), FRotator(0.000f, 259.87f, 0.000f)},
		{ "Village_Root", ELocation::Kilima_LeafhopperHills, "[Kilima] Leafhopper Hills", FVector(-21589.245f, 3920.281f, 306.105f), FRotator(0.000f, 117.04f, 0.000f)},
		{ "Village_Root", ELocation::Kilima_WhisperingBanks, "[Kilima] Whispering Banks", FVector(38958.433f, 7574.109f, -677.983f), FRotator(0.000f, 214.71f, 0.000f)},
		// Kilima Others
		{ "Village_Root", ELocation::Kilima_Fairgrounds_MajiMarket, "[Kilima] Fairgrounds Entrance (Maji Market)", FVector(54696.812f, 21802.943f, -114.603f), FRotator(0.000f, -7.65f, 0.000f)},
		{ "Village_Root", ELocation::Kilima_GeneralStore, "[Kilima] General Store", FVector(156.193f, -1509.144f, 285.576f), FRotator(0.000f, -122.07f, 0.000f)},
		{ "Village_Root", ELocation::Kilima_RepairStation, "[Kilima] Repair Station", FVector(2465.246f, 6734.343f, 303.050f), FRotator(0.000f, 120.89f, 0.000f)},
		{ "Village_Root", ELocation::Kilima_PhoenixFalls, "[Kilima] Phoenix Falls", FVector(7596.145f, -51226.853f, 4006.341f), FRotator(0.000f, -68.64f, 0.000f)},
		{ "Village_Root", ELocation::Kilima_DragonShrine, "[Kilima] Dragon Shrine", FVector(33743.980f, -22253.369f, 1303.870f), FRotator(0.000f, -90.211f, 0.000f)},
		{ "Village_Root", ELocation::Kilima_FishermansLagoon, "[Kilima] Fisherman's Lagoon", FVector(25325.044f, 19246.651f, -459.559f), FRotator(0.000f, 25.76f, 0.000f)},
		{ "UserDefined", ELocation::Kilima_WimsFishingSpot, "[Kilima] Wims Secret Fishing Spot", FVector(58217.884f, 41798.154f, -452.593f), FRotator(0.000f, 94.089f, 0.000f)},
		// Bahari Entrances
		{ "AZ1_01_Root", ELocation::Bahari_HousingPlot, "[Bahari] Housing Plot Entrance", FVector(54451.778f, -81603.206f, 940.587f), FRotator(0.000f, 2.45f, 0.000f)},
		{ "AZ1_01_Root", ELocation::Bahari_ToKilimaValley, "[Bahari] Kilima Valley Entrance", FVector(59456.610f,-10932.216f, 897.642f), FRotator(0.000f, -157.46f, 0.000f)},
		// Bahari Stables
		{ "AZ1_01_Root", ELocation::Bahari_CentralStables, "[Bahari] Central Stables", FVector(103331.330f, -63125.339f, -1948.051f), FRotator(0.000f, 148.09f,-0.000f)},
		{ "AZ1_01_Root", ELocation::Bahari_TheOutskirts, "[Bahari] The Outskirts", FVector(55743.927f, -81821.534f, 902.205f), FRotator(0.000f, 226.06f, 0.000f)},
		{ "AZ1_01_Root", ELocation::Bahari_CoralShores, "[Bahari] Coral Shores", FVector(143344.052f, -71851.374f, -4373.302f), FRotator(0.000f, 352.16f, 0.000f)},
		{ "AZ1_01_Root", ELocation::Bahari_PulsewaterPlains, "[Bahari] Pulsewater Plains", FVector(101257.267f, -23873.064f, -1655.604f), FRotator(0.000f, 287.12f, 0.000f)},
		{ "AZ1_01_Root", ELocation::Bahari_HideawayBluffs, "[Bahari] Hideaway Bluffs", FVector(82490.721f, 2994.478f, -714.785f), FRotator(0.000f, 14.61f, 0.000f)},
		{ "AZ1_01_Root", ELocation::Bahari_BeachcomberCove, "[Bahari] Beachcomber Cove", FVector(128767.154f, 4163.842f, -4406.759f), FRotator(0.000f, 312.00f, 0.000f)},
	};

	// Class name -> Display name map
	std::map<std::string, std::string> CLASS_NAME_ALIAS = {
		// Trees //

		// Birch
		{"BP_TreeChoppable_Birch_Sapling_C", "Birch (Sap)"},
		{"BP_TreeChoppable_Birch_Small_C", "Birch (S)"},
		{"BP_TreeChoppable_Birch_Medium_C", "Birch (M)"},
		{"BP_TreeChoppable_Birch_Large_C", "Birch (L)"},
		{"BP_TreeChoppable_Birch_Sapling_CoOp_C", "Flow-Infused Birch (Sap)"},
		{"BP_TreeChoppable_Birch_Small_CoOp_C", "Flow-Infused Birch (S)"},
		{"BP_TreeChoppable_Birch_Medium_CoOp_C", "Flow-Infused Birch (M)"},
		{"BP_TreeChoppable_Birch_Large_CoOp_C", "Flow-Infused Birch (L)"},

		// Juniper
		{"BP_TreeChoppable_Juniper_Sapling_C", "Juniper (Sap)"},
		{"BP_TreeChoppable_Juniper_Small_C", "Juniper (S)"},
		{"BP_TreeChoppable_Juniper_Medium_C", "Juniper (M)"},
		{"BP_TreeChoppable_Juniper_Large_C", "Juniper (L)"},
		{"BP_TreeChoppable_Juniper_Sapling_CoOp_C", "Flow-Infused Juniper (Sap)"},
		{"BP_TreeChoppable_Juniper_Small_CoOp_C", "Flow-Infused Juniper (S)"},
		{"BP_TreeChoppable_Juniper_Medium_CoOp_C", "Flow-Infused Juniper (M)"},
		{"BP_TreeChoppable_Juniper_Large_CoOp_C", "Flow-Infused Juniper (L)"},

		// Oak
		{"BP_TreeChoppable_Oak_Sapling_C", "Oak (Sap)"},
		{"BP_TreeChoppable_Oak_Small_C", "Oak (S)"},
		{"BP_TreeChoppable_Oak_Medium_C", "Oak (M)"},
		{"BP_TreeChoppable_Oak_Large_C", "Oak (L)"},
		{"BP_TreeChoppable_Oak_Sapling_CoOp_C", "Flow-Infused Oak (Sap)"},
		{"BP_TreeChoppable_Oak_Small_CoOp_C", "Flow-Infused Oak (S)"},
		{"BP_TreeChoppable_Oak_Medium_CoOp_C", "Flow-Infused Oak (M)"},
		{"BP_TreeChoppable_Oak_Large_CoOp_C", "Flow-Infused Oak (L)"},

		// Pine
		{"BP_TreeChoppable_Pine_Sapling_C", "Pine (Sap)"},
		{"BP_TreeChoppable_Pine_Small_C", "Pine (S)"},
		{"BP_TreeChoppable_Pine_Medium_C", "Pine (M)"},
		{"BP_TreeChoppable_Pine_Large_C", "Pine (L)"},
		{"BP_TreeChoppable_Pine_Large2_C", "Pine (L)"},
		{"BP_TreeChoppable_Pine_Sapling_CoOp_C", "Flow-Infused Pine (Sap)"},
		{"BP_TreeChoppable_Pine_Small_CoOp_C", "Flow-Infused Pine (S)"},
		{"BP_TreeChoppable_Pine_Medium_CoOp_C", "Flow-Infused Pine (M)"},
		{"BP_TreeChoppable_Pine_Large_CoOp_C", "Flow-Infused Pine (L)"},
		{"BP_TreeChoppable_Pine_Large2_CoOp_C", "Flow-Infused Pine (L)"},

		// Shrubs
		{"BP_ShrubChoppable_Bush_C", "Bush"},

		// Ores //

		// Stone
		{"BP_Mining_Stone_Small_C", "Stone (S)"},
		{"BP_Mining_Stone_Medium_C", "Stone (M)"},
		{"BP_Mining_Stone_Large_C", "Stone (L)"},
		{"BP_Mining_Stone_Small_SingleHarvest_C", "Stone (S)"},
		{"BP_Mining_Stone_Medium_SingleHarvest_C", "Stone (M)"},
		{"BP_Mining_Stone_Large_SingleHarvest_C", "Stone (L)"},
		{"BP_Mining_Stone_Small_MultiHarvest_C", "Stone (S)"},
		{"BP_Mining_Stone_Medium_MultiHarvest_C", "Stone (M)"},
		{"BP_Mining_Stone_Large_MultiHarvest_C", "Stone (L)"},

		// Copper
		{"BP_Mining_Copper_Small_C", "Copper (S)"},
		{"BP_Mining_Copper_Medium_C", "Copper (M)"},
		{"BP_Mining_Copper_Large_C", "Copper (L)"},
		{"BP_Mining_Copper_Small_SingleHarvest_C", "Copper (S)"},
		{"BP_Mining_Copper_Medium_SingleHarvest_C", "Copper (M)"},
		{"BP_Mining_Copper_Large_SingleHarvest_C", "Copper (L)"},
		{"BP_Mining_Copper_Small_MultiHarvest_C", "Copper (S)"},
		{"BP_Mining_Copper_Medium_MultiHarvest_C", "Copper (M)"},
		{"BP_Mining_Copper_Large_MultiHarvest_C", "Copper (L)"},

		// Iron
		{"BP_Mining_Iron_Small_C", "Iron (S)"},
		{"BP_Mining_Iron_Medium_C", "Iron (M)"},
		{"BP_Mining_Iron_Large_C", "Iron (L)"},
		{"BP_Mining_Iron_Small_SingleHarvest_C", "Iron (S)"},
		{"BP_Mining_Iron_Medium_SingleHarvest_C", "Iron (M)"},
		{"BP_Mining_Iron_Large_SingleHarvest_C", "Iron (L)"},
		{"BP_Mining_Iron_Small_MultiHarvest_C", "Iron (S)"},
		{"BP_Mining_Iron_Medium_MultiHarvest_C", "Iron (M)"},
		{"BP_Mining_Iron_Large_MultiHarvest_C", "Iron (L)"},

		// Palium
		{"BP_Mining_Palium_C", "Palium"},
		{"BP_Mining_Palium_Small_C", "Palium (S)"},
		{"BP_Mining_Palium_Medium_C", "Palium (M)"},
		{"BP_Mining_Palium_Large_C", "Palium (L)"},
		{"BP_Mining_Palium_SingleHarvest_C", "Palium"},
		{"BP_Mining_Palium_Small_SingleHarvest_C", "Palium (S)"},
		{"BP_Mining_Palium_Medium_SingleHarvest_C", "Palium (M)"},
		{"BP_Mining_Palium_Large_SingleHarvest_C", "Palium (L)"},
		{"BP_Mining_Palium_MultiHarvest_C", "Palium"},
		{"BP_Mining_Palium_Small_MultiHarvest_C", "Palium (S)"},
		{"BP_Mining_Palium_Medium_MultiHarvest_C", "Palium (M)"},
		{"BP_Mining_Palium_Large_MultiHarvest_C", "Palium (L)"},

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
		{"BP_ValeriaHuntingCreature_Cearnuk_T1_C", "Sernuk (Animal)"},
		{"BP_ValeriaHuntingCreature_Cearnuk_T2_C", "Elder Sernuk (Animal)"},
		{"BP_ValeriaHuntingCreature_Cearnuk_T3_C", "Proudhorn Sernuk (Animal)"},

		// Chaapa
		{"BP_ValeriaHuntingCreature_Chapaa_T1_C", "Chapaa (Animal)"},
		{"BP_ValeriaHuntingCreature_Chapaa_T2_C", "Striped Chapaa (Animal)"},
		{"BP_ValeriaHuntingCreature_Chapaa_T3_C", "Azure Chapaa (Animal)"},

		// Chaapa (Minigame Event)
		{"BP_ValeriaHuntingCreature_ChapaaChase_Base_C", "Chapaa Chase - Base (Animal)"},
		{"BP_ValeriaHuntingCreature_ChapaaChase_Fast_C", "Chapaa Chase - Fast (Animal)"},

		// Muujin
		{"BP_ValeriaHuntingCreature_TreeClimber_T1_C", "Muujin (Animal)"},
		{"BP_ValeriaHuntingCreature_TreeClimber_T2_C", "Banded Muujin (Animal)"},
		{"BP_ValeriaHuntingCreature_TreeClimber_T3_C", "Bluebristle Muujin (Animal)"},

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
		{ "BP_Coral_C", "Coral" },
		{ "BP_Gatherable_MushroomBlue+_C", "Brightshroom *" },
		{ "BP_Gatherable_MushroomBlue_C", "Brightshroom" },
		{ "BP_Gatherable_MushroomR+_C", "Mountain Morel *" },
		{ "BP_Gatherable_MushroomR_C", "Mountain Morel" },
		{ "BP_HeartdropLilly+_C", "Heartdrop Lily *" }, // have yet to see a SQ
		{ "BP_HeartdropLilly_C", "Heartdrop Lily" },
		{ "BP_Moss_DragonsBeard+_C", "Dragon's Beard Peat *" },
		{ "BP_Moss_DragonsBeard_C", "Dragon's Beard Peat" },
		{ "BP_Moss_EmeraldCarpet+_C", "Emerald Carpet Moss *" },
		{ "BP_Moss_EmeraldCarpet_C", "Emerald Carpet Moss" },
		{ "BP_Oyster_C", "Unopened Oyster" },
		{ "BP_PoisonFlower+_C", "Briar Daisy *" },
		{ "BP_PoisonFlower_C", "Briar Daisy" },
		{ "BP_Seashell_C", "Shell" },
		{ "BP_Spice_DariCloves+_C", "Dari Cloves *" },
		{ "BP_Spice_DariCloves_C", "Dari Cloves" },
		{ "BP_Spice_HeatRoot+_C", "Heat Root *" },
		{ "BP_Spice_HeatRoot_C", "Heat Root" },
		{ "BP_Spiced_Sprouts+_C", "Spice Sprouts *" },
		{ "BP_Spiced_Sprouts_C", "Spice Sprouts" },
		{ "BP_SundropLillies+_C", "Sundrop Lily *" },
		{ "BP_SundropLillies_C", "Sundrop Lily" },
		{ "BP_SweetLeaves+_C", "Sweet Leaf *" },
		{ "BP_SweetLeaves_C", "Sweet Leaf" },
		{ "BP_WaterFlower+_C", "Crystal Lake Lotus *" },
		{ "BP_WaterFlower_C", "Crystal Lake Lotus" },
		{ "BP_WildGarlic+_C", "Wild Garlic *" },
		{ "BP_WildGarlic_C", "Wild Garlic" },
		{ "BP_Gatherable_Ginger+_C", "Wild Ginger *" },
		{ "BP_Gatherable_Ginger_C", "Wild Ginger" },
		{ "BP_Gatherable_GreenOnion+_C", "Wild Green Onion *" },
		{ "BP_Gatherable_GreenOnion_C", "Wild Green Onion" },

		// Fishing Nodes
		{ "BP_WaterPlane_Fishing_OceanAZ1_SQ_C", "Coast" },
		{ "BP_WaterPlane_Fishing_PondVillage_SQ_C", "Pond" },
		{ "BP_WaterPlane_Fishing_LakeVillage_SQ_C", "Lake" },
		{ "BP_WaterPlane_Fishing_RiverVillage_SQ_C", "Kilima River" },
		{ "BP_WaterPlane_Fishing_RiverAZ1_SQ_C", "Bahari River" },
		{ "BP_WaterPlane_Fishing_AZ1_Cave_SQ_C", "Cave" },

		// Fish
		{ "BP_FishingJackpot", "Waterlogged Chest" },
		{ "BP_FishingRecipeBook", "Recipe Book" },
		{ "BP_Fish_AlbinoEeel", "Albino Eeel" },
		{ "BP_Fish_Alligator_Gar", "Alligator Gar" },
		{ "BP_Fish_AncientFish", "Ancient Fish" },
		{ "BP_Fish_BahariBream", "Bahari Bream" },
		{ "BP_Fish_Bahari_Pike", "Bahari Pike" },
		{ "BP_Fish_Barracuda", "Barracuda" },
		{ "BP_Fish_BatRay", "Bat Ray" },
		{ "BP_Fish_Beluga_Sturgeon", "Beluga Sturgeon" },
		{ "BP_Fish_Bigeye_Tuna", "Bigeye_Tuna" },
		{ "BP_Fish_Black_Sea_Bass", "Black Sea Bass" },
		{ "BP_Fish_Blobfish", "Blobfish" },
		{ "BP_Fish_BlueMarlin", "Blue Marlin" },
		{ "BP_Fish_Blue_Spotted_Ray", "Blue Spotted Ray" },
		{ "BP_Fish_Cactus_Moray", "Cactus Moray" },
		{ "BP_Fish_CalicoKoi", "Calico Koi" },
		{ "BP_Fish_CantankerousKoi", "CantankerousKoi" },
		{ "BP_Fish_Channel_Catfish", "Channel Catfish" },
		{ "BP_Fish_Chub", "Chub" },
		{ "BP_Fish_Cloudfish", "Cloudfish" },
		{ "BP_Fish_Codfish", "Codfish" },
		{ "BP_Fish_Crimson_Fangtooth", "Crimson Fangtooth" },
		{ "BP_Fish_Crucian_Carp", "Crucian Carp" },
		{ "BP_Fish_Cutthroat_Trout", "Cutthroat Trout" },
		{ "BP_Fish_Dawnray", "Dawnray" },
		{ "BP_Fish_Duskray", "Duskray" },
		{ "BP_Fish_Enchanted_Pupfish", "Enchanted Pupfish" },
		{ "BP_Fish_Energized_Piranha", "Energized Piranha" },
		{ "BP_Fish_EyelessMinnow", "Eyeless Minnow" },
		{ "BP_Fish_FairyCarp", "Fairy Carp" },
		{ "BP_Fish_Fathead_Minnow", "Fathead Minnow" },
		{ "BP_Fish_Flametongue_Ray", "Flametongue Ray" },
		{ "BP_Fish_FreshwaterEel", "Freshwater Eel" },
		{ "BP_Fish_Giant_Goldfish", "Giant Goldfish" },
		{ "BP_Fish_Giant_Kilima_Stingray", "Giant Kilima Stingray" },
		{ "BP_Fish_Gillyfin", "Gillyfin" },
		{ "BP_Fish_GoldenSalmon", "Golden Salmon" },
		{ "BP_Fish_Honey_Loach", "Honey Loach" },
		{ "BP_Fish_Hypnotic_Moray", "Hypnotic Moray" },
		{ "BP_Fish_Indigo_Lamprey", "Indigo Lamprey" },
		{ "BP_Fish_Kenjis_Carp", "Kenli's Carp" },
		{ "BP_Fish_KilimaCatFish", "Kilima Catfish" },
		{ "BP_Fish_Kilima_Grayling", "Kilima Grayling" },
		{ "BP_Fish_Kilima_Redfin", "Kilima Redfin" },
		{ "BP_Fish_LargemouthBass", "Largemouth Bass" },
		{ "BP_Fish_Midnight_Paddlefish", "Midnight Paddlefish" },
		{ "BP_Fish_Mirror_Carp", "Mirror Carp" },
		{ "BP_Fish_Mottled_Gobi", "Mottled Gobi" },
		{ "BP_Fish_Mudminnow", "Mudminnow" },
		{ "BP_Fish_MutatedAngler", "Mutated Angler" },
		{ "BP_Fish_Oily_Anchovy", "Oily Anchovy" },
		{ "BP_Fish_Orange_Bluegill", "Orange Bluegill" },
		{ "BP_Fish_Paddlefish", "Paddlefish" },
		{ "BP_Fish_PaintedPerch", "Painted Perch" },
		{ "BP_Fish_Platinum_Chad", "Platinum Chad" },
		{ "BP_Fish_PrismTrout", "Prism Trout" },
		{ "BP_Fish_RadiantSunfish", "Radiant Sunfish" },
		{ "BP_Fish_Rainbow_Trout", "Rainbow Trout" },
		{ "BP_Fish_Red-bellied_piranha", "Red-bellied Piranha" },
		{ "BP_Fish_RibbontailRay", "Ribbontail Ray" },
		{ "BP_Fish_Rosy_Bitterling", "Rosy Bitterling" },
		{ "BP_Fish_Sardine", "Sardine" },
		{ "BP_Fish_Scarlet_Koi", "Scarlet Koi" },
		{ "BP_Fish_Shimmerfin", "Shimmerfin" },
		{ "BP_Fish_SilverSalmon", "Silver Salmon" },
		{ "BP_Fish_Silvery_Minnow", "Silvery Minnow" },
		{ "BP_Fish_SmallmouthBass", "Smallmouth Bass" },
		{ "BP_Fish_SpottedBullhead", "Spotted Bullhead" },
		{ "BP_Fish_Stalking_Catfish", "Stalking Catfish" },
		{ "BP_Fish_Stickleback", "Stickleback" },
		{ "BP_Fish_Stonefish", "Stonefish" },
		{ "BP_Fish_Stormray", "Stormray" },
		{ "BP_Fish_StripedSturgeon", "Striped Sturgeon" },
		{ "BP_Fish_Striped_Dace", "Striped Dace" },
		{ "BP_Fish_Swordfin_Eel", "Swordfin Eel" },
		{ "BP_Fish_Thundering_Eel", "Thundering Eel" },
		{ "BP_Fish_UmbranCarp", "Umbran Carp" },
		{ "BP_Fish_UnicornFish", "Unicorn Fish" },
		{ "BP_Fish_Voidray", "Voidray" },
		{ "BP_Fish_Willow_Lamprey", "Willow Lamprey" },
		{ "BP_Fish_Yellowfin_Tuna", "Yellowfin Tuna" },
		{ "BP_Fish_Yellow_Perch", "Yellow Perch" },
		{ "BP_Trash_Shipfragments", "Ship Fragments" },
		{ "BP_Trash_Wagonwheel", "Wagon Wheel" },
		{ "BP_Trash_WaterloggedBoot", "Waterlogged Boot" },

		// Villagers
		{ "BP_Villager_Miner_C", "[NPC] Hodari" },
		{ "BP_VillagerTheArchaeologist_C", "[NPC] Jina" },
		{ "BP_Villager_Cook_C", "[NPC] Reth" },
		{ "BP_Villager_Tish_C", "[NPC] Tish" },
		{ "BP_VillagerTheDemolitionist_C", "[NPC] Najuma" },
		{ "BP_Villager_Healer_C", "[NPC] Chayne" },
		{ "BP_Villager_Elouisa_C", "[NPC] Elouisa" },
		{ "BP_Villager_TheMagistrate_C", "[NPC] Eshe" },
		{ "BP_Villager_TheLibrarian_C", "[NPC] Caleri" },
		{ "BP_Villager_Tamala_C", "[NPC] Tamala" },
		{ "BP_Villager_Blacksmith_C", "[NPC] Sifuu" },
		{ "BP_Villager_Farmer_C", "[NPC] Badruu" },
		{ "BP_VillagerDeliveryBoy_C", "[NPC] Auni" },
		{ "BP_Villager_Farmboy_C", "[NPC] Nai'o" },
		{ "BP_Villager_Tau_C", "[NPC] Tau" },
		{ "BP_VillagerTheInnKeeper_C", "[NPC] Ashura" },
		{ "BP_Villager_Hekla_C", "[NPC] Hekla" },
		{ "BP_Villager_Zeki_C", "[NPC] Zeki" },
		{ "BP_Villager_Rancher_C", "[NPC] Delaila" },
		{ "BP_VillagerFisherman_C", "[NPC] Einar" },
		{ "BP_Villager_Kenyatta_C", "[NPC] Kenyatta" },
		{ "BP_Villager_Mayor_C", "[NPC] Kenli" },
		{ "BP_Villager_Jel_C", "[NPC] Jel" },
		{ "BP_VillagerTheHunter_C", "[NPC] Hassian" },
		{ "BP_VillagerTheWatcher_C", "[NPC] Subira" },

		// Loot
		{ "BP_Hunting_LootBag_C", "[Loot] Animal"},
		{ "BP_InsectBallLoot_C", "[Loot] Bug" },
		{ "BP_LootChestRockPile_C", "[Loot] Stone" },
		{ "BP_LootChestClayPile_C", "[Loot] Clay" },
		{ "BP_LootChestCopperPile_C", "[Loot] Copper" },
		{ "BP_LootChestIronPile_C", "[Loot] Iron" },
		{ "BP_LootChestPaliumPile_C", "[Loot] Palium" },
		{ "BP_LootChestWoodBundle_Enchanted_C", "[Loot] Flow" },
		{ "BP_LootChestWoodBundle_Pine_C", "[Loot] Heartwood" },
		{ "BP_LootChestWoodBundle_Juniper_C", "[Loot] Sapwood" },
		{ "BP_LootChestWoodBundle_Oak_C", "[Loot] Sapwood" },
		{ "BP_LootChestWoodBundle_Birch_C", "[Loot] Sapwood" },

		{ "BP_ChapaaPile_C", "Rummage Pile (Kilima)" },
		{ "BP_BeachPile_C", "Rummage Pile (Bahari)" },

		{ "BP_Stables_Sign_C", "Stables - Fast Travel" },
		{ "BP_Stables_FrontGate_01_C", "Stables - Front Gate 1" },
		{ "BP_Stables_FrontGate_02_C", "Stables - Front Gate 2" },
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
		{ECreatureQuality::Chase, {"_Base_C","_Fast_C"}}
	};

	// Search map for assigning bug kind type
	std::map<EBugKind, std::vector<std::string>> CREATURE_BUGKIND_MAPPINGS = {
		{EBugKind::Bee, {"_Bug_Bee"}},
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
		{EBugQuality::Epic, {"E_C","E+_C"}}
	};

	// Search map for assigning fish kind type
	std::map<EFishType, std::vector<std::string>> FISH_TYPE_MAPPINGS = {
		{EFishType::Node, {"_WaterPlane_"}},
		{EFishType::Hook, {"_Fish_","_Trash_", "_Fishing"}},
	};

	// Forageables[Type][]
	bool Singles[(int)EOneOffs::MAX] = {};
	unsigned int SingleColors[(int)EOneOffs::MAX] = {
		IM_COL32(0xFF, 0xFF, 0x00, 0xFF),
		IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
		IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
		IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
		IM_COL32(0x00, 0xFF, 0xFF, 0xFF),
		IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
	};

	// Forageables[Type][Starred]
	bool Forageables[(int)EForageableType::MAX][2] = {};
	unsigned int ForageableColors[(int)EForageableType::MAX] = {};

	int ForageableCommon[4] = {
		(int)EForageableType::Oyster,
		(int)EForageableType::Shell,
		(int)EForageableType::Sundrop,
		(int)EForageableType::MushroomRed
	};

	int ForageableUncommon[9] = {
		(int)EForageableType::Coral,
		(int)EForageableType::PoisonFlower,
		(int)EForageableType::WaterFlower,
		(int)EForageableType::EmeraldCarpet,
		(int)EForageableType::SpicedSprouts,
		(int)EForageableType::SweetLeaves,
		(int)EForageableType::Garlic,
		(int)EForageableType::Ginger,
		(int)EForageableType::GreenOnion
	};

	int ForageableRare[3] = {
		(int)EForageableType::DragonsBeard,
		(int)EForageableType::MushroomBlue,
		(int)EForageableType::HeatRoot
	};

	int ForageableEpic[2] = {
		(int)EForageableType::Heartdrop,
		(int)EForageableType::DariCloves
	};

	// Ores[Type][Size]
	bool Ores[(int)EOreType::MAX][(int)EGatherableSize::MAX] = {};

	unsigned int OreColors[(int)EOreType::MAX] = {
		IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
		IM_COL32(0x88, 0x8C, 0x8D, 0xFF), // Stone
		IM_COL32(0xB8, 0x73, 0x33, 0xFF), // Copper
		IM_COL32(0xAD, 0x50, 0x49, 0xFF), // Clay
		IM_COL32(0xA1, 0x9D, 0x94, 0xFF), // Iron
		IM_COL32(0xAA, 0xA9, 0xAD, 0xFF), // Silver
		IM_COL32(0xDB, 0xAC, 0x34, 0xFF), // Gold
		IM_COL32(0x94, 0xA0, 0xE2, 0xFF), // Palium
	};

	// Animals[Type][Size]
	bool Animals[(int)ECreatureKind::MAX][(int)ECreatureQuality::MAX] = {};
	unsigned int AnimalColors[(int)ECreatureKind::MAX][(int)ECreatureQuality::MAX] = {};

	// Bugs[Type][Size][Starred]
	bool Bugs[(int)EBugKind::MAX][(int)EBugQuality::MAX][2] = {};
	unsigned int BugColors[(int)EBugKind::MAX][(int)EBugQuality::MAX] = {};

	// Trees[Type][Size]
	bool Trees[(int)ETreeType::MAX][(int)EGatherableSize::MAX] = {};
	unsigned int TreeColors[(int)ETreeType::MAX] = {
		IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
		IM_COL32(0x67, 0x00, 0xEA, 0xFF),
		IM_COL32(0x00, 0xFF, 0x00, 0xFF),
		IM_COL32(0x00, 0xFF, 0x00, 0xFF),
		IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
	};

	// Trees[Type][Size]
	bool Fish[(int)EFishType::MAX] = {};
	unsigned int FishColors[(int)EFishType::MAX] = {
		IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
		IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
		IM_COL32(0xFF, 0xFF, 0xFF, 0xFF),
	};

	// Is able to be gathered by player
	//bool IsFree(SDK::UGatherableComponent* comp) {
	//	auto isFree = [](SDK::EVitalType type) {
	//		return type != SDK::EVitalType::Coins &&
	//			type != SDK::EVitalType::CommunityPoints &&
	//			type != SDK::EVitalType::Health &&
	//			type != SDK::EVitalType::MedalFishing &&
	//			type != SDK::EVitalType::PremiumCurrency;
	//	};
	//	return isFree(comp->VitalRequired) && isFree(comp->Vital2Required);
	//}*/

	// ============ ESP DISPLAY MAPPINGS ================
	bool CacheActorsFast = true;

	float FOV;
	int screenWidth;
	int screenHeight;
	float FOVRadius = 185.0f;

	// ESP Booleans
	bool bEnableESP = true;
	bool bEnableBoxESP = false;
	bool bEnableInteliAim = false;
	bool bEnableAimbot = false;
	bool bEnableSilentAimbot = false;
	bool bDrawFOVCircle = true;
	bool bTeleportToTargeted = false;
	bool bAvoidTeleportingToPlayers = true;
	bool bTargetTeleportTo = true;
	bool bVisualizeDefault = false;
	bool bEnableESPCulling = true;
	bool bNewTargetAcquired = false;

	// Aim Projectile Settings
	bool bNoBowAimTimesEnabled = false;

	// Aim Fun Mods
	bool bAddAnimalToOrbit = false;

	// ESP Numericals
	int CullDistance = 150;
	std::chrono::steady_clock::time_point LastTeleportToTargetTime;
	std::chrono::steady_clock::time_point LastTimeMapTeleport;

	float SmoothingFactor = 90.0f;
	//FVector AimOffset = FVector(0, 0, 0);
	FVector AimOffset = FVector(-1.5, -1.35, 0);
	float CurrentAimTime = 0.f;
	double SelectionThreshold = 50.0;

	// Movement Booleans
	bool bAutoUpdateTickRegistration = false;
	bool bEnableWaypointTeleport = true;
	bool bBypassSpeedCheat = false;

	bool bEnableGameSpeed = false;
	bool bEnableNoclip = false;
	bool bEnableMapTeleportToMarker = true;
	bool bPreviousNoclipState = false;
	bool respectGlideSpeedLimits = false;
	bool unlimClimbingDashes = false;
	bool AlwaysJumpingIsEnabled = false;
	bool bNetworkSkipProxyPredictionAlways = false;
	bool bNetworkSkipProxyPredictionOnNetUpdate = false;

	bool ZeroServerThrottleLimit = false;
	bool disableThrottleCounter = false;
	bool setServerThrottleLimit = false;
	bool setLastWasStationary = false;
	bool setbCheatFlying = false;
	bool ZeroTimeSinceLastTeleport = false;

	// Movement Numericals
	int currentMovementModeIndex = 0;
	int movementVelocityIndex = 0;
	int globalGameSpeedIndex = 0;
	int walkSpeedIndex = 0;
	int climbingSpeedIndex = 0;
	int glidingSpeedIndex = 0;
	int glidingFallSpeedIndex = 0;
	int jumpVelocityIndex = 0;
	int maxStepHeightIndex = 0;
	int maxStepHeightOutOfWaterIndex = 0;
	int gravityScaleIndex = 0;

	float velocitySpeedMultiplier = 0.0f;
	float GlobalGameSpeed = 1.0f;
	float NoClipFlySpeed = 600.0f;

	// Fishing Booleans
	bool bEnableAutoFishing = false;
	bool bRequireClickFishing = true;
	bool bEnableInstantFishing = false;
	bool bPerfectCatch = true;
	bool bNoRodDurabilityLoss = true;
	bool bDoInstantSellFish = false;
	bool bDestroyCustomizationFishing = false;

	// Fishing Numericals
	float StartRodHealth = 100.0f;
	float EndRodHealth = 100.0f;
	float StartFishHealth = 100.0f;
	float EndFishHealth = 100.0f;

	// Fishing Captures
	FName sOverrideFishingSpot;
	UValeriaWaterBodyComponent* fWaterBody = (UValeriaWaterBodyComponent*)malloc(sizeof(UValeriaWaterBodyComponent));
	bool bCaptureFishingSpot = false;
	bool bOverrideFishingSpot = false;

	// Item Booleans
	bool bEasyModeActive = false;
	bool bSkipGiftingTimers = false;
	bool bEnableMagicArrow = false;
	bool bModifyArrowGravity = false;
	bool bEnableAutoGathering = false;
	bool bEnablePlantAllSeeds = false;
	bool bEnablePremiumUnlocks = false;
	bool bEnableLootbagTeleportation = false;
	bool bEnableLootbagCulling = true;

	// Item Numericals
	int LootbagCullDistance = 60;
	float MagicArrowFOV = 30.0;
	float ArrowSpeed = 2000.0f;
	float ArrowGravityScale = 0.0f;

	// Housing Booleans
	bool bPlaceAnywhere = false;
	bool bDisableCollisionWhenPlacing = false;

	// Quicksell Hotkeys
	bool bEnableQuicksellHotkeys = false;

	// Character Customiztion Booleans
	bool bTempUnlockAllEntitlements = false;
};