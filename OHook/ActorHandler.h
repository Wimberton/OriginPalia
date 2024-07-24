#pragma once

#include <queue>
#include <map>
#include <set>
#include <tuple>
#include <SDK.hpp>
#include <mutex>

enum class EType : uint16_t {
    Unknown,
    Tree            = 1 << 0,
    Ore             = 1 << 1,
    Bug             = 1 << 2,
    Animal          = 1 << 3,
    Forage          = 1 << 4,
    Fish            = 1 << 5,
    Loot            = 1 << 6,
    Players         = 1 << 7,
    NPCs            = 1 << 8,
    Quest           = 1 << 9,
    RummagePiles    = 1 << 10,
    Stables         = 1 << 11,
    Gates           = 1 << 12,
    Treasure        = 1 << 13,
    TimedDrop       = 1 << 14,
    MAX             = 1 << 15,
    MoveablePawn    = Bug | Animal | Fish | Players | NPCs,
};
UE_ENUM_OPERATORS(EType)

struct FEntry {
    SDK::AActor* Actor;
    SDK::FVector WorldPosition;
    std::wstring DisplayName;
    EType ActorType;
    uint8_t Type;
    uint8_t Quality;
    uint8_t Variant;
    double Distance;
    std::string TexturePath;
};

enum class ECreatureKind : uint8_t {
    Unknown,
    Chapaa,
    Cearnuk,
    TreeClimber,
    MAX
};

enum class ECreatureQuality : uint8_t {
    Unknown,
    Tier1,
    Tier2,
    Tier3,
    Chase,
    MAX
};

enum class EBugKind : uint8_t {
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

enum class EBugQuality : uint8_t {
    Unknown,
    Common,
    Uncommon,
    Rare,
    Rare2,
    Epic,
    MAX
};

enum class EGatherableSize : uint8_t {
    Unknown,
    Small,
    Medium,
    Large,
    Bush,
    MAX
};

enum class ETreeType : uint8_t {
    Unknown,
    Flow,
    Heartwood,
    Sapwood,
    Bush,
    MAX
};

enum class EOreType : uint8_t {
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

enum class EFishType : uint8_t {
    Unknown,
    Node,
    Hook,
    MAX
};

enum class EForageableType : uint8_t {
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

enum class EOneOffs : uint8_t {
    Player,
    NPC,
    FishHook,
    FishPool,
    Loot,
    Quest,
    RummagePiles,
    Stables,
    Others,
    Treasure,
    TimedDrop,
    MAX
};

inline std::map<std::string, std::string> CLASS_NAME_ALIAS = {
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
    {"BP_FishingJackpot_C", "Waterlogged Chest"},
    {"BP_FishingRecipeBook_C", "Recipe Book"},
    {"BP_Fish_AlbinoEeel_C", "Albino Eeel"},
    {"BP_Fish_Alligator_Gar_C", "Alligator Gar"},
    {"BP_Fish_AncientFish_C", "Ancient Fish"},
    {"BP_Fish_BahariBream_C", "Bahari Bream"},
    {"BP_Fish_Bahari_Pike_C", "Bahari Pike"},
    {"BP_Fish_Barracuda_C", "Barracuda"},
    {"BP_Fish_BatRay_C", "Bat Ray"},
    {"BP_Fish_Beluga_Sturgeon_C", "Beluga Sturgeon"},
    {"BP_Fish_Bigeye_Tuna_C", "Bigeye_Tuna"},
    {"BP_Fish_Black_Sea_Bass_C", "Black Sea Bass"},
    {"BP_Fish_Blobfish_C", "Blobfish"},
    {"BP_Fish_BlueMarlin_C", "Blue Marlin"},
    {"BP_Fish_Blue_Spotted_Ray_C", "Blue Spotted Ray"},
    {"BP_Fish_Cactus_Moray_C", "Cactus Moray"},
    {"BP_Fish_CalicoKoi_C", "Calico Koi"},
    {"BP_Fish_CantankerousKoi_C", "CantankerousKoi"},
    {"BP_Fish_Channel_Catfish_C", "Channel Catfish"},
    {"BP_Fish_Chub_C", "Chub"},
    {"BP_Fish_Cloudfish_C", "Cloudfish"},
    {"BP_Fish_Codfish_C", "Codfish"},
    {"BP_Fish_Crimson_Fangtooth_C", "Crimson Fangtooth"},
    {"BP_Fish_Crucian_Carp_C", "Crucian Carp"},
    {"BP_Fish_Cutthroat_Trout_C", "Cutthroat Trout"},
    {"BP_Fish_Dawnray_C", "Dawnray"},
    {"BP_Fish_Duskray_C", "Duskray"},
    {"BP_Fish_Enchanted_Pupfish_C", "Enchanted Pupfish"},
    {"BP_Fish_Energized_Piranha_C", "Energized Piranha"},
    {"BP_Fish_EyelessMinnow_C", "Eyeless Minnow"},
    {"BP_Fish_FairyCarp_C", "Fairy Carp"},
    {"BP_Fish_Fathead_Minnow_C", "Fathead Minnow"},
    {"BP_Fish_Flametongue_Ray_C", "Flametongue Ray"},
    {"BP_Fish_FreshwaterEel_C", "Freshwater Eel"},
    {"BP_Fish_Giant_Goldfish_C", "Giant Goldfish"},
    {"BP_Fish_Giant_Kilima_Stingray_C", "Giant Kilima Stingray"},
    {"BP_Fish_Gillyfin_C", "Gillyfin"},
    {"BP_Fish_GoldenSalmon_C", "Golden Salmon"},
    {"BP_Fish_Honey_Loach_C", "Honey Loach"},
    {"BP_Fish_Hypnotic_Moray_C", "Hypnotic Moray"},
    {"BP_Fish_Indigo_Lamprey_C", "Indigo Lamprey"},
    {"BP_Fish_Kenjis_Carp_C", "Kenli's Carp"},
    {"BP_Fish_KilimaCatFish_C", "Kilima Catfish"},
    {"BP_Fish_Kilima_Grayling_C", "Kilima Grayling"},
    {"BP_Fish_Kilima_Redfin_C", "Kilima Redfin"},
    {"BP_Fish_LargemouthBass_C", "Largemouth Bass"},
    {"BP_Fish_Midnight_Paddlefish_C", "Midnight Paddlefish"},
    {"BP_Fish_Mirror_Carp_C", "Mirror Carp"},
    {"BP_Fish_Mottled_Gobi_C", "Mottled Gobi"},
    {"BP_Fish_Mudminnow_C", "Mudminnow"},
    {"BP_Fish_MutatedAngler_C", "Mutated Angler"},
    {"BP_Fish_Oily_Anchovy_C", "Oily Anchovy"},
    {"BP_Fish_Orange_Bluegill_C", "Orange Bluegill"},
    {"BP_Fish_Paddlefish_C", "Paddlefish"},
    {"BP_Fish_PaintedPerch_C", "Painted Perch"},
    {"BP_Fish_Platinum_Chad_C", "Platinum Chad"},
    {"BP_Fish_PrismTrout_C", "Prism Trout"},
    {"BP_Fish_RadiantSunfish_C", "Radiant Sunfish"},
    {"BP_Fish_Rainbow_Trout_C", "Rainbow Trout"},
    {"BP_Fish_Red-bellied_piranha_C", "Red-bellied Piranha"},
    {"BP_Fish_RibbontailRay_C", "Ribbontail Ray"},
    {"BP_Fish_Rosy_Bitterling_C", "Rosy Bitterling"},
    {"BP_Fish_Sardine_C", "Sardine"},
    {"BP_Fish_Scarlet_Koi_C", "Scarlet Koi"},
    {"BP_Fish_Shimmerfin_C", "Shimmerfin"},
    {"BP_Fish_SilverSalmon_C", "Silver Salmon"},
    {"BP_Fish_Silvery_Minnow_C", "Silvery Minnow"},
    {"BP_Fish_SmallmouthBass_C", "Smallmouth Bass"},
    {"BP_Fish_SpottedBullhead_C", "Spotted Bullhead"},
    {"BP_Fish_Stalking_Catfish_C", "Stalking Catfish"},
    {"BP_Fish_Stickleback_C", "Stickleback"},
    {"BP_Fish_Stonefish_C", "Stonefish"},
    {"BP_Fish_Stormray_C", "Stormray"},
    {"BP_Fish_StripedSturgeon_C", "Striped Sturgeon"},
    {"BP_Fish_Striped_Dace_C", "Striped Dace"},
    {"BP_Fish_Swordfin_Eel_C", "Swordfin Eel"},
    {"BP_Fish_Thundering_Eel_C", "Thundering Eel"},
    {"BP_Fish_UmbranCarp_C", "Umbran Carp"},
    {"BP_Fish_UnicornFish_C", "Unicorn Fish"},
    {"BP_Fish_Voidray_C", "Voidray"},
    {"BP_Fish_Willow_Lamprey_C", "Willow Lamprey"},
    {"BP_Fish_Yellowfin_Tuna_C", "Yellowfin Tuna"},
    {"BP_Fish_Yellow_Perch_C", "Yellow Perch"},
    {"BP_Trash_Shipfragments_C", "Ship Fragments"},
    {"BP_Trash_Wagonwheel_C", "Wagon Wheel"},
    {"BP_Trash_WaterloggedBoot_C", "Waterlogged Boot"},

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

    // Stables
    {"BP_Stables_Sign_C", "Stables - Fast Travel"},

    // TimedDrop
    {"BP_TimedDropDecoration_YoungSilverwingNest_C","Young Silverwing Nest"},
    {"BP_TimedDropDecoration_GaldurSeedMaker_C","Ancient Galdur Statue"},
    {"BP_TimedDropDecoration_KitsuuRockGarden_C","Ancient Rock Garden"},
};

// Search map for assigning gatherable size
inline std::map<EGatherableSize, std::vector<std::string>> GATHERABLE_SIZE_MAPPINGS = {
    {EGatherableSize::Small, {"_Small_", "_Sapling_"}},
    {EGatherableSize::Medium, {"_Medium_"}},
    {EGatherableSize::Large, {"_Large_", "_Large2_"}},
    {EGatherableSize::Bush, {"_Bush_"}},
};

// Search map for assigning tree type, assigned by loot type vs tree species
inline std::map<ETreeType, std::vector<std::string>> TREE_TYPE_MAPPINGS = {
    {ETreeType::Flow, {"_CoOp_"}},
    {ETreeType::Heartwood, {"_Pine_"}},
    {ETreeType::Sapwood, {"_Juniper_", "_Oak_", "_Birch_"}},
    {ETreeType::Bush, {"_Bush_"}}
};

// Search map for assigning gatherable flags
inline std::map<EOreType, std::vector<std::string>> MINING_TYPE_MAPPINGS = {
    {EOreType::Stone, {"_Stone_"}},
    {EOreType::Copper, {"_Copper_"}},
    {EOreType::Clay, {"_Clay_"}},
    {EOreType::Iron, {"_Iron_"}},
    {EOreType::Silver, {"_Silver_"}},
    {EOreType::Gold, {"_Gold_"}},
    {EOreType::Palium, {"_Palium_"}}
};

// Search map for assigning forageable type
inline std::map<EForageableType, std::vector<std::string>> FORAGEABLE_TYPE_MAPPINGS = {
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
inline std::map<ECreatureKind, std::vector<std::string>> CREATURE_KIND_MAPPINGS = {
    {ECreatureKind::Chapaa, {"_Chapaa"}},
    {ECreatureKind::Cearnuk, {"_Cearnuk_"}},
    {ECreatureKind::TreeClimber, {"_TreeClimber"}}
};

// Search map for assiging creature quality type
inline std::map<ECreatureQuality, std::vector<std::string>> CREATURE_KINDQUALITY_MAPPINGS = {
    {ECreatureQuality::Tier1, {"_T1_C"}},
    {ECreatureQuality::Tier2, {"_T2_C"}},
    {ECreatureQuality::Tier3, {"_T3_C"}},
    {ECreatureQuality::Chase, {"_Base_C", "_Fast_C"}}
};

// Search map for assigning bug kind type
inline std::map<EBugKind, std::vector<std::string>> CREATURE_BUGKIND_MAPPINGS = {
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
inline std::map<EBugQuality, std::vector<std::string>> CREATURE_BUGQUALITY_MAPPINGS = {
    {EBugQuality::Common, {"C_C", "C+_C"}},
    {EBugQuality::Uncommon, {"U_C", "U+_C"}},
    {EBugQuality::Rare, {"R_C", "R+_C", "R1_C", "R1+_C"}},
    {EBugQuality::Rare2, {"R2_C", "R2+_C"}},
    {EBugQuality::Epic, {"E_C", "E+_C"}}
};

// Search map for assigning fish kind type
inline std::map<EFishType, std::vector<std::string>> FISH_TYPE_MAPPINGS = {
    {EFishType::Node, {"_WaterPlane_"}},
    {EFishType::Hook, {"_Fish_", "_Trash_", "_Fishing"}},
};

// Villager Textures
inline std::map<std::string, std::string> VillagerTextures = {
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
inline std::map<std::string, std::string> TreasureTextures = {
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

inline std::string ForageableTextures[static_cast<int>(EForageableType::MAX)] = {
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

inline std::string OreTextures[static_cast<int>(EOreType::MAX)] = {
    "", // Unknown
    "/Game/UI/Icons/Icon_Stone.Icon_Stone", // Stone
    "/Game/UI/Icons/Icon_Ore_Copper.Icon_Ore_Copper", // Copper
    "/Game/UI/Icons/Icon_Ore_Clay.Icon_Ore_Clay", // Clay
    "/Game/UI/Icons/Icon_Ore_Iron.Icon_Ore_Iron", // Iron
    "/Game/UI/Icons/Icon_Ore_Silver.Icon_Ore_Silver", // Silver
    "/Game/UI/Icons/Icon_Ore_Gold.Icon_Ore_Gold", // Gold
    "/Game/UI/Icons/Icon_Ore_Palium.Icon_Ore_Palium", // Palium
};

inline std::string AnimalTextures[static_cast<int>(ECreatureKind::MAX)][static_cast<int>(ECreatureQuality::MAX)] = {
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

inline std::string BugTextures[static_cast<int>(EBugKind::MAX)][static_cast<int>(EBugQuality::MAX)] = {
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

inline std::string TreeTextures[static_cast<int>(ETreeType::MAX)] = {
    "", // Unknown
    "/Game/UI/Icons/Icon_Wood_Magicwood.Icon_Wood_Magicwood", // Flow
    "/Game/UI/Icons/Icon_Wood_Hardwood.Icon_Wood_Hardwood", // Heartwood
    "/Game/UI/Icons/Icon_Wood_Softwood.Icon_Wood_Softwood", // Sapwood
    "", // Bush
};

inline std::unordered_map<int32_t, std::pair<std::wstring, std::string>>TimedDropLoot = {
    // GaldurSeedMaker
    {20263, {L"Acorn", "/Game/UI/Icons/Icon_Seed_Acorn.Icon_Seed_Acorn"}},
    {220, {L"Flow Acorn", "/Game/UI/Icons/WT_Icon_Seed_Acorn_CoOp.WT_Icon_Seed_Acorn_CoOp"}},
    {31632, {L"Juniper Seed", "/Game/UI/Icons/Icon_Seed_Juniper.Icon_Seed_Juniper"}},
    {22297, {L"Flow Juniper Seed", "/Game/UI/Icons/WT_Icon_Seed_Juniper_CoOp.WT_Icon_Seed_Juniper_CoOp"}},
    {22034, {L"Pinecone", "/Game/UI/Icons/Icon_Seed_Pinecone.Icon_Seed_Pinecone"}},
    {743, {L"Flow Pinecone", "/Game/UI/Icons/WT_Icon_Seed_Pinecone_CoOp.WT_Icon_Seed_Pinecone_CoOp"}},
    {20333, {L"Samara", "/Game/UI/Icons/Icon_Seed_Samara.Icon_Seed_Samara"}},
    {17757, {L"Flow Samara", "/Game/UI/Icons/WT_Icon_Seed_Samara_CoOp.WT_Icon_Seed_Samara_CoOp"}},

    //YoungSilverwingNest
    {9935, {L"Blue Candy Egg", "/Game/UI/Icons/Icon_SilverwingEggBlue.Icon_SilverwingEggBlue"}},
    {19234, {L"Green Candy Egg", "/Game/UI/Icons/Icon_SilverwingEggGreen.Icon_SilverwingEggGreen"}},
    {5692, {L"Red Candy Egg", "/Game/UI/Icons/Icon_SilverwingEggRed.Icon_SilverwingEggRed"}},
    {4350, {L"Yellow Candy Egg", "/Game/UI/Icons/Icon_SilverwingEggYellow.Icon_SilverwingEggYellow"}},

    //KitsuuRockGarden
    {16272, {L"Spicy Pepper Seed", "/Game/UI/Icons/Icon_Seeds_Peppers.Icon_Seeds_Peppers"}},
    {30124, {L"Gold Ore", "/Game/UI/Icons/Icon_Ore_Gold.Icon_Ore_Gold"}},
    {10548, {L"Chapaa Meat", "/Game/UI/Icons/Icon_Steak_Chapaa.Icon_Steak_Chapaa"}},
    {29308, {L"Sernuk Meat", "/Game/UI/Icons/Icon_Steak_Cearnuk.Icon_Steak_Cearnuk"}},
    {30911, {L"Fur", "/Game/UI/Icons/Icon_Fur.Icon_Fur"}},
    {28732, {L"Sernuk Hide", "/Game/UI/Icons/Icon_Hide.Icon_Hide"}},
    {7159, {L"Fine Arrow", "/Game/UI/Icons/Icon_Ammo_ArrowIron.Icon_Ammo_ArrowIron"}},
    {31021, {L"Slowdown Arrow", "/Game/UI/Icons/Icon_Ammo_ArrowLethargy.Icon_Ammo_ArrowLethargy"}},
    {1538, {L"Dispel Arrow", "/Game/UI/Icons/Icon_Ammo_ArrowDispel.Icon_Ammo_ArrowDispel"}},
};

inline std::unordered_map<std::string, std::string>TeleporterMap = {
    // The Tower
    {"DA_BlackMarket_toDeveloperChallenge1","Black Market"},
    // Home
    {"DA_AZ1_FromHousingPlot","Bahari Bay"},
    {"DA_VillageRoot_HousingPlots","Kilima"},
    {"DA_HousingPlot_FromGreybox","Housing Plot"},
    // Black Market
    {"DA_VillageRoot_GeneralStore","Kilima - General Store"},
    {"DA_VillageRoot_Lake","Kilima - Crystal Lake"},
    {"DA_PS_RethRoom_2","Kilima - Reth's Storeroom"},
    {"DA_TeleportTravel_DeveloperChallenge1","The Tower"},
    {"DA_BMOfficeAddress","Zeki's Office"},
    // Kilima
    {"DA_HydroStationAddress", "DO NOT USE"},
    {"DA_CaleriRoom_VillageRoot", "Library Restricted Section"},
    {"DA_BlackMarket_toVillageRootLake", "Black Market - Crystal Lake Entrance"},
    {"DA_PS_EsheRoom", "Eshe's Bedroom"},
    {"DA_PS_KenyattaRoom", "Kenyatta's Bedroom"},
    {"DA_ChanesRoom_VillageRoot", "Chayne's Bedroom"},
    {"DA_PS_SubiraInn", "Subira's Room"},
    {"DA_PS_ZekiRoom", "Zeki's Bedroom"},
    {"DA_PS_KenliRecordsRoom", "Kenli's Records Room"},
    {"DA_PS_ElouisaRoom", "Elouisa's Bedroom"},
    {"DA_PS_SifuuRoom", "Sifuu's Bedroom"},
    {"DA_PS_JinaLibrary", "Ancient Library"},
    {"DA_PS_HeklaTent", "Hekla's Tent"},
    {"DA_PS_BadruuBarn", "Badruu's Barn"},
    {"DA_NightSkyTemple_Entrance", "Night Sky Temple"},
    {"DA_PS_AuniTreehouse", "Auni's Treehouse"},
    {"DA_PS_NaioROom", "Nai'o and Auni's Bedroom"},
    {"DA_PS_DelailaRoom", "Dalaila and Badruu's Bedroom"},
    {"DA_PS_EinarCave", "Einar's Cave"},
    {"DA_WaterShrine_Entrance", "Water Shrine"},
    {"DA_VillageRoot_fromCItyHallStoreRoom", "City Hall Storage Room"},
    {"DA_BlackMarket_toGeneralStore", "Black Market - General Store Entrance"},
    {"DA_VillageRoot_toCItyHallStoreRoom", "City Hall Outside Storage Room"},
    {"DA_PS_TishRoom", "Tish's Bedroom"},
    {"DA_PS_RethRoom", "Reth's Storeroom"},
    {"DA_PS_AshuraRoom", "Ashura's Room"},
    {"DA_PS_JelRoom", "Jel's Bedroom"},
    {"DA_VillageRoot_SouthEastGate", "Kilima - Fairground's Entrance"},
    {"DA_FairGrounds_North", "Fairgrounds (Currently Closed)"},
    {"DA_AZ1_FromVillage", "Bahari"},
    {"DA_HousingPlot", "Home"},
    {"DA_BlackMarket_toPSRethStoreroom", "Black Market - Reth's Storeroom Entrance"},
    {"DA_VillageRoot_toPSRethStoreroom", "Kilima - Outside Reth's Storeroom"},
    // Bahari
    {"DA_PS_LighthouseBasement", "Lighthouse Basement"},
    {"DA_CollapsedMineshaft_Interior", "Northern Mineshaft"},
    {"DA_AeroStationAddress", "DO NOT USE"},
    {"DA_PS_TamalaBedroom", "Tamala's Bedroom"},
    {"DA_AZ1toAirTemple", "Air Temple"},
    {"DA_PyroStationAddress", "DO NOT USE"},
    {"DA_EarthTempleAddress", "DO NOT USE"},
    {"DA_PS_HassianGrove", "Hassian's Grove"},
    {"DA_FireTemple_Entrance", "Fire Temple"},
    {"DA_PS_HodariBedroom", "Hodari's Bedroom"},
    {"DA_PS_NajumaBedroom", "Najuma's Bedroom"},
    {"DA_VillageRoot_EastGate", "Kilima"},
    {"DA_HousingPlot_ShortCut", "Home"},
    // Fire Temple
    // Earth Temple
    // Air Temple
};

using namespace SDK;

class ActorHandler
{
public:
    static void SetupListeners();
    static void ProcessActor(AActor*);
    static void ProcessBPActor(AActor*);
    static std::unordered_map<std::string, AActor*>GetTeleporters();
    static bool IsBlueprintClass(UClass*);
    static bool IsActorA(AActor*, std::wstring);
    static void FinishProcessingActor(AActor*, EType);
    static void RemoveActor(AActor*);
    static void UpdateActors(bool);
    static UClass* ClassLookup(std::wstring);
    static void AddClass(UClass*);
    static void RemoveClass(UClass*);
    static UTexture2D* TextureLookup(const std::string& texture);
    static void Tick();
    static std::vector<FEntry>GetActors();
    static std::vector<FEntry>GetActorsOfType(EType);

    static void ClientTravelInternal();
    static void LevelChange(ULevel*);

    static inline bool bListenersAdded = false;
private:

    static inline std::vector<std::pair<std::wstring, EType>>BPActors = {
        {L"/Game/Core/BP_Valeria_Gatherable_Placed.BP_Valeria_Gatherable_Placed", EType::Forage},
        {L"/Game/Core/BP_Loot.BP_Loot", EType::Loot},
        {L"/Game/Gameplay/Architecture/_Generic/Stables/BP_Stables_FrontGate_01.BP_Stables_FrontGate_01", EType::Gates},
        {L"/Game/Gameplay/Architecture/_Generic/Stables/BP_Stables_FrontGate_02.BP_Stables_FrontGate_02", EType::Gates},
        {L"/Game/Gameplay/Stables/BP_Stables_Sign.BP_Stables_Sign", EType::Stables},
        {L"/Game/Gameplay/QuestItems/baseClass/BP_QuestItem_BASE.BP_QuestItem_BASE", EType::Quest},
        {L"/Game/Gameplay/Inspectables/Base/BP_SimpleInspect_Base.BP_SimpleInspect_Base", EType::Quest},
        {L"/Game/Gameplay/Inspectables/Base/BP_QuestInspect_Base.BP_QuestInspect_Base", EType::Quest},
        {L"/Game/Gameplay/Housing/Decorations/TimedDropDecorations/BP_TimedDropDecoration_Base.BP_TimedDropDecoration_Base", EType::TimedDrop},
        {L"/Game/Gameplay/Skills/Fishing/Shared/BP_Minigame_Fish.BP_Minigame_Fish", EType::Fish},
        {L"/Game/Environment/Water/BP_WaterPlane_Fishing_Base_SQ.BP_WaterPlane_Fishing_Base_SQ", EType::Fish},
    };

    //static inline std::unordered_map<std::string, UTexture2D*>TexturePtrMap = {};
    static inline std::unordered_map<std::wstring, UClass*>ClassPtrMap = {};
    //static inline std::deque<std::pair<AActor*, EType>>ActorQueue = {};
    //static inline std::deque<AActor*>BPActorQueue = {};
    static inline std::vector<FEntry>ActorCache = {};
    static inline bool bIsTraveling = false;
    static inline double LastUpdate = 0;
    static inline double LastDormantUpdate = 0;
    static inline std::vector<AActor*>Teleporters = {};

    static inline std::mutex ActorCacheMutex;

public:

};

