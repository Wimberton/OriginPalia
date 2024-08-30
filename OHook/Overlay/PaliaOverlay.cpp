#include "Overlay/PaliaOverlay.h"
#include "Core/Configuration.h"
#include "Core/HotkeysManager.h"
#include "Core/Handlers/ActorHandler.h"
#include "Core/Handlers/ToolHandler.h"
#include "Core/Handlers/WidgetHandler.h"
#include "Core/Main.h"

#include <SDK/Palia_parameters.hpp>

#include "Misc/ImGuiExt.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"

#include "menu/menu.hpp"
#include "fonts/fonts.h"

#include <algorithm>
#include <imgui_internal.h>
#include <codecvt>

#include "Detours/Main/AimDetours.h"
#include "Detours/Main/StoreDetours.h"
#include "Detours/Main/FishingDetours.h"
#include "Detours/Main/HUDDetours.h"
#include "Detours/Main/MovementDetours.h"
#include "Detours/Main/TeleportDetours.h"

#include "Overlay/Modules/ESP_AnimalSettings.h"
#include "Overlay/Modules/ESP_BugSettings.h"
#include "Overlay/Modules/ESP_ForageableSettings.h"
#include "Overlay/Modules/ESP_OreSettings.h"
#include "Overlay/Modules/ESP_SingleSettings.h"
#include "Overlay/Modules/ESP_TreeSettings.h"
#include "Overlay/Modules/Store_AutoSellSettings.h"

using namespace SDK;

void DisplayModsCategory(const std::string& title, const std::vector<std::pair<std::string, bool>>& mods) {
    bool hasMods = std::ranges::any_of(mods, [](const auto& mod) {
        return mod.second;
    });
    if (hasMods) {
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.5f, 0.9f, 1.0f));
        ImGui::Text("%s", title.c_str());
        ImGui::PopStyleColor();
        ImGui::Spacing();

        for (const auto& mod : mods) {
            if (mod.second) {
                ImGui::BulletText("%s", mod.first.c_str());
            }
        }
    }
}

void DrawEnabledFeatures() {
    if (!Configuration::showEnabledFeaturesWindow)
        return;

    AValeriaCharacter* ValeriaCharacter = GetValeriaCharacter();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize;

    auto initialPos = ImVec2(Configuration::activeWindowPosX, Configuration::activeWindowPosY);

    ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);

    if (ImGui::Begin("Active Mods", &Configuration::showEnabledFeaturesWindow, window_flags)) {
        if (ValeriaCharacter) {
            DisplayModsCategory("Global Mods", {
                                    {"AntiAFK", Configuration::bEnableAntiAfk},
                                    {"Content Unlocker", Configuration::bEnableOutfitCustomization},
                                    {"Cooking Mods", Configuration::bEnableCookingMinigameSkip},
                                    {"Interaction Mods", Configuration::bEnableInteractionMods},
                                    {"Request Mods", Configuration::bEnableRequestMods},
                                    {"PrizeWheel Mods", Configuration::bEnablePrizeWheel}
                                });

            DisplayModsCategory("ESP Mods", {
                                    {"ESP", Configuration::bEnableESP},
                                    {"ESP Icons", Configuration::bEnableESPIcons},
                                    {"InteliAim Circle", Configuration::bEnableInteliAim}
                                });

            DisplayModsCategory("Aiming Mods", {
                                    {"Silent Aimbot", Configuration::bEnableSilentAimbot},
                                });

            DisplayModsCategory("Teleport Mods", {
                                    {"TP to Targeted", Configuration::bTeleportToTargeted},
                                    {"TP to Waypoint", Configuration::bEnableWaypointTeleport},
                                    {"TP Avoid Players", Configuration::bAvoidTeleportingToPlayers},
                                    {"AutoGather", Configuration::bEnableAutoGather}
                                });

            DisplayModsCategory("Fishing Mods", {
                                    {"No Rod Durability", Configuration::bFishingNoDurability},
                                    {"Fishing MP Help", Configuration::bFishingMultiplayerHelp},
                                    {"Perfect Catch", Configuration::bFishingPerfectCatch},
                                    {"Instant Catch", Configuration::bFishingInstantCatch},
                                    {"Sell All Fish", Configuration::bFishingSell},
                                    {"Discard Fishing Junk", Configuration::bFishingDiscard},
                                    {"Open Waterlogged", Configuration::bFishingOpenStoreWaterlogged},
                                    {"Left Click Fishing", Configuration::bRequireClickFishing}
                                });

#ifdef ENABLE_SUPPORTER_FEATURES
            DisplayModsCategory("Housing Mods", {
                                    {"Place Items Anywhere", Configuration::bPlaceAnywhere},
                                    {"Unlock Landscapes", Configuration::bEnableFreeLandscape}
                                });
#else
            DisplayModsCategory("Housing Mods", {
                {"Place Items Anywhere", Configuration::bPlaceAnywhere}
            });
#endif
        } else {
            ImGui::Text("Waiting for in-game...");
        }

        ImVec2 currentPos = ImGui::GetWindowPos();
        if (!AreFloatsEqual(currentPos.x, Configuration::activeWindowPosX) || !AreFloatsEqual(currentPos.y, Configuration::activeWindowPosY)) {
            Configuration::activeWindowPosX = currentPos.x;
            Configuration::activeWindowPosY = currentPos.y;
            Configuration::Save(ESaveFile::OverlaySettings);
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
        ImGui::End();
    }
}

inline void drawTextOutlined(ImDrawList* drawList, ImVec2 pos, ImU32 color, const char* text, const ImFont* font) {
    if (!text || !drawList || !font) 
        return;

    const float fontSize = font->FontSize * Configuration::ESPTextScale;
    constexpr ImVec2 offsets[] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
    constexpr ImU32 outlineColor = IM_COL32(0, 0, 0, 170);

    for (const auto& offset : offsets) {
        drawList->AddText(font, fontSize, CustomMath::addImVec2(pos, offset), outlineColor, text);
    }

    drawList->AddText(font, fontSize, pos, color, text);
}

void PaliaOverlay::DrawGuiFOVCircle() {
    if (!Configuration::bEnableInteliAim || WidgetHandler::IsWidgetOpen())
        return;

    const auto PlayerController = GetPlayerController();
    if (!PlayerController)
        return;

    FVector PawnLocation = PlayerController->K2_GetPawn()->K2_GetActorLocation();
    FVector2D PlayerScreenPosition;
    FVector2D TargetScreenPosition;

    if (!PlayerController->ProjectWorldLocationToScreen(PawnLocation, &PlayerScreenPosition, true))
        return;

    FVector2D FOVCenter = {ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f};
    if (FOVCenter.X <= 0 || FOVCenter.Y <= 0)
        return;

    static auto drawList = ImGui::GetBackgroundDrawList();
    if (!drawList)
        return;

    if (!Configuration::bHideFOVCircle) {
        ImU32 CircleColor = IM_COL32(0xff, 0xff, 0xff, static_cast<int>(Configuration::CircleAlpha * 255));
        drawList->AddCircle(ImVec2(static_cast<float>(FOVCenter.X), static_cast<float>(FOVCenter.Y)), Configuration::FOVRadius, CircleColor, 1200, 1.0f);
    }

    if (Configuration::bDrawCrosshair) {
        ImU32 CrosshairColor = IM_COL32(0x7e, 0x55, 0xc0, static_cast<int>(Configuration::CircleAlpha * 255));
        drawList->AddCircleFilled(ImVec2(static_cast<float>(FOVCenter.X), static_cast<float>(FOVCenter.Y)), 3.0f, CrosshairColor, 60);
    }

    if (AimDetours::BestTargetLocation.IsZero())
        return;
    if (!PlayerController->ProjectWorldLocationToScreen(AimDetours::BestTargetLocation, &TargetScreenPosition, true))
        return;
    if (CustomMath::DistanceBetweenPoints(TargetScreenPosition, FOVCenter) > Configuration::FOVRadius)
        return;

    ImU32 TargetLineColor = IM_COL32(0xff, 0xff, 0xff, static_cast<int>(Configuration::CircleAlpha * 255));
    drawList->AddLine(ImVec2(static_cast<float>(FOVCenter.X), static_cast<float>(FOVCenter.Y)), ImVec2(static_cast<float>(TargetScreenPosition.X), static_cast<float>(TargetScreenPosition.Y)), TargetLineColor, 0.5f);

    // Drawing a purple circle at the end of the line
    ImU32 EndCircleColor = IM_COL32(0x40, 0x19, 0x80, static_cast<int>(Configuration::CircleAlpha * 255));
    drawList->AddCircleFilled(ImVec2(static_cast<float>(FOVCenter.X), static_cast<float>(FOVCenter.Y)), 2.0f, EndCircleColor, 60);
}

void PaliaOverlay::DrawGuiESP() {
    if (!Configuration::bEnableESP || (WidgetHandler::IsWidgetOpen() && !WidgetHandler::IsMapOpen()))
        return;
    
    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;
    
    FVector2D CanvasSize = {ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y};

    const auto drawList = ImGui::GetBackgroundDrawList();
    
    const auto narrowString = [](const std::wstring& wstr) -> std::string {
        if (wstr.empty())
            return {};

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), strTo.data(), size_needed, nullptr, nullptr);
        return strTo;
    };

    // Draw ESP Names Entities
    for (auto& entry : ActorHandler::GetActors()) {
        // Skip unselected ESP resources
        if (!entry.ShouldShow(ValeriaCharacter)) continue;
        
        //[Actor, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, Distance, Texture, Despawn]
        
        if (!entry.Actor || !IsActorValid(entry.Actor) || entry.WorldPosition.IsZero())
            continue;
        
        // skip hidden actors from other sub-plots
        if ((ActorHandler::CurrentMap & EValeriaLevel::Home) && !entry.Actor->bActorEnableCollision)
            continue;

        if (entry.ActorType == EType::Players) {
            if (entry.Actor == ValeriaCharacter)
                continue; // Skip local player.
        }

        const auto PlayerController = GetPlayerController();
        if (!PlayerController)
            break;

        FVector ActorLocation = entry.WorldPosition;

        // Adjust Z coordinate for head-level display
        float HeightAdjustment = 100.0f; // Adjust this value based on typical actor height
        ActorLocation.Z += HeightAdjustment;

        if (entry.Distance > Configuration::CullDistance)
            continue;

        ImU32 Color = IM_COL32(0xFF, 0xFF, 0xFF, 0xFF);
        bool bShouldDraw = false;

        std::string FAIconText;

        switch (entry.ActorType) {
        case EType::Forage:
            if (HUDDetours::Forageables[entry.Type][entry.Quality]) {
                if (!entry.Actor->bActorEnableCollision) {
                    continue;
                }
                bShouldDraw = true;
                if (auto it = Configuration::ForageableColors.find(static_cast<EForageableType>(entry.Type)); it != Configuration::ForageableColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_LEAF;
            }
            break;
        case EType::Ore:
            if (HUDDetours::Ores[entry.Type][entry.Variant]) {
                auto Ore = static_cast<ABP_ValeriaGatherableLoot_C*>(entry.Actor);
                if (Ore && IsActorValid(Ore)) {
                    if (Ore->IAmAlive) {
                        bShouldDraw = true;
                        // if (ActorHandler::IsActorA(Ore, L"/Game/Gameplay/Skills/Mining/BP_ValeriaGatherableLoot_Mining_MultiHarvest.BP_ValeriaGatherableLoot_Mining_MultiHarvest")) {
                        //     auto OreMulti = static_cast<ABP_ValeriaGatherableLoot_Mining_MultiHarvest_C*>(Ore);
                        //     if (OreMulti->DespawnTimestamp > 0  && OreMulti->ParticipationTracking) { // if it's not despawning we don't need to check if we've broken it
                        //         if (UBlueprintGameplayTagLibrary::IsGameplayTagValid(OreMulti->PreviousCycleParticipationTag)) {
                        //             bShouldDraw = !OreMulti->ParticipationTracking->IsParticipantForType(ValeriaCharacter, OreMulti->PreviousCycleParticipationTag);
                        //         }
                        //     }
                        // }
                        if (auto it = Configuration::OreColors.find(static_cast<EOreType>(entry.Type)); it != Configuration::OreColors.end())
                            Color = it->second;
                        FAIconText = ICON_FA_GEM;
                    }
                }
            }
            break;
        case EType::Players:
            if (HUDDetours::Singles[static_cast<int>(EOneOffs::Player)]) {
                if (entry.DisplayName.empty())
                    continue;
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::Player); it != Configuration::SingleColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_USER;
            }
            break;
        case EType::Animal:
            if (HUDDetours::Animals[entry.Type][entry.Variant]) {
                bShouldDraw = true;
                if (auto it = Configuration::AnimalColors.find(FCreatureType{static_cast<ECreatureKind>(entry.Type), static_cast<ECreatureQuality>(entry.Variant)}); it != Configuration::AnimalColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_PAW;
            }
            break;
        case EType::Tree:
            if (HUDDetours::Trees[entry.Type][entry.Variant]) {
                auto Tree = static_cast<ABP_ValeriaGatherableLoot_C*>(entry.Actor);
                if (Tree && IsActorValid(Tree)) {
                    if (Tree->IAmAlive) {
                        bShouldDraw = true;
                        if (auto it = Configuration::TreeColors.find(static_cast<ETreeType>(entry.Type)); it != Configuration::TreeColors.end())
                            Color = it->second;
                        FAIconText = ICON_FA_TREE;
                    }
                }
            }
            break;
        case EType::Bug:
            if (HUDDetours::Bugs[entry.Type][entry.Variant][entry.Quality]) {
                bShouldDraw = true;
                if (auto it = Configuration::BugColors.find(FBugType{static_cast<EBugKind>(entry.Type), static_cast<EBugQuality>(entry.Variant)}); it != Configuration::BugColors.end())
                    Color = it->second;
                if (entry.Quality == 1) {
                    FAIconText = ICON_FA_BUG;
                }
                FAIconText = ICON_FA_BUG;
            }
            break;
        case EType::NPCs:
            if (HUDDetours::Singles[static_cast<int>(EOneOffs::NPC)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::NPC); it != Configuration::SingleColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_PERSON;
            }
            break;
        case EType::Loot:
            if (HUDDetours::Singles[static_cast<int>(EOneOffs::Loot)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::Loot); it != Configuration::SingleColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_SACK;
            }
            break;
        case EType::Quest:
            if (HUDDetours::Singles[static_cast<int>(EOneOffs::Quest)]) {
                if (entry.Distance > 250)
                    continue; // quests don't always update collision until within this range
                if (!entry.Actor->bActorEnableCollision) {
                    continue;
                }
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::Quest); it != Configuration::SingleColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_LOCATION_DOT;
            }
            break;
        case EType::RummagePiles:
            if (HUDDetours::Singles[static_cast<int>(EOneOffs::RummagePiles)]) {
                auto Pile = static_cast<ATimedLootPile*>(entry.Actor);
                if (!Pile || !IsActorValid(Pile)) {
                    bShouldDraw = false;
                    break;
                }

                if (HUDDetours::Singles[static_cast<int>(EOneOffs::Others)]) {
                    bShouldDraw = true;
                    Color = Pile->bActivated ? IM_COL32(0xFF, 0xFF, 0xFF, 0xFF) : IM_COL32(0xFF, 0x00, 0x00, 0xFF);
                    FAIconText = ICON_FA_SQUARE_QUESTION;
                } else if (Pile->CanGather(ValeriaCharacter) && Pile->bActivated) {
                    bShouldDraw = true;
                    if (auto it = Configuration::SingleColors.find(EOneOffs::RummagePiles); it != Configuration::SingleColors.end())
                        Color = it->second;
                    FAIconText = ICON_FA_BALL_PILE;
                }

                if (entry.Variant > 0) {
                    std::wstring prefix = L"Rummage Pile - ";
                    entry.DisplayName.insert(0, prefix);
                }
            }
            break;
        case EType::Stables:
            if (HUDDetours::Singles[static_cast<int>(EOneOffs::Stables)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::Stables); it != Configuration::SingleColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_TORII_GATE;
            }
            break;
        case EType::Fish:
            if (HUDDetours::Singles[static_cast<int>(EOneOffs::FishHook)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::FishHook); it != Configuration::SingleColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_FISH;
            }
            break;
        case EType::Pool:
            if (HUDDetours::Singles[static_cast<int>(EOneOffs::FishPool)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::FishPool); it != Configuration::SingleColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_FISHING_ROD;
            }
            break;
        case EType::Treasure:
            if (HUDDetours::Singles[static_cast<int>(EOneOffs::Treasure)]) {
                if (entry.Distance > 150)
                    continue; // Chests don't seem to properly setup collision until in update range
                if (!entry.Actor->bActorEnableCollision) {
                    continue;
                }

                auto Treasure = static_cast<AWorldPersistGatherable*>(entry.Actor);
                if (!Treasure || !IsActorValid(Treasure))
                    break;

                if (HUDDetours::Singles[static_cast<int>(EOneOffs::Others)]) {
                    bShouldDraw = true;
                    if (auto it = Configuration::SingleColors.find(EOneOffs::Treasure); it != Configuration::SingleColors.end())
                        Color = it->second;
                    FAIconText = ICON_FA_TREASURE_CHEST;
                } else if (Treasure->CanGather(ValeriaCharacter)) {
                    bShouldDraw = true;
                    if (auto it = Configuration::SingleColors.find(EOneOffs::Treasure); it != Configuration::SingleColors.end())
                        Color = it->second;
                    FAIconText = ICON_FA_TREASURE_CHEST;
                }
            }
            break;
        case EType::TimedDrop:
            if (HUDDetours::Singles[static_cast<int>(EOneOffs::TimedDrop)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::TimedDrop); it != Configuration::SingleColors.end())
                    Color = it->second;
                if (entry.Variant > 1) {
                    FAIconText = ICON_FA_SQUARE_QUESTION;
                }
            }
            break;
        case EType::Relic:
            if (HUDDetours::Singles[static_cast<int>(EOneOffs::Relic)]) {
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::Relic); it != Configuration::SingleColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_BOOK;
            }
            break;
        default:
            break;
        }

        if (HUDDetours::Singles[static_cast<int>(EOneOffs::Others)] && entry.Type == 0)
            bShouldDraw = true;

        if (!bShouldDraw)
            continue;

        FVector2D ScreenLocation;
        if (WidgetHandler::IsMapOpen()) {
            if (!Configuration::bEnableMapESP) continue;
            if ((EType::NPCs | EType::Stables) & entry.ActorType) continue; // Don't need to draw NPCs/Stables on map, they have their own icons
            if (auto LocationPair = WidgetHandler::GetMapPoint(ActorLocation); LocationPair.first) {
                ImVec2 mousePos = ImGui::GetIO().MousePos;
                
                auto textSize = ImGui::CalcTextSize(FAIconText.c_str());
                auto TextPosition = ImVec2(static_cast<float>(LocationPair.second.X - textSize.x / 2), static_cast<float>(LocationPair.second.Y - textSize.y / 2));

                ImFont* font = Menu::ESPFont;
                ImGui::PushFont(font);
                drawTextOutlined(drawList, TextPosition, Color, FAIconText.c_str(), font);
                ImGui::PopFont();

                std::wstring text = entry.DisplayName;
                std::string shortText = narrowString(text);

                ImVec2 tooltipTextSize = ImGui::CalcTextSize(shortText.c_str());

                ImVec2 textMin = TextPosition;
                ImVec2 textMax = ImVec2(TextPosition.x + textSize.x, TextPosition.y + textSize.y);

                if (mousePos.x >= textMin.x && mousePos.x <= textMax.x && mousePos.y >= textMin.y && mousePos.y <= textMax.y) {
                    ImVec2 tooltipPos = ImVec2(mousePos.x - tooltipTextSize.x / 2, mousePos.y - tooltipTextSize.y - 10.0f);
                    /*drawList->AddRect(ImVec2(tooltipPos.x - 4, tooltipPos.y - 2), ImVec2(tooltipPos.x + tooltipTextSize.x + 4, tooltipPos.y + tooltipTextSize.y + 2), IM_COL32(0x18, 0x18, 0x30, 0x45), 2.0f);
                    drawList->AddRectFilled(ImVec2(tooltipPos.x - 4, tooltipPos.y - 2), ImVec2(tooltipPos.x + tooltipTextSize.x + 4, tooltipPos.y + tooltipTextSize.y + 2), IM_COL32(0x18, 0x18, 0x30, 0x26), 2.0f);*/
                    drawTextOutlined(drawList, tooltipPos, Color, shortText.c_str(), Menu::FontDefault);
                }
            }
        } else {
            if (!entry.TexturePath.empty() && Configuration::bEnableESPIcons)
                continue;
            try {
                if (PlayerController->ProjectWorldLocationToScreen(ActorLocation, &ScreenLocation, true)) {
                    if (ScreenLocation.X < 0 || ScreenLocation.Y < 0 || ScreenLocation.X > CanvasSize.X || ScreenLocation.Y > CanvasSize.Y)
                        continue;
                    // Construct text string

                    // Prepare text with optional parts depending on the index values
                    std::wstring text = entry.DisplayName;
                    if (Configuration::bEnableESPDistance) {
                        text += std::format(L" [{:.0f}m]", entry.Distance);
                    }
                    if (Configuration::bEnableESPDespawnTimer) {
                        if (entry.Despawn > 0) {
                            text += std::format(L" {:.0f}s", entry.Despawn);
                        }
                    }

                    std::string shortText = FAIconText + " " + narrowString(text);

                    float textScale = Configuration::ESPTextScale;

                    ImVec2 textSize = ImGui::CalcTextSize(shortText.c_str());
                    textSize.x *= textScale;
                    textSize.y *= textScale;

                    auto TextPosition = ImVec2(static_cast<float>(ScreenLocation.X - textSize.x / 2), static_cast<float>(ScreenLocation.Y - textSize.y / 2));

                    // Constrain text to screen before disappearing.
                    if (TextPosition.x < 4.0f) { // left side
                        TextPosition.x = 4.0f;
                    } else if (TextPosition.x + textSize.x > CanvasSize.X) { // right side
                        TextPosition.x = static_cast<float>(CanvasSize.X) - textSize.x;
                    }
                    if (TextPosition.y < 4.0f) { // top side
                        TextPosition.y = 4.0f;
                    } else if (TextPosition.y + textSize.y > CanvasSize.Y - 4.0f) { // bottom side
                        TextPosition.y = static_cast<float>(CanvasSize.Y) - textSize.y - 4.0f;
                    }

                    drawTextOutlined(drawList, TextPosition, Color, shortText.c_str(), Menu::ESPFont);
                }
            } catch (std::exception& e) {
                std::cout << "ProjectWorldLocationToScreen exception: " << e.what() << "\n";
            }
        }
    }
}

void PaliaOverlay::DrawHUD() {
    //auto Overlay = static_cast<PaliaOverlay*>(Instance);
    Configuration::Load();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard navigation once at initialization if possible.

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 0.35f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);

    std::string defaultWatermarkText = "OriginPalia - Game V0.183.0 - " + EValeriaLevelString[ActorHandler::CurrentMap];
    bool showWatermark = false;
    if ((ActorHandler::CurrentLevel && (EValeriaLevel::Startup & ActorHandler::CurrentMap)) || Configuration::bShowWatermark) {
        if (EValeriaLevel::Startup & ActorHandler::CurrentMap) {
            defaultWatermarkText = "Waiting for the game to load...";
        }
        showWatermark = true;
    }

    if (showWatermark) {
        ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - ImGui::CalcTextSize(defaultWatermarkText.c_str()).x) * 0.5f, 10.0f));
        ImGui::Begin("Watermark", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
        ImGui::Text("%s", defaultWatermarkText.c_str());
        ImGui::End();
    }
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();

    DrawEnabledFeatures();

    if (getTheme) { // Save previousColors here?
        switch (Configuration::selectedTheme) {
        case 0:
            ImGui::StyleColorsDark();
            break;
        case 1:
            ImGui::Spectrum::StyleColorsSpectrum();
            break;
        case 2:
            ImGui::Magma::StyleColorsMagma();
            break;
        case 3:
            // load custom theme logic.
            Configuration::ApplyCustomTheme();
            break;
        default:
            ImGui::StyleColorsDark();
            break;
        }
        getTheme = false;
    }

    /*
     * Notification rendering
     */
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImVec4 windowBg = ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_WindowBg)); //pls dont make this a const Void. Yes even if Rider tells you to
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(windowBg.x, windowBg.y, windowBg.z, 1.00f));
    ImGui::RenderNotifications();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
    
    /*
     * Notification(s) for first use-case scenario
     */
    static bool displayedNotif = false;
    if (Configuration::bFirstUse && !displayedNotif) {
        ImGui::InsertNotification({ImGuiToastType::Info, 8000, "Hello! Thank you for using OriginPalia. To get started, please open the main menu with the 'INSERT' key."});
        displayedNotif = true;
    }
}

void PaliaOverlay::DrawOverlay() {
    bool show = true;
    const ImGuiIO& io = ImGui::GetIO();
    constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;

    // Calculate the center position for the window
    const auto center_pos = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    const auto window_size = ImVec2(Configuration::windowSizeX, Configuration::windowSizeY);
    const auto window_pos = ImVec2(center_pos.x - window_size.x * 0.5f, center_pos.y - window_size.y * 0.5f);

    // Set the initial window position to the center of the screen
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_FirstUseEver);

    const auto WindowTitle = std::string("OriginPalia 3.1");
    //auto Overlay = static_cast<PaliaOverlay*>(Instance);

    if (ImGui::Begin(WindowTitle.data(), &show, window_flags)) {

        /*
         * First use case-scenario bool
         */
        if (Configuration::bFirstUse) {
            Configuration::bFirstUse = false;
            Configuration::Save(ESaveFile::OverlaySettings);
        }
        
        static int OpenTab = 0;

        if (ImGui::IsMouseDragging(0) &&
            (!AreFloatsEqual(Configuration::windowSizeX, ImGui::GetWindowSize().x) || !AreFloatsEqual(Configuration::windowSizeY, ImGui::GetWindowSize().y))) { // dont want to spam save if you're just dragging your mouse.
            Configuration::windowSizeX = ImGui::GetWindowSize().x;
            Configuration::windowSizeY = ImGui::GetWindowSize().y;
            Configuration::Save(ESaveFile::OverlaySettings);
        }

        // Draw tabs
        if (ImGui::BeginTabBar("OverlayTabs")) {
            if (ImGui::BeginTabItem(ICON_FA_EYE " ESP & Visuals")) {
                OpenTab = 0;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(ICON_FA_CROSSHAIRS " Aim Assistants")) {
                OpenTab = 1;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(ICON_FA_PERSON_WALKING " Movements & TPs")) {
                OpenTab = 2;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(ICON_FA_SCREWDRIVER_WRENCH " Skills & Tools")) {
                OpenTab = 3;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(ICON_FA_BASKET_SHOPPING " Selling & Items")) {
                OpenTab = 4;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(ICON_FA_GEAR " Mods & Settings")) {
                OpenTab = 5;
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        AValeriaCharacter* ValeriaCharacter = GetValeriaCharacter();

        // ==================================== 0 Visuals & ESPs TAB
        if (OpenTab == 0) {
            ImGui::Columns(3, nullptr, false);

            // Base ESP controls
            if (ImGui::CollapsingHeader("Visual Settings##VisualSettingsGeneralHeader", ImGuiTreeNodeFlags_DefaultOpen)) {

                IMGUI_CHECKBOX("Enable ESP", &Configuration::bEnableESP, ImGui::CheckboxHotkeyFlags::HOTKEY);

                if(Configuration::bEnableESP) {
                    ImGui::SetNextItemWidth(200.0f);
                    IMGUI_SLIDER("ESP Distance", &Configuration::CullDistance, 1.0, 1000.0, "%1.0f", "Change the distance limit of your ESP.")

                    ImGui::SetNextItemWidth(200.0f);
                    IMGUI_SLIDER("ESP Text Scale", &Configuration::ESPTextScale, 0.5f, 2.0f, "%.1f", "Adjust the scale of ESP text size.")
                    
                    IMGUI_CHECKBOX("Show Distance", &Configuration::bEnableESPDistance, "Displays the distances to the ESPs.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    IMGUI_CHECKBOX("Show Despawning Timers", &Configuration::bEnableESPDespawnTimer, "Shows the time left in seconds before a resource despawns.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    IMGUI_CHECKBOX("Show ESP In World-Map", &Configuration::bEnableMapESP, ImGui::CheckboxHotkeyFlags::HOTKEY)
                    IMGUI_CHECKBOX("Use Native Icons", &Configuration::bEnableESPIcons, "Display in-game icons instead of text on screen.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                }
            }

            ImGui::NextColumn();

            ESP_AnimalSettings::Show();
            ESP_OreSettings::Show();
            ESP_ForageableSettings::Show();

            ImGui::NextColumn();

            ESP_BugSettings::Show();
            ESP_TreeSettings::Show();
            ESP_SingleSettings::Show();
        }
        // ==================================== 1 Aimbots & Fun TAB
        else if (OpenTab == 1) {
            ImGui::Columns(2, nullptr, false);

            if (ImGui::CollapsingHeader("InteliAim Settings##InteliAimSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    IMGUI_CHECKBOX("Enable InteliAim", &Configuration::bEnableInteliAim, "Enable the FOV targeting system. Teleport to actors, enable aimbots, and more.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    if (Configuration::bEnableInteliAim) {
                        ImGui::SetNextItemWidth(200.0f);
                        IMGUI_SLIDER("InteliAim Radius", &Configuration::FOVRadius, 10.0f, 600.0f, "%1.0f", "Make your targeting area bigger or smaller. Set this before Hiding Circle.")
                        IMGUI_CHECKBOX("Hide Circle", &Configuration::bHideFOVCircle, "Use the InteliTarget system without displaying the InteliAim Circle.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                        if (!Configuration::bHideFOVCircle) {
                            ImGui::SetNextItemWidth(200.0f);
                            IMGUI_SLIDER("Circle Transparency", &Configuration::CircleAlpha, 0.25f, 1.0f, "%.2f", "Adjust the transparency of the InteliAim Circle.")
                            IMGUI_CHECKBOX("Show Crosshair", &Configuration::bDrawCrosshair, "Display a small purple crosshair in the middle of your screen. Nicely paired with Hide Circle.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                        }
                    }
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            ImGui::NextColumn();

            // InteliTarget Controls
            if (ImGui::CollapsingHeader("InteliTarget Settings##InteliTargetSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    if (Configuration::bEnableInteliAim) {
                        IMGUI_CHECKBOX("Enable Silent Aimbot", &Configuration::bEnableSilentAimbot, "Teleport Bug Bombs & Arrows To Your Target.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                        IMGUI_CHECKBOX("Enable Teleporting To Target", &Configuration::bTeleportToTargeted, "Teleport directly to your targeted entity.", ImGui::CheckboxHotkeyFlags::HOTKEY);
                        
                        if (Configuration::bTeleportToTargeted) {
                            IMGUI_CHECKBOX("Avoid Teleporting To Players", &Configuration::bAvoidTeleportingToPlayers, "Don't teleport to targeted players.", ImGui::CheckboxHotkeyFlags::HOTKEY);
                            IMGUI_CHECKBOX("Avoid Teleporting To Target When Players Are Near", &Configuration::bDoRadiusPlayersAvoidance, "Don't teleport if a player is detected near your destination.", ImGui::CheckboxHotkeyFlags::HOTKEY);
                            if (Configuration::bDoRadiusPlayersAvoidance) {
                                ImGui::SetNextItemWidth(200.0f);
                                IMGUI_SLIDER("Avoidance Radius", &Configuration::TeleportPlayerAvoidanceRadius, 1, 100, "%1", "The distance to avoid players when teleporting to a target.")
                            }
                        }
                    } else {
                        Configuration::bEnableSilentAimbot = false;
                        ImGui::Spacing();
                        ImGui::Text("[ ENABLE INTELIAIM TO VIEW INTELITARGET OPTIONS ]");
                        ImGui::Spacing();
                    }
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }
        }
        // ==================================== 2 Movement & Teleport TAB
        else if (OpenTab == 2) {
            // Setting the columns layout
            ImGui::Columns(2, nullptr, false);

            UValeriaCharacterMoveComponent* MovementComponent = nullptr;
            if (ValeriaCharacter) {
                MovementComponent = ValeriaCharacter->GetValeriaCharacterMovementComponent();
            }

            // Movement settings column
            if (ImGui::CollapsingHeader("Movement Settings##MovementGeneralSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (IsComponentValid(MovementComponent)) {
                    IMGUI_CHECKBOX("Enable Noclip", &MovementDetours::bEnableNoclip, "Allows you to fly and phase through anything.", ImGui::CheckboxHotkeyFlags::HOTKEY)

                    // Create a combo box for selecting the movement mode
                    static const char* movementModes[] = {"Walking", "Flying", "Fly No Collision"};
                    ImGui::Text("Movement Mode");
                    ImGui::SetNextItemWidth(300.0f);
                    if (ImGui::BeginCombo("##MovementMode", movementModes[MovementDetours::currentMovementModeIndex])) {
                        for (int n = 0; n < IM_ARRAYSIZE(movementModes); n++) {
                            const bool isSelected = MovementDetours::currentMovementModeIndex == n;
                            if (ImGui::Selectable(movementModes[n], isSelected)) {
                                MovementDetours::currentMovementModeIndex = n;
                            }
                            // Set the initial focus when opening the combo
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::SameLine();
                    // Button to apply the selected movement mode
                    if (ImGui::Button("Set")) {
                        switch (MovementDetours::currentMovementModeIndex) {
                        case 0: // Walking
                            MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking, 1);
                            ValeriaCharacter->CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
                            break;
                        case 1: // Swimming
                            MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying, 4);
                            ValeriaCharacter->CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
                            break;
                        case 2: // Noclip
                            MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying, 5);
                            ValeriaCharacter->CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
                            ValeriaCharacter->CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
                            ValeriaCharacter->CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
                            ValeriaCharacter->CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
                            ValeriaCharacter->CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
                            ValeriaCharacter->CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);
                            break;
                        default:
                            break;
                        }
                    }

                    constexpr float f1000 = 1000.0f, f5 = 5.f, f1 = 1.f;
                    constexpr int i1 = 1;
                    //constexpr int i1000 = 1000, i5 = 5,

                    // Global Game Speed with slider
                    ImGui::Text("Global Game Speed: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##GlobalGameSpeed", ImGuiDataType_Float, &MovementDetours::CustomGameSpeed, &f1, &f1000, "%.2f", ImGuiInputTextFlags_None)) {
                        UWorld* World = GetWorld();
                        if (World) {
                            static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject)->SetGlobalTimeDilation(World, MovementDetours::CustomGameSpeed);
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##GlobalGameSpeed")) {
                        MovementDetours::CustomGameSpeed = MovementDetours::GameSpeed;
                        UWorld* World = GetWorld();
                        if (World) {
                            static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject)->SetGlobalTimeDilation(World, MovementDetours::GameSpeed);
                        }
                    }

                    // Walk Speed
                    ImGui::Text("Walk Speed: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##WalkSpeed", ImGuiDataType_Float, &Configuration::CustomWalkSpeed, &f5)) {
                        Configuration::Save(ESaveFile::MovementSettings);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##WalkSpeed")) {
                        Configuration::CustomWalkSpeed = MovementDetours::WalkSpeed;
                        Configuration::Save(ESaveFile::MovementSettings);
                    }

                    // Sprint Speed
                    ImGui::Text("Sprint Speed: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##SprintSpeedMultiplier", ImGuiDataType_Float, &Configuration::CustomSprintSpeed, &f5)) {
                        Configuration::Save(ESaveFile::MovementSettings);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##SprintSpeedMultiplier")) {
                        Configuration::CustomSprintSpeed = MovementDetours::SprintSpeedMultiplier;
                        Configuration::Save(ESaveFile::MovementSettings);
                    }

                    // Climbing Speed
                    ImGui::Text("Climbing Speed: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##ClimbingSpeed", ImGuiDataType_Float, &Configuration::CustomClimbingSpeed, &f5)) {
                        Configuration::Save(ESaveFile::MovementSettings);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##ClimbingSpeed")) {
                        Configuration::CustomClimbingSpeed = MovementDetours::ClimbingSpeed;
                        Configuration::Save(ESaveFile::MovementSettings);
                    }

                    // Gliding Speed
                    ImGui::Text("Gliding Speed: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##GlidingSpeed", ImGuiDataType_Float, &Configuration::CustomGlidingSpeed, &f5)) {
                        Configuration::Save(ESaveFile::MovementSettings);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##GlidingSpeed")) {
                        Configuration::CustomGlidingSpeed = MovementDetours::GlidingSpeed;
                        Configuration::Save(ESaveFile::MovementSettings);
                    }

                    // Gliding Fall Speed
                    ImGui::Text("Gliding Fall Speed: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##GlidingFallSpeed", ImGuiDataType_Float, &Configuration::CustomGlidingFallSpeed, &f5)) {
                        Configuration::Save(ESaveFile::MovementSettings);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##GlidingFallSpeed")) {
                        Configuration::CustomGlidingFallSpeed = MovementDetours::GlidingFallSpeed;
                        Configuration::Save(ESaveFile::MovementSettings);
                    }

                    // Jump Velocity
                    ImGui::Text("Jump Velocity: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##JumpVelocity", ImGuiDataType_Float, &Configuration::CustomJumpVelocity, &f5)) {
                        Configuration::Save(ESaveFile::MovementSettings);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##JumpVelocity")) {
                        Configuration::CustomJumpVelocity = MovementDetours::JumpVelocity;
                        Configuration::Save(ESaveFile::MovementSettings);
                    }

                    // Max Jump Count
                    ImGui::Text("Max Jumps: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##MaxJumpAmount", ImGuiDataType_S32, &Configuration::CustomMaxJumps, &i1)) {
                        Configuration::Save(ESaveFile::MovementSettings);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##MaxJumpAmount")) {
                        Configuration::CustomMaxJumps = MovementDetours::JumpMaxCount;
                        Configuration::Save(ESaveFile::MovementSettings);
                    }

                    // Step Height
                    ImGui::Text("Step Height: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##MaxStepHeight", ImGuiDataType_Float, &Configuration::CustomMaxStepHeight, &f5)) {
                        Configuration::Save(ESaveFile::MovementSettings);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##MaxStepHeight")) {
                        Configuration::CustomMaxStepHeight = MovementDetours::MaxStepHeight;
                        Configuration::Save(ESaveFile::MovementSettings);
                    }
                } else {
                    if (!ValeriaCharacter) {
                        ImGui::Text("Waiting for character initialization...");
                    } else {
                        ImGui::Text("Movement component not available.");
                    }
                }
            }

            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Locations & Coordinates##LocationSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    // Locations and exploits column
                    ImGui::Text("Teleport List");
                    ImGui::Text("Double-click a location listing to teleport");
                    if (ImGui::BeginListBox("##TeleportList", ImVec2(-1, 150))) {
                        for (auto& [MapName, Type, Name, Location, Rotate] : TeleportLocations) {
                            if (EValeriaLevelString[ActorHandler::CurrentMap] == MapName || MapName == "UserDefined") {
                                if (ImGui::Selectable(Name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                                    if (ImGui::IsMouseDoubleClicked(0)) {
                                        if (Type == ELocation::Global_Home) {
                                            ValeriaCharacter->GetTeleportComponent()->RpcServerTeleport_Home();
                                        } else {
                                            TeleportDetours::TeleportPlayer(Location);
                                        }
                                        show = false;
                                    }
                                }
                            }
                        }
                        ImGui::EndListBox();
                    } else {
                        ImGui::Text("Not enough space to display the list.");
                    }

                    ImGui::Text("Map: %s", EValeriaLevelString[ActorHandler::CurrentMap].c_str());
                    
                    auto [PlayerX, PlayerY, PlayerZ] = ValeriaCharacter->K2_GetActorLocation();
                    ImGui::Text("Current Coords: %.3f, %.3f, %.3f", PlayerX, PlayerY, PlayerZ);
                    ImGui::Spacing();

                    // Set the width for the labels and inputs
                    constexpr float labelWidth = 50.0f;
                    constexpr float inputWidth = 200.0f;

                    // 
                    static FVector TeleportLocation;
                    //static FRotator TeleportRotate;

                    constexpr double d5 = 5., d1 = 1.;

                    // X Coordinate
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("X: ");
                    ImGui::SameLine(labelWidth);
                    ImGui::SetNextItemWidth(inputWidth);
                    ImGui::InputScalar("##TeleportLocationX", ImGuiDataType_Double, &TeleportLocation.X, &d5);

                    // Y Coordinate
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("Y: ");
                    ImGui::SameLine(labelWidth);
                    ImGui::SetNextItemWidth(inputWidth);
                    ImGui::InputScalar("##TeleportLocationY", ImGuiDataType_Double, &TeleportLocation.Y, &d5);

                    // Z Coordinate
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("Z: ");
                    ImGui::SameLine(labelWidth);
                    ImGui::SetNextItemWidth(inputWidth);
                    ImGui::InputScalar("##TeleportLocationZ", ImGuiDataType_Double, &TeleportLocation.Z, &d5);

                    // // Yaw
                    // ImGui::AlignTextToFramePadding();
                    // ImGui::Text("YAW: ");
                    // ImGui::SameLine(labelWidth);
                    // ImGui::SetNextItemWidth(inputWidth);
                    // ImGui::InputScalar("##TeleportRotateYaw", ImGuiDataType_Double, &TeleportRotate.Yaw, &d1);

                    ImGui::Spacing();

                    if (ImGui::Button("Get Current Coordinates")) {
                        TeleportLocation = ValeriaCharacter->K2_GetActorLocation();
                        //TeleportRotate = ValeriaCharacter->K2_GetActorRotation();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Teleport To Coordinates")) {
                        TeleportDetours::TeleportPlayer(TeleportLocation);
                    }
                    // Travel
                    ImGui::Text("Travel List");
                    ImGui::Text("Double-click a location listing to travel");
                    if (ImGui::BeginListBox("##TravelList", ImVec2(-1, 150))) {
                        for (auto& [Name, TeleportActor] : ActorHandler::GetTeleporters(ValeriaCharacter)) {
                            if (ImGui::Selectable(Name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                                if (ImGui::IsMouseDoubleClicked(0)) {
                                    auto Teleporter = static_cast<AVAL_TeleporterBase*>(TeleportActor);
                                    Teleporter->Client_ExecuteTeleport(ValeriaCharacter);
                                    show = false;
                                }
                            }
                        }
                        ImGui::EndListBox();
                    } else {
                        ImGui::Text("Not enough space to display the list.");
                    }
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }
        }
        // ==================================== 3 Skills & Tools TAB
        else if (OpenTab == 3) {
            ImGui::Columns(2, nullptr, false);

            UFishingComponent* FishingComponent = nullptr;

            auto EquippedTool = ToolHandler::EquippedTool;

            if (ValeriaCharacter) FishingComponent = ValeriaCharacter->GetFishing();

            // Tool Swinger
            if (ImGui::CollapsingHeader("Axe & Pickaxe Settings##SwingToolsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    IMGUI_SUPPORTER_FEATURE("Auto Swing Tool", &Configuration::bEnableAutoSwinging, "Automatically chop or mine materials around you as you play.", "checkbox", NULL, NULL, NULL, ImGui::CheckboxHotkeyFlags::HOTKEY)
                    if (Configuration::bEnableAutoSwinging) {
                        IMGUI_SUPPORTER_FEATURE("Auto Equip Tool", &Configuration::bResourceAutoEquipTool, "Automatically equips your Axe/Pickaxe when in range of resources.", "checkbox", NULL, NULL, NULL, ImGui::CheckboxHotkeyFlags::HOTKEY);
                        IMGUI_SUPPORTER_FEATURE("Avoid Chopping Grove", &Configuration::bAvoidGroveChopping, "Avoid chopping flow trees near other flow trees.", "checkbox", NULL, NULL, NULL, ImGui::CheckboxHotkeyFlags::HOTKEY)
                        // TODO: Include Player Avoidance Option To Avoid Use Of Auto Features
                    }
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            // Bug Settings Section
            if (ImGui::CollapsingHeader("Bug Settings##BugToolsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    IMGUI_SUPPORTER_FEATURE("Auto Catch Bugs", &Configuration::bEnableBugCatching, "Automatically catch bugs within set distance of your player while you have a bug belt active", "checkbox", NULL, NULL, NULL, ImGui::CheckboxHotkeyFlags::HOTKEY);

                    if (Configuration::bEnableBugCatching) {
                        IMGUI_SUPPORTER_FEATURE("Auto Equip Smoke Bombs", &Configuration::bBugAutoEquipTool, "Automatically equips your Smoke Bombs when in range of bugs.", "checkbox", NULL, NULL, NULL, ImGui::CheckboxHotkeyFlags::HOTKEY);
                        IMGUI_SUPPORTER_FEATURE("Random Catching Delay", &Configuration::bBugUseRandomDelay, "Make your auto catching utilize humanization - less suspicious.", "checkbox", NULL, NULL, NULL, ImGui::CheckboxHotkeyFlags::HOTKEY);

                        ImGui::SetNextItemWidth(200.0f);
                        IMGUI_SUPPORTER_FEATURE("Catching Distance", &Configuration::BugCatchingDistance, "Change the working distance of your auto catching bug bombs", "slider", 2.0f, 20.0f, "%1.0f");

                        if (Configuration::bBugUseRandomDelay) {
                            ImGui::SetNextItemWidth(200.0f);
                            if (ImGui::Combo("Catching Randomization", &Configuration::BugSpeedPreset, speed_items, IM_ARRAYSIZE(speed_items))) {
                                Configuration::BugCatchingDelay = 100 * (1 + 4 * Configuration::BugSpeedPreset);
                                Configuration::Save(ESaveFile::ToolSkillsSettings);
                            }
                        } else {
                            ImGui::SetNextItemWidth(200.0f);
                            if (ImGui::InputInt("Catching Speed", &Configuration::BugCatchingDelay, 50)) {
                                Configuration::BugCatchingDelay = std::clamp(Configuration::BugCatchingDelay, 100, 2000);
                                Configuration::Save(ESaveFile::ToolSkillsSettings);
                            }
                        }
                    }
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            // Animal Settings Section
            if (ImGui::CollapsingHeader("Animal Settings##AnimalToolsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    IMGUI_SUPPORTER_FEATURE("Auto Hunt Animals", &Configuration::bEnableAnimalHunting, "Automatically hunt animals within set distance of your player while you have a bow active", "checkbox", NULL, NULL, NULL, ImGui::CheckboxHotkeyFlags::HOTKEY);

                    if (Configuration::bEnableAnimalHunting) {
                        IMGUI_SUPPORTER_FEATURE("Auto Equip Bow", &Configuration::bAnimalAutoEquipTool, "Automatically equips your Bow when in range of animals.", "checkbox", NULL, NULL, NULL, ImGui::CheckboxHotkeyFlags::HOTKEY);
                        IMGUI_SUPPORTER_FEATURE("Random Hunting Delay", &Configuration::bAnimalUseRandomDelay, "Make your auto hunting utilize humanization - less suspicious.", "checkbox", NULL, NULL, NULL, ImGui::CheckboxHotkeyFlags::HOTKEY);

                        ImGui::SetNextItemWidth(200.0f);
                        IMGUI_SUPPORTER_FEATURE("Hunting Distance", &Configuration::AnimalHuntingDistance, "Change the working distance of your auto hunting arrows", "slider", 2.0f, 20.0f, "%1.0f");

                        if (Configuration::bAnimalUseRandomDelay) {
                            ImGui::SetNextItemWidth(200.0f);
                            if (ImGui::Combo("Hunting Randomization", &Configuration::AnimalSpeedPreset, speed_items, IM_ARRAYSIZE(speed_items))) {
                                Configuration::AnimalHuntingDelay = 100 * (1 + 4 * Configuration::AnimalSpeedPreset);
                                Configuration::Save(ESaveFile::ToolSkillsSettings);
                            }
                        } else {
                            ImGui::SetNextItemWidth(200.0f);
                            if (ImGui::InputInt("Hunting Speed", &Configuration::AnimalHuntingDelay, 50)) {
                                Configuration::AnimalHuntingDelay = std::clamp(Configuration::AnimalHuntingDelay, 100, 2000);
                                Configuration::Save(ESaveFile::ToolSkillsSettings);
                            }
                        }
                    }
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            if (ImGui::CollapsingHeader("Other Settings##OtherHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    IMGUI_CHECKBOX("Auto Gather Nearby Loot", &Configuration::bEnableAutoGather, "Automatically gather forageables (within range) when you play.", ImGui::CheckboxHotkeyFlags::HOTKEY);
                    IMGUI_CHECKBOX("Skip Cooking Minigames", &Configuration::bEnableCookingMinigameSkip, "Skips the cooking minigame process completely.", ImGui::CheckboxHotkeyFlags::HOTKEY);
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Fishing Settings##FishingHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (IsComponentValid(FishingComponent)) {
                    IMGUI_CHECKBOX("No Rod Durability Loss", &Configuration::bFishingNoDurability, "Never break your fishing rod.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    IMGUI_CHECKBOX("Enable Multiplayer Help", &Configuration::bFishingMultiplayerHelp, "Force fishing with other players for extra quest completion.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    IMGUI_CHECKBOX("Always Perfect Catch", &Configuration::bFishingPerfectCatch, "Fishing will always result in a perfect catch.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    IMGUI_CHECKBOX("Instant Catch", &Configuration::bFishingInstantCatch, "Catch fish as soon as your bobber hits the water.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    IMGUI_CHECKBOX("Discard All Junk", &Configuration::bFishingDiscard, "When fishing, automatically remove junk items from your inventory.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    IMGUI_CHECKBOX("Open and Store Makeshift Decor", &Configuration::bFishingOpenStoreWaterlogged, "When fishing, automatically move valuables to your home base storage.", ImGui::CheckboxHotkeyFlags::HOTKEY)

                    if (EquippedTool == ETools::FishingRod) {
                        IMGUI_CHECKBOX("Auto Fast Fishing", &FishingDetours::bEnableAutoFishing, "Fish extremely fast. Pairs nicely with other fishing features.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                        IMGUI_CHECKBOX("Require Holding Left-Click To Auto Fish", &Configuration::bRequireClickFishing, "Require holding the left mouse button to toggle the fast fishing.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    } else {
                        ImGui::Spacing();
                        ImGui::Text("[ EQUIP FISHING ROD TO VIEW FAST FISHING OPTIONS ]");
                        ImGui::Spacing();
                        Configuration::bRequireClickFishing = true;
                    }

                    IMGUI_CHECKBOX("Force Fishing Pool", &FishingDetours::bOverrideFishingSpot, "Force all catches to result from the selected pool.", ImGui::CheckboxHotkeyFlags::HOTKEY)

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(200.0f);
                    if (ImGui::Combo("##FishingSpotsCombo", &FishingDetours::bSelectedFishingSpot, FishingDetours::bFishingSpots, IM_ARRAYSIZE(FishingDetours::bFishingSpots))) {
                        if (FishingDetours::bSelectedFishingSpot > 0) {
                            FishingDetours::sOverrideFishingSpot = UKismetStringLibrary::Conv_StringToName(FString(FishingDetours::bFishingSpotsFString[FishingDetours::bSelectedFishingSpot - 1]));
                        } else {
                            FishingDetours::bSelectedFishingSpot = 0;
                            FishingDetours::sOverrideFishingSpot = {};
                            FishingDetours::bOverrideFishingSpot = false;
                        }
                    }
                } else {
                    if (!ValeriaCharacter) {
                        ImGui::Text("Waiting for character initialization...");
                    } else {
                        ImGui::Text("Fishing component not available.");
                    }
                }
            }
        }
        // ==================================== 4 Selling & Player TAB
        else if (OpenTab == 4) {
            ImGui::Columns(2, nullptr, false);

            if (ImGui::CollapsingHeader("Selling Settings##SellingSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    IMGUI_CHECKBOX("Item Buy Multiplier", &Configuration::bEnableBuyMultiplier, "Enables ability to change how many items you purchase each time.", ImGui::CheckboxHotkeyFlags::HOTKEY);
                    if (Configuration::bEnableBuyMultiplier) {
                        IMGUI_CHECKBOX("Enable CTRL-Click to Buy Modded", &Configuration::bEnableCtrlClickBuy, "Enables ability to require holding CTRL key to buy modded stack amount.", ImGui::CheckboxHotkeyFlags::HOTKEY);

                        ImGui::SetNextItemWidth(200.0f);
                        IMGUI_SLIDER("Amount to Multiply", &Configuration::buyMultiplierValue, 1, 99, "%1", "Buy amount multiplier")
                    }

                    ImGui::Text("Select the bag, slot, and quantity to sell.");
                    ImGui::Spacing();
                    static int selectedSlot = 0;
                    static int selectedQuantity = 1;
                    static const char* quantities[] = {"1", "10", "50", "999", "Custom"};
                    static char customQuantity[64] = "100";

                    // Slot selection dropdown
                    ImGui::SetNextItemWidth(200.0f);
                    if (ImGui::BeginCombo("Slot", std::to_string(selectedSlot + 1).c_str())) {
                        for (int i = 0; i < 8; i++) {
                            const bool isSelected = (selectedSlot == i);
                            if (ImGui::Selectable(std::to_string(i + 1).c_str(), isSelected)) {
                                selectedSlot = i;
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    // Quantity selection dropdown
                    ImGui::SetNextItemWidth(200.0f);
                    if (ImGui::BeginCombo("Quantity", quantities[selectedQuantity])) {
                        for (int i = 0; i < IM_ARRAYSIZE(quantities); i++) {
                            const bool isSelected = (selectedQuantity == i);
                            if (ImGui::Selectable(quantities[i], isSelected)) {
                                selectedQuantity = i;
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    if (selectedQuantity == 4) {
                        ImGui::InputText("##CustomQuantity", customQuantity, IM_ARRAYSIZE(customQuantity));
                    }

                    bool cannotSell = false;
                    if (ImGui::Button("Sell Items")) {
                        StoreDetours::ManualSellLocation.BagIndex = 0;
                        StoreDetours::ManualSellLocation.SlotIndex = selectedSlot;

                        StoreDetours::ManualSellQty = selectedQuantity < 4
                                                          ? atoi(quantities[selectedQuantity])
                                                          : atoi(customQuantity);

                        StoreDetours::bShouldManualSell = true;
                    }

                    if (ImGui::BeginPopup("##cannotSell")) {
                        ImGui::Text("Cannot sell, try again");
                        if (ImGui::Button("Close")) {
                            cannotSell = false;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }

                    if (cannotSell) {
                        ImGui::OpenPopup("##cannotSell");
                    }
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            if (ImGui::CollapsingHeader("Auto Sell##AutoSellHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    Store_AutoSellSettings::Show();
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Villager Guild Stores##GuildStoresHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    ImGuiStyle style = ImGui::GetStyle();
                    float width = ImGui::CalcItemWidth();
                    float spacing = style.ItemInnerSpacing.x;
                    float buttonSize = ImGui::GetFrameHeight();

                    static int selectedStore = 2;

                    ImGui::Text("Open Remote Stores");

                    ImGui::PushItemWidth(width - spacing * 2.0f - buttonSize * 2.0f); //ImGui::PushItemWidth(CalcTextSize(stores[2])).x + style.FramePadding.x * 2.0f);
                    ImGui::SetNextItemWidth(250.0f);
                    if (ImGui::BeginCombo("##VillagerStores", StoreDetours::GuildStores[selectedStore].c_str())) {
                        for (auto& it : StoreDetours::GuildStores) {
                            const int guildStore = it.first;
                            const bool is_selected = (selectedStore == guildStore);
                            if (ImGui::Selectable(it.second.c_str(), is_selected)) {
                                selectedStore = guildStore;
                            }
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::PopItemWidth();

                    if (WidgetHandler::GameplayUIManager && WidgetHandler::GameplayUIManager->IsValidLowLevel()) {
                        ImGui::SameLine();
                        IMGUI_BUTTON("Open Guild Store",
                            StoreDetours::Func_OpenGuildStore(ValeriaCharacter, selectedStore);
                            show = false;
                        );
                    } else {
                        ImGui::SameLine();
                        ImGui::Text("Couldn't retrieve stores.");
                        // ======== END OF GUILD STORES
                    }

                    ImGui::PushItemWidth(width - spacing * 2.0f - buttonSize * 2.0f); //ImGui::PushItemWidth(CalcTextSize(stores[2])).x + style.FramePadding.x * 2.0f);
                    ImGui::SetNextItemWidth(250.0f);
                    if (ImGui::BeginCombo("##BuildingStores", StoreDetours::ConfigStores[StoreDetours::SelectedStoreConfig].c_str())) {
                        for (auto& it : StoreDetours::ConfigStores) {
                            const std::string configStore = it.first;
                            const bool is_selected = (StoreDetours::SelectedStoreConfig == configStore);
                            if (ImGui::Selectable(it.second.c_str(), is_selected)) {
                                StoreDetours::SelectedStoreConfig = configStore;
                            }
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::PopItemWidth();

                    if (WidgetHandler::GameplayUIManager && WidgetHandler::GameplayUIManager->IsValidLowLevel()) {
                        ImGui::SameLine();
                        IMGUI_BUTTON("Open Building Store",
                            StoreDetours::bShouldOpenConfigStore = true;
                            show = false;
                        );
                    } else {
                        ImGui::SameLine();
                        ImGui::Text("Couldn't retrieve stores.");
                        // ======== END OF GUILD STORES
                    }
                    ImGui::Text("Some stores are only available at night.");
                    ImGui::Text("Opening during off hours will show no inventory, or the inventory of a previously opened store.");
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            if (ImGui::CollapsingHeader("Player Features##PlayerSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    if (WidgetHandler::GameplayUIManager && WidgetHandler::GameplayUIManager->IsValidLowLevel()) {
                        IMGUI_BUTTON("Open Player Storage",
                            WidgetHandler::GameplayUIManager->Implementation_OpenPlayerStorage();
                            show = false;
                        );
                    }
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }
        }
        // ==================================== 5 Mods & Settings TAB
        else if (OpenTab == 5) {
            ImGui::Columns(2, nullptr, false);

            if (ImGui::CollapsingHeader("Character Settings##CharacterSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    IMGUI_CHECKBOX("Anti AFK", &Configuration::bEnableAntiAfk, "Stop inactivity disconnects and play forever.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    ImGui::Spacing();
                    IMGUI_CHECKBOX("Unlimited Wardrobe Respecs", &Configuration::bEnableUnlimitedWardrobeRespec, "Allows yous to change genders/appereances without the cooldown.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    ImGui::Spacing();

                    IMGUI_CHECKBOX("Further Interaction Distance", &Configuration::bEnableInteractionMods, "Interact with things from further distances.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    if (Configuration::bEnableInteractionMods) {
                        float step = 1.0f;
                        float step_fast = 100.0f;
                        ImGui::SetNextItemWidth(200.0f);
                        if (ImGui::InputScalar("Interaction Distance", ImGuiDataType_Float, &Configuration::InteractionRadius, &step, &step_fast, "%.1f", ImGuiInputTextFlags_None)) {
                            Configuration::InteractionRadius = std::clamp(Configuration::InteractionRadius, 1.0f, 99999.0f);
                            Configuration::Save(ESaveFile::ModSettings);
                        }
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                            ImGui::SetTooltip("Adjust the maximum distance for interactions. Range: 1.0 to 99999.0");
                    }

                    ImGui::Spacing();
                    IMGUI_CHECKBOX("Teleport To Waypoints", &Configuration::bEnableWaypointTeleport, "Automatically teleports you at your waypoints locations.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    ImGui::Spacing();
                    IMGUI_CHECKBOX("Easy Quest Mode", &Configuration::bEasyModeActive, "Experimental - May skip some questing requirements.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    ImGui::Spacing();
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            if (ImGui::CollapsingHeader("Origin Settings##OriginSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                IMGUI_CHECKBOX("Show Watermark", &Configuration::bShowWatermark, "Toggle display of the custom watermark on the screen.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                ImGui::Spacing();
                IMGUI_CHECKBOX("Active Mods Window", &Configuration::showEnabledFeaturesWindow, "Display a list of currently activated mods", ImGui::CheckboxHotkeyFlags::HOTKEY)
                ImGui::Spacing();
                ImGui::SetNextItemWidth(200.0f);
                if (ImGui::Combo("##ThemesCombo", &Configuration::selectedTheme, themes, IM_ARRAYSIZE(themes))) {
                    switch (Configuration::selectedTheme) {
                    case 0: // Origin
                        ImGui::StyleColorsDark();
                        Configuration::Save(ESaveFile::OverlaySettings);
                        break;
                    case 1: // Spectrum Light
                        ImGui::Spectrum::StyleColorsSpectrum();
                        Configuration::Save(ESaveFile::OverlaySettings);
                        break;
                    case 2: // Magma
                        ImGui::Magma::StyleColorsMagma();
                        Configuration::Save(ESaveFile::OverlaySettings);
                        break;
                    case 3: // Custom theme
                        Configuration::ApplyCustomTheme();
                        Configuration::Save(ESaveFile::OverlaySettings);
                        break;
                    default:
                        break;
                    }
                }
                ImGui::SameLine();
                ImGui::Text("Select Theme");

                ImGuiStyle* style = &ImGui::GetStyle();
                auto guiColor = style->Colors;

                static int selectedColor = -1;
                static std::string currentColorName = "Select a widget";

                static bool openCustomThemePopup = false;

                if (Configuration::selectedTheme != 3) {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, (Configuration::selectedTheme == 3));
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, style->Alpha * 0.25f);
                    ImGui::Button("Edit Custom Theme");
                    ImGui::PopItemFlag();
                    ImGui::PopStyleVar();

                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Select 'Custom Theme' to begin editing.");
                } else {
                    if (ImGui::Button("Edit Custom Theme")) {
                        openCustomThemePopup = true;

                        previousColors.clear(); // Ensuring previousColors is fresh.

                        for (int i = 0; i < ImGuiCol_COUNT; ++i) {
                            ImVec4 col = style->Colors[i];
                            previousColors[i] = col;
                        }
                    }
                }

                if (ImGui::BeginPopupModal("Custom Theme Editor##CustomThemeEditor", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
                    if (ImGui::BeginCombo("Widgets", currentColorName.c_str())) {
                        for (const auto& colorPair : GuiColors) {
                            int colorEnum = colorPair.first;
                            const std::string& colorName = colorPair.second;

                            bool isSelected = (selectedColor == colorEnum);
                            if (ImGui::Selectable(colorName.c_str(), isSelected)) {
                                selectedColor = colorEnum;
                                currentColorName = colorName;
                            }
                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    if (selectedColor != -1) {
                        ImVec4& color = style->Colors[selectedColor];

                        ImGui::Text("%s color", currentColorName.c_str());
                        ImGui::SameLine();
                        if (ImGui::ColorEdit4("##GuiColorPicker", reinterpret_cast<float*>(&color), ImGuiColorEditFlags_NoInputs)) {
                            auto colorIt = Configuration::customColors.find(selectedColor);
                            if (colorIt != Configuration::customColors.end()) { // Check if color already exists in customColors & replace instead of inserting.
                                if (colorIt->second != color) {
                                    colorIt->second = color;
                                }
                            } else {
                                Configuration::customColors.insert(std::make_pair(selectedColor, color));
                            }
                            style->Colors[selectedColor] = color;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Undo##UndoGuiColor")) {
                            // Undo colour function
                            auto it = previousColors.find(selectedColor);
                            auto colorIt = Configuration::customColors.find(selectedColor);
                            if (it != previousColors.end()) {
                                style->Colors[selectedColor] = it->second;
                                colorIt->second = it->second;
                            }
                        }
                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip("Undo Changes.");

                        ImGui::BeginGroupPanel("Widget Preview");
                        {
                            ImGui::Spacing();

                            // Widget preview
                            GetWidgetForPreview(selectedColor);

                            ImGui::Spacing();
                        }
                        ImGui::EndGroupPanel();
                    }

                    ImGui::Separator();
                    // Calculate the center position buttons
                    float windowWidth = ImGui::GetWindowSize().x;
                    float buttonWidth = 160.0f * 2 + ImGui::GetStyle().ItemSpacing.x;
                    ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

                    // Save button
                    if (ImGui::Button("Save", ImVec2(160, 0))) {
                        // Save to config.
                        Configuration::Save(ESaveFile::CustomThemeSettings);
                        openCustomThemePopup = false;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SetItemDefaultFocus();
                    ImGui::SameLine();

                    // Cancel button
                    if (ImGui::Button("Close Without Saving", ImVec2(160, 0))) {
                        // Restore old colors.
                        auto colorIt = Configuration::customColors.find(selectedColor);
                        for (auto& it : previousColors) {
                            int idx = it.first;
                            ImVec4 col = it.second;
                            style->Colors[idx] = col;
                            colorIt->second = col;
                        }

                        previousColors.clear();
                        openCustomThemePopup = false;
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                if (openCustomThemePopup) {
                    ImGui::OpenPopup("Custom Theme Editor##CustomThemeEditor");
                }
            }

            ImGui::NextColumn();

            // Fun Mods - Entities column
            if (ImGui::CollapsingHeader("Fun Settings##FunModsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    IMGUI_CHECKBOX("Advanced Placement", &Configuration::bPlaceAnywhere, "Allow for placement of housing items anywhere.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    ImGui::Spacing();
                    if (Configuration::bPlaceAnywhere) {
                        ImGui::SetNextItemWidth(200.0f);
                        IMGUI_SLIDER("Caps Lock Rotation", &Configuration::PlacementRotation, 1.0, 90.0, "%1.0f", "While caps lock is on, rotate by this amount instead.")
                        ImGui::Spacing();
                    }
                    // Temporarily disable outfits
                    // IMGUI_SUPPORTER_FEATURE("Temporarily Unlock Outfits", &Configuration::bEnableOutfitCustomization, "Temporarily unlocks premium outfits.", "checkbox", NULL, NULL, NULL, ImGui::CheckboxHotkeyFlags::HOTKEY)
                    // ImGui::Spacing();
                    if (Configuration::bEnableOutfitCustomization) {
                        ImGui::Text("To finish enabling outfits & collections, you need to open the Premium Store and let it load.");
                        ImGui::Spacing();
                    }
                    IMGUI_SUPPORTER_FEATURE("Temporarily Unlock Content", &Configuration::bEnableContentUnlocker, "Temporarily unlocks some content like furniture and colors.", "checkbox", NULL, NULL, NULL, ImGui::CheckboxHotkeyFlags::HOTKEY)
                    ImGui::Spacing();
                    // Disabled until fixed
                    // IMGUI_SUPPORTER_FEATURE("Unlock Landscapes", &Configuration::bEnableFreeLandscape, "Allows you to activate any landscape.", "checkbox", NULL, NULL, NULL, ImGui::CheckboxHotkeyFlags::HOTKEY)
                    // ImGui::Spacing();
                    IMGUI_CHECKBOX("Show All Villager Weekly Gifts", &Configuration::bEnableShowWeeklyWants, "Show all the gifts Villagers want this week without learning about them.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    ImGui::Spacing();
                    IMGUI_CHECKBOX("Unrepair Tool For Gold", &Configuration::bEnableToolBricker, "Unrepair your tool of choice for max gold in return.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    ImGui::Spacing();
                    IMGUI_CHECKBOX("Fulfill Infinite Player Requests", &Configuration::bEnableRequestMods, "Fulfill as many requests as you want. May require multiple tries.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    //ImGui::Spacing();
                    //IMGUI_CHECKBOX("Show Unseen Items For Requests", &Configuration::bEnableUnseenItems, "Show items you haven't seen yet in the Request Panel", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    ImGui::Spacing();
                    IMGUI_CHECKBOX("Black Market Prize Wheel", &Configuration::bEnablePrizeWheel, "Choose which positions on the wheel to accept.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                    ImGui::Spacing();
                    if (Configuration::bEnablePrizeWheel) {
                        IMGUI_CHECKBOX("Use Locked Storage", &Configuration::bPrizeWheelUseLocked, "Use locked storage instead of general storage.", ImGui::CheckboxHotkeyFlags::HOTKEY)
                        ImGui::BeginGroupPanel("Prize Wheel Positions");
                        {
                            ImGui::Spacing();
                            IMGUI_CHECKBOX("0| Red    - Fireworks", &Configuration::PrizeWheelSlots[0], ImGui::CheckboxHotkeyFlags::HOTKEY)
                            IMGUI_CHECKBOX("1| Purple - Decor", &Configuration::PrizeWheelSlots[1], ImGui::CheckboxHotkeyFlags::HOTKEY)
                            IMGUI_CHECKBOX("2| Blue   - Ingredients", &Configuration::PrizeWheelSlots[2], ImGui::CheckboxHotkeyFlags::HOTKEY)
                            IMGUI_CHECKBOX("3| Green  - Gold", &Configuration::PrizeWheelSlots[3], ImGui::CheckboxHotkeyFlags::HOTKEY)
                            IMGUI_CHECKBOX("4| Red    - Fireworks", &Configuration::PrizeWheelSlots[4], ImGui::CheckboxHotkeyFlags::HOTKEY)
                            IMGUI_CHECKBOX("5| Brown  - Lucky Coins", &Configuration::PrizeWheelSlots[5], ImGui::CheckboxHotkeyFlags::HOTKEY)
                            IMGUI_CHECKBOX("6| Blue   - Ingredients", &Configuration::PrizeWheelSlots[6], ImGui::CheckboxHotkeyFlags::HOTKEY)
                            IMGUI_CHECKBOX("7| Green  - Gold", &Configuration::PrizeWheelSlots[7], ImGui::CheckboxHotkeyFlags::HOTKEY)
                            IMGUI_CHECKBOX("8| Yellow - Plushie", &Configuration::PrizeWheelSlots[8], ImGui::CheckboxHotkeyFlags::HOTKEY)
                            ImGui::Spacing();
                        }
                        ImGui::EndGroupPanel();
                        ImGui::Spacing();
                    }
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }
        }
    }

    ImGui::End();

    if (!show)
        Menu::bShowMenu = false;
}
