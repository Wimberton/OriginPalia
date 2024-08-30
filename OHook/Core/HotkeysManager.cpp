#include "Core/HotkeysManager.h"
#include "Core/Configuration.h"
#include "Core/DetourManager.h"
#include "Core/Handlers/WidgetHandler.h"
#include "Overlay/PaliaOverlay.h"
#include "Detours/Main/MovementDetours.h"
#include "Detours/Main/FishingDetours.h"
#include "Detours/Main/TeleportDetours.h"

#include "menu/menu.hpp"

#include "Misc/ImGuiExt.h"
#include "Misc/Utils.h"

#include <unordered_map>
#include <functional>
#include <Windows.h>
#include <string>
#include <unordered_set>
#include <set>

std::unordered_map<std::string, std::pair<int, std::function<void()>>> HotkeysManager::hotkeys;
std::unordered_set<int> HotkeysManager::pressedKeys;
std::set<int> HotkeysManager::disallowedKeys = { 0x01, 0x02, 0x1B, 0x2D, 0x41, 0x44, 0x53, 0x57 }; // Initialize the disallowed keys

#define REGISTER_HOTKEY(actionName, callback, save_file_type)   \
    RegisterHotkey(actionName, GetHotkey(actionName), [=]() {   \
        callback();                                             \
        Configuration::Save(save_file_type);                    \
    });

void HotkeysManager::InitHotkeys() {
    // =========================================
    // Checkboxes
    // ESP settings
    #pragma region ESPSettings
    REGISTER_HOTKEY("EnableESP", [] {
        Configuration::bEnableESP = !Configuration::bEnableESP;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "ESP toggled %s.", (Configuration::bEnableESP ? "ON" : "OFF")});
    }, ESaveFile::ESPSettings)
    REGISTER_HOTKEY("ShowDistance", [] {
        Configuration::bEnableESPDistance = !Configuration::bEnableESPDistance;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Show Distance toggled %s.", (Configuration::bEnableESPDistance ? "ON" : "OFF")});
    }, ESaveFile::ESPSettings)
    REGISTER_HOTKEY("ShowDespawningTimers", [] {
        Configuration::bEnableESPDespawnTimer = !Configuration::bEnableESPDespawnTimer;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Show Despawn Timers toggled %s.", (Configuration::bEnableESPDespawnTimer ? "ON" : "OFF")});
    }, ESaveFile::ESPSettings)
    REGISTER_HOTKEY("ShowESPInWorld-Map", [] {
        Configuration::bEnableMapESP = !Configuration::bEnableMapESP;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Show ESP In World-Map toggled %s.", (Configuration::bEnableMapESP ? "ON" : "OFF")});
    }, ESaveFile::ESPSettings)
    REGISTER_HOTKEY("UseNativeIcons", [] {
        Configuration::bEnableESPIcons = !Configuration::bEnableESPIcons;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Use Native Icons toggled %s.", (Configuration::bEnableESPIcons ? "ON" : "OFF")});
    }, ESaveFile::ESPSettings)
    #pragma endregion
    
    // Aim settings
    #pragma region AimSettings
    REGISTER_HOTKEY("EnableInteliAim", [] {
        Configuration::bEnableInteliAim = !Configuration::bEnableInteliAim;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Inteli Aim toggled %s.", (Configuration::bEnableInteliAim ? "ON" : "OFF")});
    }, ESaveFile::AimSettings)
    REGISTER_HOTKEY("HideCircle", [] {
        Configuration::bHideFOVCircle = !Configuration::bHideFOVCircle;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Hide Circle toggled %s.", (Configuration::bHideFOVCircle ? "ON" : "OFF")});
    }, ESaveFile::AimSettings)
    REGISTER_HOTKEY("ShowCrosshair", [] {
        Configuration::bDrawCrosshair = !Configuration::bDrawCrosshair;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Show Crosshair toggled %s.", (Configuration::bDrawCrosshair ? "ON" : "OFF")});
    }, ESaveFile::AimSettings)
    REGISTER_HOTKEY("EnableSilentAimbot", [] {
        Configuration::bEnableSilentAimbot = !Configuration::bEnableSilentAimbot;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Silent Aimbot toggled %s.", (Configuration::bEnableSilentAimbot ? "ON" : "OFF")});
    }, ESaveFile::AimSettings)
    REGISTER_HOTKEY("EnableTeleportingToTarget", [] {
        Configuration::bTeleportToTargeted = !Configuration::bTeleportToTargeted;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Teleport To Targeted toggled %s.", (Configuration::bTeleportToTargeted ? "ON" : "OFF")});
    }, ESaveFile::AimSettings)
    REGISTER_HOTKEY("AvoidTeleportingToPlayers", [] {
        Configuration::bAvoidTeleportingToPlayers = !Configuration::bAvoidTeleportingToPlayers;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Avoid Teleporting To Players toggled %s.", (Configuration::bAvoidTeleportingToPlayers ? "ON" : "OFF")});
    }, ESaveFile::AimSettings)
    REGISTER_HOTKEY("AvoidTeleportingToTargetWhenPlayersAreNear", [] {
        Configuration::bDoRadiusPlayersAvoidance = !Configuration::bDoRadiusPlayersAvoidance;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Avoid Teleporting To Target When Players Are Near toggled %s.", (Configuration::bDoRadiusPlayersAvoidance ? "ON" : "OFF")});
    }, ESaveFile::AimSettings)
    #pragma endregion
    
    // Movement settings
    #pragma region MovementSettings
    REGISTER_HOTKEY("EnableNoclip", [] {
        MovementDetours::bEnableNoclip = !MovementDetours::bEnableNoclip;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "NoClip toggled %s.", (MovementDetours::bEnableNoclip ? "ON" : "OFF")});
    }, ESaveFile::MovementSettings)
    #pragma endregion
    
    // ToolSkills settings
    #pragma region ToolSkillsSettings
    REGISTER_HOTKEY("AutoSwingTool", [] {
        Configuration::bEnableAutoSwinging = !Configuration::bEnableAutoSwinging;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Auto Swing Tool toggled %s.", (Configuration::bEnableAutoSwinging ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("AutoEquipTool", [] {
        Configuration::bResourceAutoEquipTool = !Configuration::bResourceAutoEquipTool;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Auto Equip Tool toggled %s.", (Configuration::bResourceAutoEquipTool ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("AvoidChoppingGrove", [] {
        Configuration::bAvoidGroveChopping = !Configuration::bAvoidGroveChopping;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Avoid Chopping Grove toggled %s.", (Configuration::bAvoidGroveChopping ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("AutoCatchBugs", [] {
        Configuration::bEnableBugCatching = !Configuration::bEnableBugCatching;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Auto Catch Bugs toggled %s.", (Configuration::bEnableBugCatching ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("AutoEquipSmokeBombs", [] {
        Configuration::bBugAutoEquipTool = !Configuration::bBugAutoEquipTool;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Auto Equip Smoke Bombs toggled %s.", (Configuration::bBugAutoEquipTool ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("RandomCatchingDelay", [] {
        Configuration::BugCatchingDelay = !Configuration::BugCatchingDelay;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Random Catching Delay toggled %s.", (Configuration::BugCatchingDelay ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("AutoHuntAnimals", [] {
        Configuration::bEnableAnimalHunting = !Configuration::bEnableAnimalHunting;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Auto Hunt Animals toggled %s.", (Configuration::bEnableAnimalHunting ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("AutoEquipBow", [] {
        Configuration::bAnimalAutoEquipTool = !Configuration::bAnimalAutoEquipTool;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Auto Equip Bow toggled %s.", (Configuration::bAnimalAutoEquipTool ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("RandomHuntingDelay", [] {
        Configuration::AnimalHuntingDelay = !Configuration::AnimalHuntingDelay;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Random Hunting Delay toggled %s.", (Configuration::AnimalHuntingDelay ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("AutoGatherNearbyLoot", [] {
        Configuration::bEnableAutoGather = !Configuration::bEnableAutoGather;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Auto Gather Nearby Loot toggled %s.", (Configuration::bEnableAutoGather ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("SkipCookingMinigames", [] {
        Configuration::bEnableCookingMinigameSkip = !Configuration::bEnableCookingMinigameSkip;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Skip Cooking Minigames toggled %s.", (Configuration::bEnableCookingMinigameSkip ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("NoRodDurabilityLoss", [] {
        Configuration::bFishingNoDurability = !Configuration::bFishingNoDurability;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "No Rod Durability Loss toggled %s.", (Configuration::bFishingNoDurability ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("EnableMultiplayerHelp", [] {
        Configuration::bFishingMultiplayerHelp = !Configuration::bFishingMultiplayerHelp;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Enable Multiplayer Help toggled %s.", (Configuration::bFishingMultiplayerHelp ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("AlwaysPerfectCatch", [] {
        Configuration::bFishingPerfectCatch = !Configuration::bFishingPerfectCatch;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Always Perfect Catch toggled %s.", (Configuration::bFishingPerfectCatch ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("InstantCatch", [] {
        Configuration::bFishingInstantCatch = !Configuration::bFishingInstantCatch;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Instant Catch toggled %s.", (Configuration::bFishingInstantCatch ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("DiscardAllJunk", [] {
        Configuration::bFishingDiscard = !Configuration::bFishingDiscard;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Discard All Junk toggled %s.", (Configuration::bFishingDiscard ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("OpenandStoreMakeshiftDecor", [] {
        Configuration::bFishingOpenStoreWaterlogged = !Configuration::bFishingOpenStoreWaterlogged;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Open and Store Makeshift Decor toggled %s.", (Configuration::bFishingOpenStoreWaterlogged ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("AutoFastFishing", [] {
        FishingDetours::bEnableAutoFishing = !FishingDetours::bEnableAutoFishing;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Auto Fast Fishing toggled %s.", (FishingDetours::bEnableAutoFishing ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("RequireHoldingLeft-ClickToAutoFish", [] {
        Configuration::bRequireClickFishing = !Configuration::bRequireClickFishing;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Require Holding Left-Click to Auto Fish toggled %s.", (Configuration::bRequireClickFishing ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    REGISTER_HOTKEY("ForceFishingPool", [] {
        FishingDetours::bOverrideFishingSpot = !FishingDetours::bOverrideFishingSpot;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Force Fishing Pool toggled %s.", (FishingDetours::bOverrideFishingSpot ? "ON" : "OFF")});
    }, ESaveFile::ToolSkillsSettings)
    #pragma endregion ToolSkillsSettings
    
    // SellItems settings
    #pragma region SellItemsSettings
    REGISTER_HOTKEY("ItemBuyMultiplier", [] {
        Configuration::bEnableBuyMultiplier = !Configuration::bEnableBuyMultiplier;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Item Buy Multiplier toggled %s.", (Configuration::bEnableBuyMultiplier ? "ON" : "OFF")});
    }, ESaveFile::SellItemsSettings)
    REGISTER_HOTKEY("EnableCTRL-ClicktoBuyModded", [] {
        Configuration::bEnableCtrlClickBuy = !Configuration::bEnableCtrlClickBuy;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Enable CTRL-Click to Buy Modded toggled %s.", (Configuration::bEnableCtrlClickBuy ? "ON" : "OFF")});
    }, ESaveFile::SellItemsSettings)
    REGISTER_HOTKEY("OpenPlayerStorage", [] {
        WidgetHandler::GameplayUIManager->Implementation_OpenPlayerStorage();
    }, ESaveFile::NONE)
    #pragma endregion
    
    // Mod settings TAB
    #pragma region ModSettings
    REGISTER_HOTKEY("AntiAFK", [] {
        Configuration::bEnableAntiAfk = !Configuration::bEnableAntiAfk;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "AntiAFK toggled %s.", (Configuration::bEnableAntiAfk ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("UnlimitedWardrobeRespecs", [] {
        Configuration::bEnableUnlimitedWardrobeRespec = !Configuration::bEnableUnlimitedWardrobeRespec;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Unlimited Wardrobe Respecs toggled %s.", (Configuration::bEnableUnlimitedWardrobeRespec ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("FurtherInteractionDistance", [] {
        Configuration::bEnableInteractionMods = !Configuration::bEnableInteractionMods;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Further Interaction Distance toggled %s.", (Configuration::bEnableInteractionMods ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("TeleportToWaypoints", [] {
        Configuration::bEnableWaypointTeleport = !Configuration::bEnableWaypointTeleport;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Teleport To Waypoints toggled %s.", (Configuration::bEnableWaypointTeleport ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("EasyQuestMode", [] {
        Configuration::bEasyModeActive = !Configuration::bEasyModeActive;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Easy Quest Mode toggled %s.", (Configuration::bEasyModeActive ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("ShowWatermark", [] {
        Configuration::bShowWatermark = !Configuration::bShowWatermark;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Show Watermark toggled %s.", (Configuration::bShowWatermark ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("ActiveModsWindow", [] {
        Configuration::showEnabledFeaturesWindow = !Configuration::showEnabledFeaturesWindow;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Active Mods Window toggled %s.", (Configuration::showEnabledFeaturesWindow ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("AdvancedPlacement", [] {
        Configuration::bPlaceAnywhere = !Configuration::bPlaceAnywhere;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Advanced Placement toggled %s.", (Configuration::bPlaceAnywhere ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("TemporarilyUnlockOutfits", [] {
        Configuration::bEnableOutfitCustomization = !Configuration::bEnableOutfitCustomization;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Temporarily Unlock Outfits toggled %s.", (Configuration::bEnableOutfitCustomization ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("UnlockLandscapes", [] {
        Configuration::bEnableFreeLandscape = !Configuration::bEnableFreeLandscape;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Unlock Landscapes toggled %s.", (Configuration::bEnableFreeLandscape ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("ShowAllVillagerWeeklyGifts", [] {
        Configuration::bEnableShowWeeklyWants = !Configuration::bEnableShowWeeklyWants;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Show All Villager Weekly Gifts toggled %s.", (Configuration::bEnableShowWeeklyWants ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("UnrepairToolForGold", [] {
        Configuration::bEnableToolBricker = !Configuration::bEnableToolBricker;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Unrepair Tool For Gold toggled %s.", (Configuration::bEnableToolBricker ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("FulfillInfinitePlayerRequests", [] {
        Configuration::bEnableRequestMods = !Configuration::bEnableRequestMods;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Fulfill Infinite Player Requests toggled %s.", (Configuration::bEnableRequestMods ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("BlackMarketPrizeWheel", [] {
        Configuration::bEnablePrizeWheel = !Configuration::bEnablePrizeWheel;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Black Market Prize Wheel mod toggled %s.", (Configuration::bEnablePrizeWheel ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("UseLockedStorage", [] {
        Configuration::bPrizeWheelUseLocked = !Configuration::bPrizeWheelUseLocked;
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Use Locked Storage toggled %s.", (Configuration::bPrizeWheelUseLocked ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("0|Red-Fireworks", [] {
        Configuration::PrizeWheelSlots[0] = !Configuration::PrizeWheelSlots[0];
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Fireworks Prize Slot toggled %s.", (Configuration::PrizeWheelSlots[0] ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("1|Purple-Decor", [] {
        Configuration::PrizeWheelSlots[1] = !Configuration::PrizeWheelSlots[1];
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Decor Prize Slot toggled %s.", (Configuration::PrizeWheelSlots[1] ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("2|Blue-Ingredients", [] {
        Configuration::PrizeWheelSlots[2] = !Configuration::PrizeWheelSlots[2];
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Ingredients Prize Slot toggled %s.", (Configuration::PrizeWheelSlots[2] ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("3|Green-Gold", [] {
        Configuration::PrizeWheelSlots[3] = !Configuration::PrizeWheelSlots[3];
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Gold Prize Slot toggled %s.", (Configuration::PrizeWheelSlots[3] ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("4|Red-Fireworks", [] {
        Configuration::PrizeWheelSlots[4] = !Configuration::PrizeWheelSlots[4];
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Fireworks Prize Slot toggled %s.", (Configuration::PrizeWheelSlots[4] ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("5|Brown-LuckyCoins", [] {
        Configuration::PrizeWheelSlots[5] = !Configuration::PrizeWheelSlots[5];
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Lucky Coins Prize Slot toggled %s.", (Configuration::PrizeWheelSlots[5] ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("6|Blue-Ingredients", [] {
        Configuration::PrizeWheelSlots[6] = !Configuration::PrizeWheelSlots[6];
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Ingredients Prize Slot toggled %s.", (Configuration::PrizeWheelSlots[6] ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("7|Green-Gold", [] {
        Configuration::PrizeWheelSlots[7] = !Configuration::PrizeWheelSlots[7];
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Gold Prize Slot toggled %s.", (Configuration::PrizeWheelSlots[7] ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    REGISTER_HOTKEY("8|Yellow-Plushie", [] {
        Configuration::PrizeWheelSlots[8] = !Configuration::PrizeWheelSlots[8];
        if (!Configuration::showEnabledFeaturesWindow)
            ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Plushie Prize Slot toggled %s.", (Configuration::PrizeWheelSlots[8] ? "ON" : "OFF")});
    }, ESaveFile::ModSettings)
    #pragma endregion
    
    // =========================================
    // Buttons
    //OpenGuildStore -- not sure how to implement here.
    //OpenBuildingStore -- not sure how to implement here.
}

void HotkeysManager::RegisterHotkey(const std::string& actionName, int key, const std::function<void()>& callback) {
    hotkeys[actionName] = { key, callback };
}

void HotkeysManager::CheckHotkeys(const UObject* Context, UFunction* Function, void* Parms) {
    if (ImGui::isHotkeySelectorPopupOpen)
        return;

    for (auto& [actionName, keyCallbackPair] : hotkeys) {
        int key = keyCallbackPair.first;
        auto& callback = keyCallbackPair.second;
        if (GetAsyncKeyState(key) & 0x8000 && !pressedKeys.contains(key)) {
            if (IsGameWindowActive() || !Menu::bShowMenu) {
                if (callback) callback();
                pressedKeys.insert(key);
            }
        }
        if (!(GetAsyncKeyState(key) & 0x8000)) {
            pressedKeys.erase(key);
        }
    }
}

void HotkeysManager::SetHotkey(const std::string& actionName, int key) {
    // Allow setting multiple hotkeys to 0 (which represents "None")
    if (key == 0) {
        hotkeys[actionName] = { 0, hotkeys[actionName].second };
        return;
    }

    // Check if the key is disallowed
    if (disallowedKeys.contains(key)) {
        return; // Key is disallowed, do not set it
    }

    // Check if the key is already in use by another action (except when setting to 0)
    if (hotkeys.contains(actionName)) {
        for (const auto& [otherActionName, keyCallbackPair] : hotkeys) {
            if (otherActionName != actionName && keyCallbackPair.first == key) {
                return; // Handle the case where the key is already in use
            }
        }
    }

    // Set and register the hotkey
    hotkeys[actionName] = { key, hotkeys[actionName].second };
    RegisterHotkey(actionName, key, hotkeys[actionName].second);
}


int HotkeysManager::GetHotkey(const std::string& actionName) {
    if (hotkeys.contains(actionName)) {
        return hotkeys[actionName].first;
    }
    return 0;
}

const std::unordered_map<std::string, std::pair<int, std::function<void()>>>& HotkeysManager::GetHotkeys() {
    return hotkeys;
}

const std::set<int>& HotkeysManager::GetDisallowedKeys() {
    return disallowedKeys;
}

void HotkeysManager::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
    DetourManager::AddInstance(VC);
	DetourManager::AddFunctionListener("Function Engine.Actor.ReceiveTick", &CheckHotkeys, nullptr);
}