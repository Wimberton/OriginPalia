#include "PaliaOverlay.h"
#include "Configuration.h"
#include "ActorHandler.h"

#include <SDK/Palia_parameters.hpp>

#include "Misc/ImGuiExt.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"

#include "menu/menu.hpp"
#include "fonts/fonts.h"

#include <algorithm>
#include <imgui_internal.h>
#include <locale>
#include <codecvt>

#include "Detours/AimDetours.h"
#include "Detours/BlackMarketDetours.h"
#include "Detours/FishingDetours.h"
#include "Detours/HUDDetours.h"
#include "Detours/MovementDetours.h"
#include "Detours/TeleportDetours.h"
#include "Detours/WidgetDetours.h"

using namespace SDK;

void DisplayModsCategory(const std::string& title, const std::vector<std::pair<std::string, bool>>& mods) {
    bool hasMods = std::ranges::any_of(mods, [](const auto& mod) { return mod.second; });
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
#ifdef ENABLE_SUPPORTER_FEATURES
                {"Content Unlocker", Configuration::bEnableOutfitCustomization},
#endif
                {"Cooking Mods", Configuration::bEnableMinigameSkip},
                {"Interaction Mods", Configuration::bEnableInteractionMods},
                {"Request Mods", Configuration::bEnableRequestMods},
                {"PrizeWheel Mods", Configuration::bEnablePrizeWheel}
                });

            DisplayModsCategory("ESP Mods", {
                {"ESP", Configuration::bEnableESP},
                {"ESP Icons", Configuration::bESPIcons},
                {"InteliAim Circle", Configuration::bDrawFOVCircle}
                });

            DisplayModsCategory("Aiming Mods", {
                {"Silent Aimbot", Configuration::bEnableSilentAimbot},
                {"Legacy Aimbot", Configuration::bEnableAimbot}
                });

            DisplayModsCategory("Teleport Mods", {
                {"TP to Targeted", Configuration::bTeleportToTargeted},
                {"TP to Waypoint", Configuration::bEnableWaypointTeleport},
                {"TP Avoid Players", Configuration::bAvoidTeleportingToPlayers},
                {"TP Loot to Player", Configuration::bEnableLootbagTeleportation},
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
        }
        else {
            ImGui::Text("Waiting for in-game...");
        }

        ImVec2 currentPos = ImGui::GetWindowPos();
        if (currentPos.x != Configuration::activeWindowPosX || currentPos.y != Configuration::activeWindowPosY) {
            Configuration::activeWindowPosX = currentPos.x;
            Configuration::activeWindowPosY = currentPos.y;
            Configuration::Save();
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
        ImGui::End();
    }
}

void PaliaOverlay::SetupHooks() {
    gDetourManager.SetupHooks();
    gDetourManager.InitFunctions();
};

inline ImVec2 addImVec2(ImVec2 a, ImVec2 b) {
    return ImVec2(a.x + b.x, a.y + b.y);
}

inline void drawTextOutlined(ImDrawList* drawList, ImVec2 pos, ImU32 color, const char* text, ImFont* font) {
    if (!text) return;

    drawList->AddText(font, font->FontSize, addImVec2(pos, ImVec2(1, 1)), IM_COL32(0, 0, 0, 170), text);
    drawList->AddText(font, font->FontSize, addImVec2(pos, ImVec2(1, -1)), IM_COL32(0, 0, 0, 170), text);
    drawList->AddText(font, font->FontSize, addImVec2(pos, ImVec2(-1, 1)), IM_COL32(0, 0, 0, 170), text);
    drawList->AddText(font, font->FontSize, addImVec2(pos, ImVec2(-1, -1)), IM_COL32(0, 0, 0, 170), text);
    drawList->AddText(font, font->FontSize, pos, color, text);
}

void PaliaOverlay::DrawGuiFOVCircle() {
    if (!Configuration::bDrawFOVCircle) return;

    const auto PlayerController = GetPlayerController();
    if (!PlayerController || PlayerController->K2_GetActorLocation().IsZero())
        return;

    FVector PawnLocation = PlayerController->K2_GetPawn()->K2_GetActorLocation();
    FVector2D PlayerScreenPosition;
    FVector2D TargetScreenPosition;

    if (!PlayerController->ProjectWorldLocationToScreen(PawnLocation, &PlayerScreenPosition, true))
        return;

    FVector2D FOVCenter = { ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f };
    if (FOVCenter.X <= 0 || FOVCenter.Y <= 0)
        return;

    static auto drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return;

    if (!Configuration::bHideFOVCircle) {
        ImU32 CircleColor = IM_COL32(0xff, 0xff, 0xff, static_cast<int>(Configuration::CircleAlpha * 255));
        drawList->AddCircle(ImVec2(FOVCenter.X, FOVCenter.Y), Configuration::FOVRadius, CircleColor, 1200, 1.0f);
    }

    if (Configuration::bDrawCrosshair) {
        FVector2D CrossHairRadius = { 3.f, 3.f };
        ImU32 CrosshairColor = IM_COL32(0x7e, 0x55, 0xc0, static_cast<int>(Configuration::CircleAlpha * 255));
        drawList->AddCircleFilled(ImVec2(FOVCenter.X, FOVCenter.Y), 3.0f, CrosshairColor, 60);
    }

    if (AimDetours::BestTargetLocation.IsZero())
        return;
    if (!PlayerController->ProjectWorldLocationToScreen(AimDetours::BestTargetLocation, &TargetScreenPosition, true))
        return;
    if (!(CustomMath::DistanceBetweenPoints(TargetScreenPosition, FOVCenter) <= Configuration::FOVRadius))
        return;

    ImU32 TargetLineColor = IM_COL32(0xff, 0xff, 0xff, static_cast<int>(Configuration::CircleAlpha * 255));
    drawList->AddLine(ImVec2(FOVCenter.X, FOVCenter.Y), ImVec2(TargetScreenPosition.X, TargetScreenPosition.Y), TargetLineColor, 0.5f);

    // Drawing a purple circle at the end of the line
    ImU32 EndCircleColor = IM_COL32(0x40, 0x19, 0x80, static_cast<int>(Configuration::CircleAlpha * 255));
    drawList->AddCircleFilled(ImVec2(TargetScreenPosition.X, TargetScreenPosition.Y), 2.0f, EndCircleColor, 60);
}

void PaliaOverlay::DrawGuiESP() {
	if (!Configuration::bEnableESP) {
		return;
	}

	const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter || ValeriaCharacter->K2_GetActorLocation().IsZero())
        return;

	FVector PawnLocation = ValeriaCharacter->K2_GetActorLocation();

    if (ActorHandler::GetActors().empty())
        return;

	const FVector2D CanvasSize = { ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y };

	const auto drawList = ImGui::GetBackgroundDrawList();

    const auto narrowString = [](const std::wstring& wstr) -> std::string {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wstr);
    };

	// Draw ESP Names Entities
	for (auto& [Actor, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, Distance, Texture] : ActorHandler::GetActors()) {
        if (!Actor || !IsActorValid(Actor) || WorldPosition.IsZero())
            continue;

        // only update location on movable pawns, otherwise use cached
        if (EType::MoveablePawn & ActorType ||
            WorldPosition.IsZero())
        {
            WorldPosition = Actor->K2_GetActorLocation();
            if (WorldPosition.IsZero()) continue;
        }
        Distance = PawnLocation.GetDistanceToInMeters(WorldPosition);

		if (ActorType == EType::Players) {
			if (Actor == ValeriaCharacter)
				continue; // Skip local player.
		}

        const auto PlayerController = GetPlayerController();
        if (!PlayerController)
            break;

		FVector ActorLocation = WorldPosition;

		// Adjust Z coordinate for head-level display
		float HeightAdjustment = 100.0f; // Adjust this value based on typical actor height
		ActorLocation.Z += HeightAdjustment;

		if (Configuration::bEnableESPCulling && Distance > Configuration::CullDistance)
			continue;

		ImU32 Color = IM_COL32(0xFF, 0xFF, 0xFF, 0xFF);
		bool bShouldDraw = false;

		bool bShouldDrawStar = false;
		bool bIconShouldUseColor = false;
		bool bOverlayText = false;
		std::string OverlayText;

        std::string FAIconText;

		switch (ActorType) {
		case EType::Forage:
			if (HUDDetours::Forageables[Type][Quality]) {
				if (!Actor->bActorEnableCollision) {
					continue;
				}
				bShouldDraw = !Configuration::bESPIcons;
                if (auto it = Configuration::ForageableColors.find(static_cast<EForageableType>(Type)); it != Configuration::ForageableColors.end())
                    Color = it->second;
				if (Quality == 1) {
					bShouldDrawStar = true;
				}
                FAIconText = ICON_FA_LEAF;
			}
			break;
		case EType::Ore:
			if (HUDDetours::Ores[Type][Variant]) {
				auto Ore = static_cast<ABP_ValeriaGatherableLoot_C*>(Actor);
				if (Ore && IsActorValid(Ore)) {
					if (Ore->IAmAlive) {
                        bShouldDraw = !Configuration::bESPIcons;
                        if (auto it = Configuration::OreColors.find(static_cast<EOreType>(Type)); it != Configuration::OreColors.end())
                            Color = it->second;
						bOverlayText = true;
						OverlayText = GatherableSizeNames[Variant];
                        FAIconText = ICON_FA_GEM;
					}
				}
			}
			break;
		case EType::Players:
			if (HUDDetours::Singles[static_cast<int>(EOneOffs::Player)]) {
				if (DisplayName.empty()) continue;
                bShouldDraw = true;
                if (auto it = Configuration::SingleColors.find(EOneOffs::Player); it != Configuration::SingleColors.end())
                    Color = it->second;
                if (!Configuration::bESPIcons)
                    FAIconText = ICON_FA_USER;
			}
			break;
		case EType::Animal:
			if (HUDDetours::Animals[Type][Variant]) {
                bShouldDraw = !Configuration::bESPIcons;
                if (auto it = Configuration::AnimalColors.find(FCreatureType(static_cast<ECreatureKind>(Type), static_cast<ECreatureQuality>(Variant))); it != Configuration::AnimalColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_PAW;
			}
			break;
		case EType::Tree:
			if (HUDDetours::Trees[Type][Variant]) {
				auto Tree = static_cast<ABP_ValeriaGatherableLoot_C*>(Actor);
				if (Tree && IsActorValid(Tree)) {
					if (Tree->IAmAlive) {
                        bShouldDraw = !Configuration::bESPIcons;
                        if (auto it = Configuration::TreeColors.find(static_cast<ETreeType>(Type)); it != Configuration::TreeColors.end())
                            Color = it->second;
						bOverlayText = true;
						OverlayText = GatherableSizeNames[Variant];
                        FAIconText = ICON_FA_TREE;
					}
				}
			}
			break;
		case EType::Bug:
			if (HUDDetours::Bugs[Type][Variant][Quality]) {
                bShouldDraw = !Configuration::bESPIcons;
                if (auto it = Configuration::BugColors.find(FBugType(static_cast<EBugKind>(Type), static_cast<EBugQuality>(Variant))); it != Configuration::BugColors.end())
                    Color = it->second;
				if (Quality == 1) {
					bShouldDrawStar = true;
                    FAIconText = ICON_FA_BUG;
				}
                FAIconText = ICON_FA_BUG;
			}
			break;
		case EType::NPCs:
			if (HUDDetours::Singles[static_cast<int>(EOneOffs::NPC)]) {
                bShouldDraw = !Configuration::bESPIcons;
                if (auto it = Configuration::SingleColors.find(EOneOffs::NPC); it != Configuration::SingleColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_PERSON;
			}
			break;
		case EType::Loot:
			if (HUDDetours::Singles[static_cast<int>(EOneOffs::Loot)]) {
                bShouldDraw = !Configuration::bESPIcons;
                if (auto it = Configuration::SingleColors.find(EOneOffs::Loot); it != Configuration::SingleColors.end())
                    Color = it->second;
				bIconShouldUseColor = true;
                FAIconText = ICON_FA_SACK;
			}
			break;
		case EType::Quest:
			if (HUDDetours::Singles[static_cast<int>(EOneOffs::Quest)]) {
				if (Distance > 250) continue; // quests don't always update collision until within this range
				if (!Actor->bActorEnableCollision) {
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
				auto Pile = static_cast<ATimedLootPile*>(Actor);
				if (!Pile || !IsActorValid(Pile)) {
					bShouldDraw = false;
					break;
				}

				if (Configuration::bEnableOthers) {
                    bShouldDraw = !Configuration::bESPIcons;
					Color = Pile->bActivated ? IM_COL32(0xFF, 0xFF, 0xFF, 0xFF) : IM_COL32(0xFF, 0x00, 0x00, 0xFF);
                    FAIconText = ICON_FA_SQUARE_QUESTION;
				}
				else if (Pile->CanGather(ValeriaCharacter) && Pile->bActivated) {
                    bShouldDraw = !Configuration::bESPIcons;
                    if (auto it = Configuration::SingleColors.find(EOneOffs::RummagePiles); it != Configuration::SingleColors.end())
                        Color = it->second;
                    FAIconText = ICON_FA_BALL_PILE;
				}
			}
			break;
		case EType::Stables:
			if (HUDDetours::Singles[static_cast<int>(EOneOffs::Stables)]) {
                bShouldDraw = !Configuration::bESPIcons;
                if (auto it = Configuration::SingleColors.find(EOneOffs::Stables); it != Configuration::SingleColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_TORII_GATE;
			}
			break;
		case EType::Fish:
			if (HUDDetours::Fish[Type]) {
				bShouldDraw = true;
                if (auto it = Configuration::FishColors.find(static_cast<EFishType>(Type)); it != Configuration::FishColors.end())
                    Color = it->second;
                FAIconText = ICON_FA_FISH;
			}
			break;
		case EType::Treasure:
			if (HUDDetours::Singles[static_cast<int>(EOneOffs::Treasure)]) {
				if (Distance > 150) continue; // Chests don't seem to properly setup collision until in update range
				if (!Actor->bActorEnableCollision) {
					continue;
				}

				auto Treasure = static_cast<AWorldPersistGatherable*>(Actor);
				if (!Treasure || !IsActorValid(Treasure)) break;

				if (Configuration::bEnableOthers) {
                    bShouldDraw = !Configuration::bESPIcons;
                    if (auto it = Configuration::SingleColors.find(EOneOffs::Treasure); it != Configuration::SingleColors.end())
                        Color = it->second;
                    FAIconText = ICON_FA_TREASURE_CHEST;
				}
				else if (Treasure->CanGather(ValeriaCharacter)) {
                    bShouldDraw = !Configuration::bESPIcons;
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
				if (Variant > 1) {
					bOverlayText = true;
					OverlayText = std::format("{}", Variant);
                    FAIconText = ICON_FA_SQUARE_QUESTION;
				}
			}
			break;
		default:
			break;
		}

		if (Configuration::bEnableOthers && Type == 0)
			bShouldDraw = true;

		if (!bShouldDraw)
			continue;
        
        FVector2D ScreenLocation;
        try {
            if (PlayerController->ProjectWorldLocationToScreen(ActorLocation, &ScreenLocation, true)) {
                if (ScreenLocation.X < 0 || ScreenLocation.Y < 0 || ScreenLocation.X > CanvasSize.X || ScreenLocation.Y > CanvasSize.Y) continue;
                // Construct text string
                std::wstring qualityName = (Quality > 0) ? PaliaOverlay::GetQualityName(Quality, ActorType) : L"";

                std::vector<std::wstring> IconText{};
                std::wstring IconAboveText;
                // Prepare text with optional parts depending on the index values
                std::wstring text = DisplayName;
                if (!qualityName.empty()) {
                    text += L" " + qualityName;
                }
                text += std::format(L" [{:.0f}m]", Distance);

                // TODO: Still crashing
                // if (Configuration::bEnableDespawnTimer) {
                //     double seconds = 0;
                //
                //     if (IsActorValid(Actor)) {
                //         if (ActorType == EType::Ore) {
                //             auto GatherableLoot = static_cast<ABP_ValeriaGatherableLoot_Mining_MultiHarvest_C*>(Actor);
                //             if (GatherableLoot && IsActorValid(GatherableLoot)) {
                //                 if (GatherableLoot->GatherableLoot && GatherableLoot->GatherableLoot->IsValidLowLevel()) {
                //                     GatherableLoot->GetSecondsUntilDespawn(&seconds);
                //                 }
                //             }
                //         }
                //         else if (ActorType == EType::Forage) {
                //             auto ForageableLoot = static_cast<ABP_ValeriaGatherable_C*>(Actor);
                //             if (ForageableLoot && IsActorValid(ForageableLoot)) {
                //                 if (ForageableLoot->Gatherable && ForageableLoot->Gatherable->IsValidLowLevel()) {
                //                     seconds = ForageableLoot->Gatherable->GetSecondsUntilDespawn();
                //                 }
                //             }
                //         }
                //     }
                //
                //     if (seconds > 0) {
                //         text += std::format(L" {:.0f}s", seconds);
                //         IconText.push_back(std::format(L"{:.0f}s", seconds));
                //     }
                // }

                std::string shortText = FAIconText + " " + narrowString(text);

                ImVec2 textSize = ImGui::CalcTextSize(shortText.c_str());

                const float rectPadding = 3.0f;

                ImVec2 boxSize = ImVec2(textSize.x + rectPadding * 2, textSize.y + rectPadding * 2);
                ImVec2 boxPosition = ImVec2(ScreenLocation.X - boxSize.x / 2, ScreenLocation.Y - boxSize.y / 2);

                ImVec2 TextPosition = ImVec2(ScreenLocation.X - textSize.x / 2, ScreenLocation.Y - textSize.y / 2);
                ImVec2 ShadowPosition = ImVec2(TextPosition.x + 1.5f, TextPosition.y + 1.5f);

                // Constrain text to screen before disappearing.
                if (TextPosition.x < 4.0f) { // left side
                    TextPosition.x = 4.0f;
                }
                else if (TextPosition.x + textSize.x > CanvasSize.X) { // right side
                    TextPosition.x = CanvasSize.X - textSize.x;
                }
                if (TextPosition.y < 4.0f) { // top side
                    TextPosition.y = 4.0f;
                }
                else if (TextPosition.y + textSize.y > CanvasSize.Y - 4.0f) { // bottom side
                    TextPosition.y = CanvasSize.Y - textSize.y - 4.0f;
                }

                //drawList->AddRectFilled(boxPosition, ImVec2(boxPosition.x + boxSize.x, boxPosition.y + boxSize.y), IM_COL32(0, 0, 0, 190));

                ImFont* font = Menu::ESPFont;
                ImGui::PushFont(font);
                drawTextOutlined(drawList, TextPosition, Color, shortText.c_str(), font);
                ImGui::PopFont();
            }
        }
        catch (std::exception& e) {
            std::cout << "ProjectWorldLocationToScreen exception: " << e.what() << "\n";
        }
	}
}

static bool getTheme = true;
void PaliaOverlay::DrawHUD() {
    auto Overlay = static_cast<PaliaOverlay*>(Instance);

    Configuration::Load(Overlay);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard navigation once at initialization if possible.

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 0.35f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);

    std::string defaultWatermarkText = "OriginPalia - discord.gg/OriginSoftware - Game V0.181.0" + (DetourManager::CurrentMap.empty() ? "" : " - " + DetourManager::CurrentMap);
    bool showWatermark = false;
    if ((DetourManager::CurrentLevel && (DetourManager::CurrentMap == "MAP_PreGame" || DetourManager::CurrentMap == "Unknown")) || Configuration::bShowWatermark) {
        if (DetourManager::CurrentMap == "MAP_PreGame" || DetourManager::CurrentMap == "Unknown") {
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

    const auto WindowTitle = std::string("OriginPalia 3.0");
    auto Overlay = static_cast<PaliaOverlay*>(Instance);

    if (ImGui::Begin(WindowTitle.data(), &show, window_flags)) {
        static int OpenTab = 0;

        if (ImGui::IsMouseDragging(0) && (Configuration::windowSizeX != ImGui::GetWindowSize().x || Configuration::windowSizeY != ImGui::GetWindowSize().y)) { // dont want to spam save if you're just dragging your mouse.
            Configuration::windowSizeX = ImGui::GetWindowSize().x;
            Configuration::windowSizeY = ImGui::GetWindowSize().y;
            Configuration::Save();
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
            if (ImGui::BeginTabItem(ICON_FA_BASKET_SHOPPING " Selling & Items")) {
                OpenTab = 3;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(ICON_FA_SCREWDRIVER_WRENCH " Skills & Tools")) {
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
                
                IMGUI_CHECKBOX("Enable ESP", &Configuration::bEnableESP)
                IMGUI_CHECKBOX("Use Icons", &Configuration::bESPIcons, "Display icons instead of text on screen.")

                if (Configuration::bESPIcons) {
                    IMGUI_CHECKBOX("Show Distance with Icons", &Configuration::bESPIconDistance, "Displays the distance underneath the icon.")
                }

                /*
                * TODO: This is crashing
                */
                // if (ImGui::Checkbox("Enable Despawn Timer", &Configuration::bEnableDespawnTimer)) {
                //     Configuration::Save();
                // }
                
                /* 
                * Resizing font for ImGui is a major ballache.
                * TODO: Leaving it up to someone else ngl. ImGui has docs on this.
                */
                // ImGui::SetNextItemWidth(200.0f);
                // if (ImGui::SliderFloat("ESP Text Scale", &Configuration::ESPTextScale, 0.5f, 3.0f, "%.1f")) {
                //     Configuration::Save();
                // }
                // if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                //     ImGui::SetTooltip("Adjust the scale of ESP text size.");

                Configuration::CullDistance = std::clamp(Configuration::CullDistance, 10, 999);
                ImGui::SetNextItemWidth(200.0f);
                if (ImGui::InputInt("ESP Distance", &Configuration::CullDistance)) {
                    Configuration::CullDistance = std::clamp(Configuration::CullDistance, 10, 999);
                    Configuration::Save();
                }
            }

            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Animals##AnimalsSettingsHeader")) {

                if (ImGui::Button("Sernuk##SernukBtn")) {
                    HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)];
                    HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier2)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier2)];
                    HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier3)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier3)];

                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Chapaa##ChapaaBtn")) {
                    HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)];
                    HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier2)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier2)];
                    HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier3)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier3)];
                    HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Chase)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Chase)];

                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Muujin##MuujinBtn")) {
                    HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier1)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier1)];
                    HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier2)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier2)];
                    HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier3)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier3)];

                    Configuration::Save();
                }

                IMGUI_SETUP_TABLE("Animals", 3)
                {
                    IMGUI_TABLE_SETUP_COLUMN("Name", ImGuiTableColumnFlags_None, 0)
                        IMGUI_TABLE_SETUP_COLUMN("Show", ImGuiTableColumnFlags_WidthFixed, 40)
                        IMGUI_TABLE_SETUP_COLUMN("Color", ImGuiTableColumnFlags_WidthFixed, 40)

                        IMGUI_ADD_HEADERS("", "Show", "Color")
                        // SERNUKS
                        IMGUI_ADD_ROW("Sernuk",
                            IMGUI_CHECKBOX("##Sernuk", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)]),
                            IMGUI_COLORPICKER("##SernukColor", &Configuration::AnimalColors.find(FCreatureType(ECreatureKind::Cearnuk, ECreatureQuality::Tier1))->second)
                        )
                        IMGUI_ADD_ROW("Elder Sernuk",
                            IMGUI_CHECKBOX("##ElderSernuk", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier2)]),
                            IMGUI_COLORPICKER("##ElderSernukColor", &Configuration::AnimalColors.find(FCreatureType(ECreatureKind::Cearnuk, ECreatureQuality::Tier2))->second)
                        )
                        IMGUI_ADD_ROW("Proudhorn Sernuk",
                            IMGUI_CHECKBOX("##ProudhornSernuk", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier3)]),
                            IMGUI_COLORPICKER("##ProudhornSernukColor", &Configuration::AnimalColors.find(FCreatureType(ECreatureKind::Cearnuk, ECreatureQuality::Tier3))->second)
                        )

                        // CHAPAAS
                        IMGUI_ADD_HEADERS("", "Show", "Color")
                        IMGUI_ADD_ROW("Chapaa",
                            IMGUI_CHECKBOX("##Chapaa", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)]),
                            IMGUI_COLORPICKER("##ChapaaColor", &Configuration::AnimalColors.find(FCreatureType(ECreatureKind::Chapaa, ECreatureQuality::Tier1))->second)
                        )
                        IMGUI_ADD_ROW("Striped Chapaa",
                            IMGUI_CHECKBOX("##StripedChapaa", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier2)]),
                            IMGUI_COLORPICKER("##StripedChapaaColor", &Configuration::AnimalColors.find(FCreatureType(ECreatureKind::Chapaa, ECreatureQuality::Tier2))->second)
                        )
                        IMGUI_ADD_ROW("Azure Chapaa",
                            IMGUI_CHECKBOX("##AzureChapaa", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier3)]),
                            IMGUI_COLORPICKER("##AzureChapaaColor", &Configuration::AnimalColors.find(FCreatureType(ECreatureKind::Chapaa, ECreatureQuality::Tier3))->second)
                        )
                        IMGUI_ADD_ROW("Minigame Chapaa",
                            IMGUI_CHECKBOX("##MinigameChapaa", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Chase)]),
                            IMGUI_COLORPICKER("##MinigameChapaaColor", &Configuration::AnimalColors.find(FCreatureType(ECreatureKind::Chapaa, ECreatureQuality::Chase))->second)
                        )

                        // MUUJINS
                        IMGUI_ADD_HEADERS("", "Show", "Color")
                        IMGUI_ADD_ROW("Muujin",
                            IMGUI_CHECKBOX("##Muujin", &HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier1)]),
                            IMGUI_COLORPICKER("##MuujinColor", &Configuration::AnimalColors.find(FCreatureType(ECreatureKind::TreeClimber, ECreatureQuality::Tier1))->second)
                        )
                        IMGUI_ADD_ROW("Banded Muujin",
                            IMGUI_CHECKBOX("##BandedMuujin", &HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier2)]),
                            IMGUI_COLORPICKER("##BandedMuujinColor", &Configuration::AnimalColors.find(FCreatureType(ECreatureKind::TreeClimber, ECreatureQuality::Tier2))->second)
                        )
                        IMGUI_ADD_ROW("Bluebristle Muujin",
                            IMGUI_CHECKBOX("##BluebristleMuujin", &HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier3)]),
                            IMGUI_COLORPICKER("##BluebristleMuujinColor", &Configuration::AnimalColors.find(FCreatureType(ECreatureKind::TreeClimber, ECreatureQuality::Tier3))->second)
                        )
                }
                IMGUI_END_TABLE()
            }
            if (ImGui::CollapsingHeader("Ores##OresSettingsHeader")) {

                if (ImGui::Button("Clay##ClayBtn")) {
                    bool newState = !HUDDetours::Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(EGatherableSize::Large)];
                    for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                        HUDDetours::Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(size.first)] = newState;
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Stone##StoneBtn")) {
                    bool newState = !HUDDetours::Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Large)];
                    for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                        HUDDetours::Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(size.first)] = newState;
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Copper##CopperBtn")) {
                    bool newState = !HUDDetours::Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Large)];
                    for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                        HUDDetours::Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(size.first)] = newState;
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Iron##IronBtn")) {
                    bool newState = !HUDDetours::Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Large)];
                    for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                        HUDDetours::Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(size.first)] = newState;
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Palium##PaliumBtn")) {
                    bool newState = !HUDDetours::Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Large)];
                    for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                        HUDDetours::Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(size.first)] = newState;
                    }
                    Configuration::Save();
                }

                IMGUI_SETUP_TABLE("Ores", 5)
                {
                    IMGUI_TABLE_SETUP_COLUMN("Name", ImGuiTableColumnFlags_None, 0)
                        IMGUI_TABLE_SETUP_COLUMN("Sm", ImGuiTableColumnFlags_WidthFixed, 40)
                        IMGUI_TABLE_SETUP_COLUMN("Med", ImGuiTableColumnFlags_WidthFixed, 40)
                        IMGUI_TABLE_SETUP_COLUMN("Lg", ImGuiTableColumnFlags_WidthFixed, 40)
                        IMGUI_TABLE_SETUP_COLUMN("Color", ImGuiTableColumnFlags_WidthFixed, 40)

                        IMGUI_ADD_HEADERS("", "Sm", "Med", "Lg", "Color")

                        // CLAY
                        IMGUI_ADD_ROW("Clay",
                            IMGUI_TABLENEXTCOLUMN()
                            IMGUI_TABLENEXTCOLUMN()
                            IMGUI_CHECKBOX("##ClayLg", &HUDDetours::Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(EGatherableSize::Large)]),
                            IMGUI_COLORPICKER("##Clay", &Configuration::OreColors.find(EOreType::Clay)->second)
                        )
                        //Stone
                        IMGUI_ADD_ROW("Stone",
                            IMGUI_CHECKBOX("##StoneSm", &HUDDetours::Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Small)], true)
                            IMGUI_CHECKBOX("##StoneMed", &HUDDetours::Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Medium)], true)
                            IMGUI_CHECKBOX("##StoneLg", &HUDDetours::Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Large)]),
                            IMGUI_COLORPICKER("##Stone", &Configuration::OreColors.find(EOreType::Stone)->second)
                        )
                        //Copper
                        IMGUI_ADD_ROW("Copper",
                            IMGUI_CHECKBOX("##CopperSm", &HUDDetours::Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Small)], true)
                            IMGUI_CHECKBOX("##CopperMed", &HUDDetours::Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Medium)], true)
                            IMGUI_CHECKBOX("##CopperLg", &HUDDetours::Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Large)]),
                            IMGUI_COLORPICKER("##Copper", &Configuration::OreColors.find(EOreType::Copper)->second)
                        )
                        //Iron
                        IMGUI_ADD_ROW("Iron",
                            IMGUI_CHECKBOX("##IronSm", &HUDDetours::Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Small)], true)
                            IMGUI_CHECKBOX("##IronMed", &HUDDetours::Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Medium)], true)
                            IMGUI_CHECKBOX("##IronLg", &HUDDetours::Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Large)]),
                            IMGUI_COLORPICKER("##Iron", &Configuration::OreColors.find(EOreType::Iron)->second)
                        )
                        //Palium
                        IMGUI_ADD_ROW("Palium",
                            IMGUI_CHECKBOX("##PaliumSm", &HUDDetours::Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Small)], true)
                            IMGUI_CHECKBOX("##PaliumMed", &HUDDetours::Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Medium)], true)
                            IMGUI_CHECKBOX("##PaliumLg", &HUDDetours::Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Large)]),
                            IMGUI_COLORPICKER("##Palium", &Configuration::OreColors.find(EOreType::Palium)->second)
                        )
                }
                IMGUI_END_TABLE()
            }
            if (ImGui::CollapsingHeader("Forageables##ForageablesSettingsHeader")) {

                if (ImGui::Button("Common##Forage")) {
                    for (int pos : HUDDetours::ForageableCommon) {
                        HUDDetours::Forageables[pos][1] = HUDDetours::Forageables[pos][0] = !HUDDetours::Forageables[pos][0];
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Uncommon##Forage")) {
                    for (int pos : HUDDetours::ForageableUncommon) {
                        HUDDetours::Forageables[pos][1] = HUDDetours::Forageables[pos][0] = !HUDDetours::Forageables[pos][0];
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Rare##Forage")) {
                    for (int pos : HUDDetours::ForageableRare) {
                        HUDDetours::Forageables[pos][1] = HUDDetours::Forageables[pos][0] = !HUDDetours::Forageables[pos][0];
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Epic##Forage")) {
                    for (int pos : HUDDetours::ForageableEpic) {
                        HUDDetours::Forageables[pos][1] = HUDDetours::Forageables[pos][0] = !HUDDetours::Forageables[pos][0];
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Star##Forage")) {
                    for (auto& Forageable : HUDDetours::Forageables) {
                        Forageable[1] = !Forageable[1];
                    }
                    Configuration::Save();
                }

                IMGUI_SETUP_TABLE("Forageables", 4)
                {
                    IMGUI_TABLE_SETUP_COLUMN("Name", ImGuiTableColumnFlags_None, 0)
                        IMGUI_TABLE_SETUP_COLUMN("Normal", ImGuiTableColumnFlags_WidthFixed, 40)
                        IMGUI_TABLE_SETUP_COLUMN("Star", ImGuiTableColumnFlags_WidthFixed, 40)
                        IMGUI_TABLE_SETUP_COLUMN("Color", ImGuiTableColumnFlags_WidthFixed, 40)

                        // + BEACH
                        IMGUI_ADD_HEADERS("Beach", "Normal", "", "Color")
                        // Coral
                        IMGUI_ADD_ROW("Coral",
                            IMGUI_CHECKBOX("##Coral", &HUDDetours::Forageables[static_cast<int>(EForageableType::Coral)][0], true),
                            IMGUI_COLORPICKER("##CoralColor", &Configuration::ForageableColors.find(EForageableType::Coral)->second)
                        )
                        // Oyster
                        IMGUI_ADD_ROW("Oyster",
                            IMGUI_CHECKBOX("##Oyster", &HUDDetours::Forageables[static_cast<int>(EForageableType::Oyster)][0], true),
                            IMGUI_COLORPICKER("##OysterColor", &Configuration::ForageableColors.find(EForageableType::Oyster)->second)
                        )
                        // Shell
                        IMGUI_ADD_ROW("Shell",
                            IMGUI_CHECKBOX("##Shell", &HUDDetours::Forageables[static_cast<int>(EForageableType::Shell)][0], true),
                            IMGUI_COLORPICKER("##ShellColor", &Configuration::ForageableColors.find(EForageableType::Shell)->second)
                        )

                        // + FLOWER
                        IMGUI_ADD_HEADERS("Flower", "Normal", "Star", "Color")
                        // Briar Daisy
                        IMGUI_ADD_ROW("Briar Daisy",
                            IMGUI_CHECKBOX("##PoisonFlower", &HUDDetours::Forageables[static_cast<int>(EForageableType::PoisonFlower)][0], true)
                            IMGUI_CHECKBOX("##PoisonFlowerC", &HUDDetours::Forageables[static_cast<int>(EForageableType::PoisonFlower)][1]),
                            IMGUI_COLORPICKER("##PoisonFlower", &Configuration::ForageableColors.find(EForageableType::PoisonFlower)->second)
                        )
                        // Crystal Lake Lotus
                        IMGUI_ADD_ROW("Crystal Lake Lotus",
                            IMGUI_CHECKBOX("##WaterFlower", &HUDDetours::Forageables[static_cast<int>(EForageableType::WaterFlower)][0], true)
                            IMGUI_CHECKBOX("##WaterFlowerC", &HUDDetours::Forageables[static_cast<int>(EForageableType::WaterFlower)][1]),
                            IMGUI_COLORPICKER("##WaterFlower", &Configuration::ForageableColors.find(EForageableType::WaterFlower)->second)
                        )
                        // Heartdrop Lily
                        IMGUI_ADD_ROW("Heartdrop Lily",
                            IMGUI_CHECKBOX("##Heartdrop", &HUDDetours::Forageables[static_cast<int>(EForageableType::Heartdrop)][0], true)
                            IMGUI_CHECKBOX("##HeartdropC", &HUDDetours::Forageables[static_cast<int>(EForageableType::Heartdrop)][1]),
                            IMGUI_COLORPICKER("##Heartdrop", &Configuration::ForageableColors.find(EForageableType::Heartdrop)->second)
                        )
                        // Sundrop Lily
                        IMGUI_ADD_ROW("Sundrop Lily",
                            IMGUI_CHECKBOX("##Sundrop", &HUDDetours::Forageables[static_cast<int>(EForageableType::Sundrop)][0], true)
                            IMGUI_CHECKBOX("##SundropC", &HUDDetours::Forageables[static_cast<int>(EForageableType::Sundrop)][1]),
                            IMGUI_COLORPICKER("##Sundrop", &Configuration::ForageableColors.find(EForageableType::Sundrop)->second)
                        )

                        // + MOSS
                        IMGUI_ADD_HEADERS("Moss", "Normal", "Star", "Color")
                        // Dragon's Beard Peat
                        IMGUI_ADD_ROW("Dragon's Beard Peat",
                            IMGUI_CHECKBOX("##DragonsBeard", &HUDDetours::Forageables[static_cast<int>(EForageableType::DragonsBeard)][0], true)
                            IMGUI_CHECKBOX("##DragonsBeardC", &HUDDetours::Forageables[static_cast<int>(EForageableType::DragonsBeard)][1]),
                            IMGUI_COLORPICKER("##DragonsBeard", &Configuration::ForageableColors.find(EForageableType::DragonsBeard)->second)
                        )
                        // Emerald Carpet Moss
                        IMGUI_ADD_ROW("Emerald Carpet Moss",
                            IMGUI_CHECKBOX("##EmeraldCarpet", &HUDDetours::Forageables[static_cast<int>(EForageableType::EmeraldCarpet)][0], true)
                            IMGUI_CHECKBOX("##EmeraldCarpetC", &HUDDetours::Forageables[static_cast<int>(EForageableType::EmeraldCarpet)][1]),
                            IMGUI_COLORPICKER("##EmeraldCarpet", &Configuration::ForageableColors.find(EForageableType::EmeraldCarpet)->second)
                        )

                        // + MUSHROOM
                        IMGUI_ADD_HEADERS("Mushroom", "Normal", "Star", "Color")
                        // Brightshroom
                        IMGUI_ADD_ROW("Brightshroom",
                            IMGUI_CHECKBOX("##MushroomBlue", &HUDDetours::Forageables[static_cast<int>(EForageableType::MushroomBlue)][0], true)
                            IMGUI_CHECKBOX("##MushroomBlueC", &HUDDetours::Forageables[static_cast<int>(EForageableType::MushroomBlue)][1]),
                            IMGUI_COLORPICKER("##MushroomBlue", &Configuration::ForageableColors.find(EForageableType::MushroomBlue)->second)
                        )
                        // Mountain Morel
                        IMGUI_ADD_ROW("Mountain Morel",
                            IMGUI_CHECKBOX("##MushroomRed", &HUDDetours::Forageables[static_cast<int>(EForageableType::MushroomRed)][0], true)
                            IMGUI_CHECKBOX("##MushroomRedC", &HUDDetours::Forageables[static_cast<int>(EForageableType::MushroomRed)][1]),
                            IMGUI_COLORPICKER("##MushroomRed", &Configuration::ForageableColors.find(EForageableType::MushroomRed)->second)
                        )

                        // + SPICE
                        IMGUI_ADD_HEADERS("Spice", "Normal", "Star", "Color")
                        // Dari Cloves
                        IMGUI_ADD_ROW("Dari Cloves",
                            IMGUI_CHECKBOX("##DariCloves", &HUDDetours::Forageables[static_cast<int>(EForageableType::DariCloves)][0], true)
                            IMGUI_CHECKBOX("##DariClovesC", &HUDDetours::Forageables[static_cast<int>(EForageableType::DariCloves)][1]),
                            IMGUI_COLORPICKER("##DariCloves", &Configuration::ForageableColors.find(EForageableType::DariCloves)->second)
                        )
                        // Heat Root
                        IMGUI_ADD_ROW("Heat Root",
                            IMGUI_CHECKBOX("##HeatRoot", &HUDDetours::Forageables[static_cast<int>(EForageableType::HeatRoot)][0], true)
                            IMGUI_CHECKBOX("##HeatRootC", &HUDDetours::Forageables[static_cast<int>(EForageableType::HeatRoot)][1]),
                            IMGUI_COLORPICKER("##HeatRoot", &Configuration::ForageableColors.find(EForageableType::HeatRoot)->second)
                        )
                        // Spice Sprouts
                        IMGUI_ADD_ROW("Spice Sprouts",
                            IMGUI_CHECKBOX("##SpicedSprouts", &HUDDetours::Forageables[static_cast<int>(EForageableType::SpicedSprouts)][0], true)
                            IMGUI_CHECKBOX("##SpicedSproutsC", &HUDDetours::Forageables[static_cast<int>(EForageableType::SpicedSprouts)][1]),
                            IMGUI_COLORPICKER("##SpicedSprouts", &Configuration::ForageableColors.find(EForageableType::SpicedSprouts)->second)
                        )
                        // Sweet Leaf
                        IMGUI_ADD_ROW("Sweet Leaf",
                            IMGUI_CHECKBOX("##SweetLeaves", &HUDDetours::Forageables[static_cast<int>(EForageableType::SweetLeaves)][0], true)
                            IMGUI_CHECKBOX("##SweetLeavesC", &HUDDetours::Forageables[static_cast<int>(EForageableType::SweetLeaves)][1]),
                            IMGUI_COLORPICKER("##SweetLeaves", &Configuration::ForageableColors.find(EForageableType::SweetLeaves)->second)
                        )

                        // + VEGETABLE
                        IMGUI_ADD_HEADERS("Vegetable", "Normal", "Star", "Color")
                        // Wild Garlic
                        IMGUI_ADD_ROW("Wild Garlic",
                            IMGUI_CHECKBOX("##Garlic", &HUDDetours::Forageables[static_cast<int>(EForageableType::Garlic)][0], true)
                            IMGUI_CHECKBOX("##GarlicC", &HUDDetours::Forageables[static_cast<int>(EForageableType::Garlic)][1]),
                            IMGUI_COLORPICKER("##Garlic", &Configuration::ForageableColors.find(EForageableType::Garlic)->second)
                        )
                        // Wild Ginger
                        IMGUI_ADD_ROW("Wild Ginger",
                            IMGUI_CHECKBOX("##Ginger", &HUDDetours::Forageables[static_cast<int>(EForageableType::Ginger)][0], true)
                            IMGUI_CHECKBOX("##GingerC", &HUDDetours::Forageables[static_cast<int>(EForageableType::Ginger)][1]),
                            IMGUI_COLORPICKER("##Ginger", &Configuration::ForageableColors.find(EForageableType::Ginger)->second)
                        )
                        // Wild Green Onion
                        IMGUI_ADD_ROW("Wild Green Onion",
                            IMGUI_CHECKBOX("##GreenOnion", &HUDDetours::Forageables[static_cast<int>(EForageableType::GreenOnion)][0], true)
                            IMGUI_CHECKBOX("##GreenOnionC", &HUDDetours::Forageables[static_cast<int>(EForageableType::GreenOnion)][1]),
                            IMGUI_COLORPICKER("##GreenOnion", &Configuration::ForageableColors.find(EForageableType::GreenOnion)->second)
                        )
                }
                IMGUI_END_TABLE()
            }

            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Bugs##BugsSettingsHeader")) {

                if (ImGui::Button("Common##Bugs")) {
                    for (auto& Bug : HUDDetours::Bugs) {
                        Bug[static_cast<int>(EBugQuality::Common)][1] = Bug[static_cast<int>(EBugQuality::Common)][0] = !Bug[static_cast<int>(EBugQuality::Common)][0];
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Uncommon##Bugs")) {
                    for (auto& Bug : HUDDetours::Bugs) {
                        Bug[static_cast<int>(EBugQuality::Uncommon)][1] = Bug[static_cast<int>(EBugQuality::Uncommon)][0] = !Bug[static_cast<int>(EBugQuality::Uncommon)][0];
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Rare##Bugs")) {
                    for (auto& Bug : HUDDetours::Bugs) {
                        Bug[static_cast<int>(EBugQuality::Rare)][1] = Bug[static_cast<int>(EBugQuality::Rare)][0] = !Bug[static_cast<int>(EBugQuality::Rare)][0];
                        Bug[static_cast<int>(EBugQuality::Rare2)][1] = Bug[static_cast<int>(EBugQuality::Rare2)][0] = !Bug[static_cast<int>(EBugQuality::Rare2)][0];
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Epic##Bugs")) {
                    for (auto& Bug : HUDDetours::Bugs) {
                        Bug[static_cast<int>(EBugQuality::Epic)][1] = Bug[static_cast<int>(EBugQuality::Epic)][0] = !Bug[static_cast<int>(EBugQuality::Epic)][0];
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Star##Bugs")) {
                    for (auto& Bug : HUDDetours::Bugs) {
                        for (auto& j : Bug) {
                            j[1] = !j[1];
                        }
                    }
                    Configuration::Save();
                }

                IMGUI_SETUP_TABLE("Bugs", 4)
                {
                    IMGUI_TABLE_SETUP_COLUMN("Name", ImGuiTableColumnFlags_None, 0)
                        IMGUI_TABLE_SETUP_COLUMN("Normal", ImGuiTableColumnFlags_WidthFixed, 40)
                        IMGUI_TABLE_SETUP_COLUMN("Star", ImGuiTableColumnFlags_WidthFixed, 40)
                        IMGUI_TABLE_SETUP_COLUMN("Color", ImGuiTableColumnFlags_WidthFixed, 40)

                        // + BEE
                        IMGUI_ADD_HEADERS("Bee", "Normal", "Star", "Color")
                        // Bahari Bee
                        IMGUI_ADD_ROW("Bahari Bee",
                            IMGUI_CHECKBOX("##BahariBee", &HUDDetours::Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                            IMGUI_CHECKBOX("##BahariBeeC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)][1]),
                            IMGUI_COLORPICKER("##BahariBee", &Configuration::BugColors.find(FBugType(EBugKind::Bee, EBugQuality::Uncommon))->second)
                        )
                        // Golden Glory Bee
                        IMGUI_ADD_ROW("Golden Glory Bee",
                            IMGUI_CHECKBOX("##GoldenGloryBee", &HUDDetours::Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)][0], true)
                            IMGUI_CHECKBOX("##GoldenGloryBeeC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)][1]),
                            IMGUI_COLORPICKER("##GoldenGloryBee", &Configuration::BugColors.find(FBugType(EBugKind::Bee, EBugQuality::Rare))->second)
                        )

                        // + BEETLE
                        IMGUI_ADD_HEADERS("Beetle", "Normal", "Star", "Color")
                        // Spotted Stink Bug
                        IMGUI_ADD_ROW("Spotted Stink Bug",
                            IMGUI_CHECKBOX("##SpottedStinkBug", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Common)][0], true)
                            IMGUI_CHECKBOX("##SpottedStinkBugC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Common)][1]),
                            IMGUI_COLORPICKER("##SpottedStinkBug", &Configuration::BugColors.find(FBugType(EBugKind::Beetle, EBugQuality::Common))->second)
                        )
                        // Proudhorned Stag Beetle
                        IMGUI_ADD_ROW("Proudhorned Stag Beetle",
                            IMGUI_CHECKBOX("##ProudhornedStag", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                            IMGUI_CHECKBOX("##ProudhornedStagC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Uncommon)][1]),
                            IMGUI_COLORPICKER("##ProudhornedStag", &Configuration::BugColors.find(FBugType(EBugKind::Beetle, EBugQuality::Uncommon))->second)
                        )
                        // Raspberry Beetle
                        IMGUI_ADD_ROW("Raspberry Beetle",
                            IMGUI_CHECKBOX("##Raspberry", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Rare)][0], true)
                            IMGUI_CHECKBOX("##RaspberryC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Rare)][1]),
                            IMGUI_COLORPICKER("##Raspberry", &Configuration::BugColors.find(FBugType(EBugKind::Beetle, EBugQuality::Rare))->second)
                        )
                        // Ancient Amber Beetle
                        IMGUI_ADD_ROW("Ancient Amber Beetle",
                            IMGUI_CHECKBOX("##AncientAmber", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Epic)][0], true)
                            IMGUI_CHECKBOX("##AncientAmberC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Epic)][1]),
                            IMGUI_COLORPICKER("##AncientAmber", &Configuration::BugColors.find(FBugType(EBugKind::Beetle, EBugQuality::Epic))->second)
                        )

                        // + BUTTERFLY
                        IMGUI_ADD_HEADERS("Butterfly", "Normal", "Star", "Color")
                        // Common Blue Butterfly
                        IMGUI_ADD_ROW("Common Blue Butterfly",
                            IMGUI_CHECKBOX("##CommonBlue", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Common)][0], true)
                            IMGUI_CHECKBOX("##CommonBlueC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Common)][1]),
                            IMGUI_COLORPICKER("##CommonBlue", &Configuration::BugColors.find(FBugType(EBugKind::Butterfly, EBugQuality::Common))->second)
                        )
                        // Duskwing Butterfly
                        IMGUI_ADD_ROW("Duskwing Butterfly",
                            IMGUI_CHECKBOX("##Duskwing", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                            IMGUI_CHECKBOX("##DuskwingC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Uncommon)][1]),
                            IMGUI_COLORPICKER("##Duskwing", &Configuration::BugColors.find(FBugType(EBugKind::Butterfly, EBugQuality::Uncommon))->second)
                        )
                        // Brighteye Butterfly
                        IMGUI_ADD_ROW("Brighteye Butterfly",
                            IMGUI_CHECKBOX("##Brighteye", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Rare)][0], true)
                            IMGUI_CHECKBOX("##BrighteyeC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Rare)][1]),
                            IMGUI_COLORPICKER("##Brighteye", &Configuration::BugColors.find(FBugType(EBugKind::Butterfly, EBugQuality::Rare))->second)
                        )
                        // Rainbow-Tipped Butterfly
                        IMGUI_ADD_ROW("Rainbow-Tipped Butterfly",
                            IMGUI_CHECKBOX("##Rainbow", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Epic)][0], true)
                            IMGUI_CHECKBOX("##RainbowC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Epic)][1]),
                            IMGUI_COLORPICKER("##Rainbow", &Configuration::BugColors.find(FBugType(EBugKind::Butterfly, EBugQuality::Epic))->second)
                        )

                        // + CICADA
                        IMGUI_ADD_HEADERS("Cicada", "Normal", "Star", "Color")
                        // Common Bark Cicada
                        IMGUI_ADD_ROW("Common Bark Cicada",
                            IMGUI_CHECKBOX("##CommonBark", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Common)][0], true)
                            IMGUI_CHECKBOX("##CommonBarkC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Common)][1]),
                            IMGUI_COLORPICKER("##CommonBark", &Configuration::BugColors.find(FBugType(EBugKind::Cicada, EBugQuality::Common))->second)
                        )
                        // Cerulean Cicada
                        IMGUI_ADD_ROW("Cerulean Cicada",
                            IMGUI_CHECKBOX("##Cerulean", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                            IMGUI_CHECKBOX("##CeruleanC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Uncommon)][1]),
                            IMGUI_COLORPICKER("##Cerulean", &Configuration::BugColors.find(FBugType(EBugKind::Cicada, EBugQuality::Uncommon))->second)
                        )
                        // Spitfire Cicada
                        IMGUI_ADD_ROW("Spitfire Cicada",
                            IMGUI_CHECKBOX("##Spitfire", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Rare)][0], true)
                            IMGUI_CHECKBOX("##SpitfireC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Rare)][1]),
                            IMGUI_COLORPICKER("##Spitfire", &Configuration::BugColors.find(FBugType(EBugKind::Cicada, EBugQuality::Rare))->second)
                        )

                        // + CRAB
                        IMGUI_ADD_HEADERS("Crab", "Normal", "Star", "Color")
                        // Bahari Crab
                        IMGUI_ADD_ROW("Bahari Crab",
                            IMGUI_CHECKBOX("##BahariCrab", &HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)][0], true)
                            IMGUI_CHECKBOX("##BahariCrabC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)][1]),
                            IMGUI_COLORPICKER("##BahariCrab", &Configuration::BugColors.find(FBugType(EBugKind::Crab, EBugQuality::Common))->second)
                        )
                        // Spineshell Crab
                        IMGUI_ADD_ROW("Spineshell Crab",
                            IMGUI_CHECKBOX("##SpineshellCrab", &HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                            IMGUI_CHECKBOX("##SpineshellCrabC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)][1]),
                            IMGUI_COLORPICKER("##SpineshellCrab", &Configuration::BugColors.find(FBugType(EBugKind::Crab, EBugQuality::Uncommon))->second)
                        )
                        // Vampire Crab
                        IMGUI_ADD_ROW("Vampire Crab",
                            IMGUI_CHECKBOX("##VampireCrab", &HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)][0], true)
                            IMGUI_CHECKBOX("##VampireCrabC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)][1]),
                            IMGUI_COLORPICKER("##VampireCrab", &Configuration::BugColors.find(FBugType(EBugKind::Crab, EBugQuality::Rare))->second)
                        )

                        // + CRICKET
                        IMGUI_ADD_HEADERS("Cricket", "Normal", "Star", "Color")
                        // Common Field Cricket
                        IMGUI_ADD_ROW("Common Field Cricket",
                            IMGUI_CHECKBOX("##CommonField", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Common)][0], true)
                            IMGUI_CHECKBOX("##CommonFieldC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Common)][1]),
                            IMGUI_COLORPICKER("##CommonField", &Configuration::BugColors.find(FBugType(EBugKind::Cricket, EBugQuality::Common))->second)
                        )
                        // Garden Leafhopper
                        IMGUI_ADD_ROW("Garden Leafhopper",
                            IMGUI_CHECKBOX("##Leafhopper", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                            IMGUI_CHECKBOX("##LeafhopperC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Uncommon)][1]),
                            IMGUI_COLORPICKER("##Leafhopper", &Configuration::BugColors.find(FBugType(EBugKind::Cricket, EBugQuality::Uncommon))->second)
                        )
                        // Azure Stonehopper
                        IMGUI_ADD_ROW("Azure Stonehopper",
                            IMGUI_CHECKBOX("##Stonehopper", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Rare)][0], true)
                            IMGUI_CHECKBOX("##StonehopperC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Rare)][1]),
                            IMGUI_COLORPICKER("##Stonehopper", &Configuration::BugColors.find(FBugType(EBugKind::Cricket, EBugQuality::Rare))->second)
                        )

                        // + DRAGONFLY
                        IMGUI_ADD_HEADERS("Dragonfly", "Normal", "Star", "Color")
                        // Brushtail Dragonfly
                        IMGUI_ADD_ROW("Brushtail Dragonfly",
                            IMGUI_CHECKBOX("##Brushtail", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Common)][0], true)
                            IMGUI_CHECKBOX("##BrushtailC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Common)][1]),
                            IMGUI_COLORPICKER("##Brushtail", &Configuration::BugColors.find(FBugType(EBugKind::Dragonfly, EBugQuality::Common))->second)
                        )
                        // Inky Dragonfly
                        IMGUI_ADD_ROW("Inky Dragonfly",
                            IMGUI_CHECKBOX("##Inky", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                            IMGUI_CHECKBOX("##InkyC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Uncommon)][1]),
                            IMGUI_COLORPICKER("##Inky", &Configuration::BugColors.find(FBugType(EBugKind::Dragonfly, EBugQuality::Uncommon))->second)
                        )
                        // Firebreathing Dragonfly
                        IMGUI_ADD_ROW("Firebreathing Dragonfly",
                            IMGUI_CHECKBOX("##Firebreathing", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Rare)][0], true)
                            IMGUI_CHECKBOX("##FirebreathingC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Rare)][1]),
                            IMGUI_COLORPICKER("##Firebreathing", &Configuration::BugColors.find(FBugType(EBugKind::Dragonfly, EBugQuality::Rare))->second)
                        )
                        // Jewelwing Dragonfly
                        IMGUI_ADD_ROW("Jewelwing Dragonfly",
                            IMGUI_CHECKBOX("##Jewelwing", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Epic)][0], true)
                            IMGUI_CHECKBOX("##JewelwingC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Epic)][1]),
                            IMGUI_COLORPICKER("##Jewelwing", &Configuration::BugColors.find(FBugType(EBugKind::Dragonfly, EBugQuality::Epic))->second)
                        )

                        // + GLOWBUG
                        IMGUI_ADD_HEADERS("Glowbug", "Normal", "Star", "Color")
                        // Paper Lantern Bug
                        IMGUI_ADD_ROW("Paper Lantern Bug",
                            IMGUI_CHECKBOX("##PaperLantern", &HUDDetours::Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Common)][0], true)
                            IMGUI_CHECKBOX("##PaperLanternC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Common)][1]),
                            IMGUI_COLORPICKER("##PaperLantern", &Configuration::BugColors.find(FBugType(EBugKind::Glowbug, EBugQuality::Common))->second)
                        )
                        // Bahari Glowbug
                        IMGUI_ADD_ROW("Bahari Glowbug",
                            IMGUI_CHECKBOX("##BahariGlowbug", &HUDDetours::Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                            IMGUI_CHECKBOX("##BahariGlowbugC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Uncommon)][1]),
                            IMGUI_COLORPICKER("##BahariGlowbug", &Configuration::BugColors.find(FBugType(EBugKind::Glowbug, EBugQuality::Uncommon))->second)
                        )

                        // + LADYBUG
                        IMGUI_ADD_HEADERS("Ladybug", "Normal", "Star", "Color")
                        // Garden Ladybug
                        IMGUI_ADD_ROW("Garden Ladybug",
                            IMGUI_CHECKBOX("##GardenLadybug", &HUDDetours::Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Common)][0], true)
                            IMGUI_CHECKBOX("##GardenLadybugC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Common)][1]),
                            IMGUI_COLORPICKER("##GardenLadybug", &Configuration::BugColors.find(FBugType(EBugKind::Ladybug, EBugQuality::Common))->second)
                        )
                        // Princess Ladybug 
                        IMGUI_ADD_ROW("Princess Ladybug",
                            IMGUI_CHECKBOX("##PrincessLadybug", &HUDDetours::Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                            IMGUI_CHECKBOX("##PrincessLadybugC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Uncommon)][1]),
                            IMGUI_COLORPICKER("##PrincessLadybug", &Configuration::BugColors.find(FBugType(EBugKind::Ladybug, EBugQuality::Uncommon))->second)
                        )

                        // + MANTIS
                        IMGUI_ADD_HEADERS("Mantis", "Normal", "Star", "Color")
                        // Garden Mantis U
                        IMGUI_ADD_ROW("Garden Mantis",
                            IMGUI_CHECKBOX("##GardenMantis", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                            IMGUI_CHECKBOX("##GardenMantisC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Uncommon)][1]),
                            IMGUI_COLORPICKER("##GardenMantis", &Configuration::BugColors.find(FBugType(EBugKind::Mantis, EBugQuality::Uncommon))->second)
                        )
                        // Spotted Mantis R
                        IMGUI_ADD_ROW("Spotted Mantis",
                            IMGUI_CHECKBOX("##SpottedMantis", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare)][0], true)
                            IMGUI_CHECKBOX("##SpottedMantisC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare)][1]),
                            IMGUI_COLORPICKER("##SpottedMantis", &Configuration::BugColors.find(FBugType(EBugKind::Mantis, EBugQuality::Rare))->second)
                        )
                        // Leafstalker Mantis R2
                        IMGUI_ADD_ROW("Leafstalker Mantis",
                            IMGUI_CHECKBOX("##LeafstalkerMantis", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare2)][0], true)
                            IMGUI_CHECKBOX("##LeafstalkerMantisC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare2)][1]),
                            IMGUI_COLORPICKER("##LeafstalkerMantis", &Configuration::BugColors.find(FBugType(EBugKind::Mantis, EBugQuality::Rare2))->second)
                        )
                        // Fairy Mantis E
                        IMGUI_ADD_ROW("Fairy Mantis",
                            IMGUI_CHECKBOX("##FairyMantis", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Epic)][0], true)
                            IMGUI_CHECKBOX("##FairyMantisC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Epic)][1]),
                            IMGUI_COLORPICKER("##FairyMantis", &Configuration::BugColors.find(FBugType(EBugKind::Mantis, EBugQuality::Epic))->second)
                        )

                        // + MOTH
                        IMGUI_ADD_HEADERS("Moth", "Normal", "Star", "Color")
                        // Kilima Night Moth
                        IMGUI_ADD_ROW("Kilima Night Moth",
                            IMGUI_CHECKBOX("##KilimaNightMoth", &HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)][0], true)
                            IMGUI_CHECKBOX("##KilimaNightMothC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)][1]),
                            IMGUI_COLORPICKER("##KilimaNightMoth", &Configuration::BugColors.find(FBugType(EBugKind::Moth, EBugQuality::Common))->second)
                        )
                        // Lunar Fairy Moth
                        IMGUI_ADD_ROW("Lunar Fairy Moth",
                            IMGUI_CHECKBOX("##LunarFairyMoth", &HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                            IMGUI_CHECKBOX("##LunarFairyMothC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)][1]),
                            IMGUI_COLORPICKER("##LunarFairyMoth", &Configuration::BugColors.find(FBugType(EBugKind::Moth, EBugQuality::Uncommon))->second)
                        )
                        // Gossamer Veil Moth
                        IMGUI_ADD_ROW("Gossamer Veil Moth",
                            IMGUI_CHECKBOX("##GossamerVeilMoth", &HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)][0], true)
                            IMGUI_CHECKBOX("##GossamerVeilMothC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)][1]),
                            IMGUI_COLORPICKER("##GossamerVeilMoth", &Configuration::BugColors.find(FBugType(EBugKind::Moth, EBugQuality::Rare))->second)
                        )

                        // + Pede
                        IMGUI_ADD_HEADERS("Pede", "Normal", "Star", "Color")
                        // Garden Millipede
                        IMGUI_ADD_ROW("Garden Millipede",
                            IMGUI_CHECKBOX("##GardenMillipede", &HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                            IMGUI_CHECKBOX("##GardenMillipedeC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)][1]),
                            IMGUI_COLORPICKER("##GardenMillipede", &Configuration::BugColors.find(FBugType(EBugKind::Pede, EBugQuality::Uncommon))->second)
                        )
                        // Hairy Millipede
                        IMGUI_ADD_ROW("Hairy Millipede",
                            IMGUI_CHECKBOX("##HairyMillipede", &HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)][0], true)
                            IMGUI_CHECKBOX("##HairyMillipedeC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)][1]),
                            IMGUI_COLORPICKER("##HairyMillipede", &Configuration::BugColors.find(FBugType(EBugKind::Pede, EBugQuality::Rare))->second)
                        )
                        // Scintillating Centipede
                        IMGUI_ADD_ROW("Scintillating Centipede",
                            IMGUI_CHECKBOX("##ScintillatingCentipede", &HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)][0], true)
                            IMGUI_CHECKBOX("##ScintillatingCentipedeC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)][1]),
                            IMGUI_COLORPICKER("##ScintillatingCentipede", &Configuration::BugColors.find(FBugType(EBugKind::Pede, EBugQuality::Rare2))->second)
                        )

                        // + SNAIL
                        IMGUI_ADD_HEADERS("Snail", "Normal", "Star", "Color")
                        // Garden Snail U
                        IMGUI_ADD_ROW("Garden Snail",
                            IMGUI_CHECKBOX("##GardenSnail", &HUDDetours::Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                            IMGUI_CHECKBOX("##GardenSnailC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)][1]),
                            IMGUI_COLORPICKER("##GardenSnail", &Configuration::BugColors.find(FBugType(EBugKind::Snail, EBugQuality::Uncommon))->second)
                        )
                        // Stripeshell Snail R
                        IMGUI_ADD_ROW("Stripeshell Snail",
                            IMGUI_CHECKBOX("##StripeshellSnail", &HUDDetours::Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)][0], true)
                            IMGUI_CHECKBOX("##StripeshellSnailC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)][1]),
                            IMGUI_COLORPICKER("##StripeshellSnail", &Configuration::BugColors.find(FBugType(EBugKind::Snail, EBugQuality::Rare))->second)
                        )
                }
                IMGUI_END_TABLE()
            }
            if (ImGui::CollapsingHeader("Trees##TreesSettingHeader")) {
                if (ImGui::Button("Bush##BushBtn")) {
                    bool newState = !HUDDetours::Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Bush)];
                    HUDDetours::Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Bush)] = newState;
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Sapwood##SapwoodBtn")) {
                    for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                        HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(size.first)] =
                            !HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(size.first)];
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Heartwood##HeartwoodBtn")) {
                    for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                        HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(size.first)] =
                            !HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(size.first)];
                    }
                    Configuration::Save();
                }
                ImGui::SameLine();
                if (ImGui::Button("Flow##FlowBtn")) {
                    for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                        HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(size.first)] =
                            !HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(size.first)];
                    }
                    Configuration::Save();
                }

                IMGUI_SETUP_TABLE("Trees", 5)
                {
                    IMGUI_TABLE_SETUP_COLUMN("Name", ImGuiTableColumnFlags_None, 0)
                        IMGUI_TABLE_SETUP_COLUMN("Sm", ImGuiTableColumnFlags_WidthFixed, 40)
                        IMGUI_TABLE_SETUP_COLUMN("Med", ImGuiTableColumnFlags_WidthFixed, 40)
                        IMGUI_TABLE_SETUP_COLUMN("Lg", ImGuiTableColumnFlags_WidthFixed, 40)
                        IMGUI_TABLE_SETUP_COLUMN("Color", ImGuiTableColumnFlags_WidthFixed, 40)

                        IMGUI_ADD_HEADERS("", "Sm", "Med", "Lg", "Color")

                        // Bush
                        IMGUI_ADD_ROW("Bush",
                            IMGUI_CHECKBOX("##BushSm", &HUDDetours::Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Bush)]),
                            IMGUI_TABLENEXTCOLUMN() // these are to separate the ColorPicker 2 more columns.
                            IMGUI_TABLENEXTCOLUMN()
                            IMGUI_COLORPICKER("##Bush", &Configuration::TreeColors.find(ETreeType::Bush)->second)
                        )
                        // Sapwood
                        IMGUI_ADD_ROW("Sapwood",
                            IMGUI_CHECKBOX("##SapwoodSm", &HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Small)], true)
                            IMGUI_CHECKBOX("##SapwoodMed", &HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Medium)], true)
                            IMGUI_CHECKBOX("##SapwoodLg", &HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Large)]),
                            IMGUI_COLORPICKER("##Sapwood", &Configuration::TreeColors.find(ETreeType::Sapwood)->second)
                        )
                        // Heartwood
                        IMGUI_ADD_ROW("Heartwood",
                            IMGUI_CHECKBOX("##HeartwoodSm", &HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Small)], true)
                            IMGUI_CHECKBOX("##HeartwoodMed", &HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Medium)], true)
                            IMGUI_CHECKBOX("##HeartwoodLg", &HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Large)]),
                            IMGUI_COLORPICKER("##Heartwood", &Configuration::TreeColors.find(ETreeType::Heartwood)->second)
                        )
                        // Flow-Infused
                        IMGUI_ADD_ROW("Flow-Infused",
                            IMGUI_CHECKBOX("##FlowSm", &HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Small)], true)
                            IMGUI_CHECKBOX("##FlowMed", &HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Medium)], true)
                            IMGUI_CHECKBOX("##FlowLg", &HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Large)]),
                            IMGUI_COLORPICKER("##Flow", &Configuration::TreeColors.find(ETreeType::Flow)->second)
                        )
                }
                IMGUI_END_TABLE()
            }
            if (ImGui::CollapsingHeader("Player & Entities##PlayerEntitiesSettingHeader")) {
                if (ImGui::Button("Toggle All##MiscBtn")) {
                    bool newState = !HUDDetours::Singles[static_cast<int>(EOneOffs::Player)];

                    HUDDetours::Singles[static_cast<int>(EOneOffs::Player)] = newState;
                    HUDDetours::Singles[static_cast<int>(EOneOffs::NPC)] = newState;
                    HUDDetours::Fish[static_cast<int>(EFishType::Hook)] = newState;
                    HUDDetours::Fish[static_cast<int>(EFishType::Node)] = newState;
                    HUDDetours::Singles[static_cast<int>(EOneOffs::Loot)] = newState;
                    HUDDetours::Singles[static_cast<int>(EOneOffs::Quest)] = newState;
                    HUDDetours::Singles[static_cast<int>(EOneOffs::RummagePiles)] = newState;
                    HUDDetours::Singles[static_cast<int>(EOneOffs::Treasure)] = newState;
                    HUDDetours::Singles[static_cast<int>(EOneOffs::Stables)] = newState;
                    HUDDetours::Singles[static_cast<int>(EOneOffs::TimedDrop)] = newState;
                    HUDDetours::Singles[static_cast<int>(EOneOffs::Others)] = newState;

                    Configuration::Save();
                }

                IMGUI_SETUP_TABLE("Odds", 3)
                {
                    IMGUI_TABLE_SETUP_COLUMN("Name", ImGuiTableColumnFlags_None, 0)
                        IMGUI_TABLE_SETUP_COLUMN("Show", ImGuiTableColumnFlags_WidthFixed, 40)
                        IMGUI_TABLE_SETUP_COLUMN("Color", ImGuiTableColumnFlags_WidthFixed, 40)
                        IMGUI_ADD_HEADERS("Show", "Color", "Players")

                        IMGUI_ADD_ROW("Players",
                            IMGUI_CHECKBOX("##Players", &HUDDetours::Singles[static_cast<int>(EOneOffs::Player)]),
                            IMGUI_COLORPICKER("##Players", &Configuration::SingleColors.find(EOneOffs::Player)->second)
                        )
                        IMGUI_ADD_ROW("NPCs",
                            IMGUI_CHECKBOX("##NPC", &HUDDetours::Singles[static_cast<int>(EOneOffs::NPC)]),
                            IMGUI_COLORPICKER("##NPC", &Configuration::SingleColors.find(EOneOffs::NPC)->second)
                        )
                        IMGUI_ADD_ROW("Fish",
                            IMGUI_CHECKBOX("##FishHook", &HUDDetours::Fish[static_cast<int>(EFishType::Hook)]),
                            IMGUI_COLORPICKER("##FishHook", &Configuration::FishColors.find(EFishType::Hook)->second)
                        )
                        IMGUI_ADD_ROW("Fish Pools",
                            IMGUI_CHECKBOX("##FishPool", &HUDDetours::Fish[static_cast<int>(EFishType::Node)]),
                            IMGUI_COLORPICKER("##FishPool", &Configuration::FishColors.find(EFishType::Node)->second)
                        )
                        IMGUI_ADD_ROW("Loot",
                            IMGUI_CHECKBOX("##Loot", &HUDDetours::Singles[static_cast<int>(EOneOffs::Loot)]),
                            IMGUI_COLORPICKER("##Loot", &Configuration::SingleColors.find(EOneOffs::Loot)->second)
                        )
                        IMGUI_ADD_ROW("Quests",
                            IMGUI_CHECKBOX("##Quest", &HUDDetours::Singles[static_cast<int>(EOneOffs::Quest)]),
                            IMGUI_COLORPICKER("##Quest", &Configuration::SingleColors.find(EOneOffs::Quest)->second)
                        )
                        IMGUI_ADD_ROW("Rummage Piles",
                            IMGUI_CHECKBOX("##RummagePiles", &HUDDetours::Singles[static_cast<int>(EOneOffs::RummagePiles)]),
                            IMGUI_COLORPICKER("##RummagePiles", &Configuration::SingleColors.find(EOneOffs::RummagePiles)->second)
                        )
                        IMGUI_ADD_ROW("Treaure",
                            IMGUI_CHECKBOX("##Treasure", &HUDDetours::Singles[static_cast<int>(EOneOffs::Treasure)]),
                            IMGUI_COLORPICKER("##Treasure", &Configuration::SingleColors.find(EOneOffs::Treasure)->second)
                        )
                        IMGUI_ADD_ROW("Stables",
                            IMGUI_CHECKBOX("##Stables", &HUDDetours::Singles[static_cast<int>(EOneOffs::Stables)]),
                            IMGUI_COLORPICKER("##Stables", &Configuration::SingleColors.find(EOneOffs::Stables)->second)
                        )
                        IMGUI_ADD_ROW("Timed Drops",
                            IMGUI_CHECKBOX("##TimedDrops", &HUDDetours::Singles[static_cast<int>(EOneOffs::TimedDrop)]),
                            IMGUI_COLORPICKER("##TimedDrops", &Configuration::SingleColors.find(EOneOffs::TimedDrop)->second)
                        )
                        IMGUI_ADD_ROW("Others",
                            IMGUI_CHECKBOX("##Others", &HUDDetours::Singles[static_cast<int>(EOneOffs::Others)]),
                            IMGUI_COLORPICKER("##Others", &Configuration::SingleColors.find(EOneOffs::Others)->second)
                        )
                }
                IMGUI_END_TABLE()
            }
        }
        // ==================================== 1 Aimbots & Fun TAB
        else if (OpenTab == 1) {
            ImGui::Columns(2, nullptr, false);

            // Aim Controls
            if (ImGui::CollapsingHeader("Aimbot Settings##AimbotSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    if (Configuration::bDrawFOVCircle) {
                        IMGUI_CHECKBOX("Enable Silent Aimbot", &Configuration::bEnableSilentAimbot, "Teleport Bug Bombs & Arrows To Your Target.")
                        IMGUI_CHECKBOX("Enable Legacy Aimbot", &Configuration::bEnableAimbot, "Typical Aimbot system for targets.")

                        if (Configuration::bEnableAimbot) {
                            ImGui::Text("Aim Smoothing:");
                            ImGui::SetNextItemWidth(200.0f);
                            IMGUI_SLIDER("Smoothing Factor", &MovementDetours::SmoothingFactor, 1.0f, 100.0f, "%1.0f");
                            ImGui::Text("Aim Offset Adjustment (Drag Point):");
                            constexpr auto canvas_size = ImVec2(200, 200); // Canvas size
                            static auto cursor_pos = ImVec2(0, 0); // Start at the center (0, 0 relative to center)
                            constexpr float scaling_factor = 0.5f; // Reduced scaling factor for finer control

                            ImU32 gridColor = IM_COL32(50, 45, 139, 255); // Grid lines color
                            ImU32 gridBackgroundColor = IM_COL32(26, 28, 33, 255); // Background color
                            ImU32 cursorColor = IM_COL32(69, 39, 160, 255); // Cursor color

                            if (ImGui::BeginChild("GridArea", ImVec2(200, 200), false, ImGuiWindowFlags_NoScrollbar)) {
                                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                                ImVec2 canvas_p0 = ImGui::GetCursorScreenPos(); // Top-left corner of the canvas
                                auto grid_center = ImVec2(canvas_p0.x + canvas_size.x * 0.5f, canvas_p0.y + canvas_size.y * 0.5f);

                                draw_list->AddRectFilled(canvas_p0, ImVec2(canvas_p0.x + canvas_size.x, canvas_p0.y + canvas_size.y), gridBackgroundColor);
                                draw_list->AddLine(ImVec2(grid_center.x, canvas_p0.y), ImVec2(grid_center.x, canvas_p0.y + canvas_size.y), gridColor);
                                draw_list->AddLine(ImVec2(canvas_p0.x, grid_center.y), ImVec2(canvas_p0.x + canvas_size.x, grid_center.y), gridColor);

                                ImGui::SetCursorScreenPos(ImVec2(grid_center.x + cursor_pos.x - 5, grid_center.y + cursor_pos.y - 5));
                                ImGui::InvisibleButton("cursor", ImVec2(10, 10));
                                if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
                                    cursor_pos.x = ImClamp(cursor_pos.x - ImGui::GetIO().MouseDelta.x * scaling_factor, -canvas_size.x * 0.5f, canvas_size.x * 0.5f);
                                    cursor_pos.y = ImClamp(cursor_pos.y - ImGui::GetIO().MouseDelta.y * scaling_factor, -canvas_size.y * 0.5f, canvas_size.y * 0.5f);
                                }

                                draw_list->AddCircleFilled(ImVec2(grid_center.x + cursor_pos.x, grid_center.y + cursor_pos.y), 5, cursorColor, 12);

                                // Sliders for fine-tuned control
                                ImGui::SetCursorPosY(canvas_p0.y + canvas_size.y + 5);
                                IMGUI_SLIDER("Horizontal & Vertical", reinterpret_cast<float*>(&cursor_pos), -canvas_size.x * 0.5f, canvas_size.x * 0.5f, "H: %.1f, V: %.1f");
                            }
                            ImGui::EndChild();

                            // Convert cursor_pos to AimOffset affecting Pitch and Yaw
                            AimDetours::AimOffset = { cursor_pos.x * scaling_factor, cursor_pos.y * scaling_factor, 0.0f };
                            ImGui::Text("Current Offset: Pitch: %.2f, Yaw: %.2f", AimDetours::AimOffset.X, AimDetours::AimOffset.Y);
                        }
                    }
                    else {
                        ImGui::Spacing();
                        ImGui::Text("[ ENABLE INTELIAIM TO VIEW AIMBOT OPTIONS ]");
                        ImGui::Spacing();

                        // This loops while intelli-aim is disabled - Major lag issue.
                        Configuration::bEnableSilentAimbot = false;
                        Configuration::bEnableAimbot = false;
                        //Configuration::Save();
                    }
                }
                else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            ImGui::NextColumn();
            
            // InteliTarget Controls
            if (ImGui::CollapsingHeader("InteliTarget Settings##InteliTargetSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    ImGui::Text("General Settings");
                    ImGui::Spacing();
                    IMGUI_CHECKBOX("Teleport to Targeted", &Configuration::bTeleportToTargeted, "Teleport directly to your targeted entity.")
                    IMGUI_CHECKBOX("Avoid Teleporting To Targeted Players", &Configuration::bAvoidTeleportingToPlayers, "Don't teleport to targeted players.")
                    IMGUI_CHECKBOX("Avoid Teleporting To Targeted When Players Are Near", &Configuration::bDoRadiusPlayersAvoidance, "Don't teleport if a player is detected near your destination.")

                    if (Configuration::bDoRadiusPlayersAvoidance) {
                        ImGui::SetNextItemWidth(200.0f);
                        if (ImGui::InputInt("Avoidance Radius (meters)", &Configuration::AvoidanceRadius)) { // Wrap in if-statement to avoid config::save spam.
                            Configuration::AvoidanceRadius = std::clamp(Configuration::AvoidanceRadius, 1, 100);
                            Configuration::Save();
                        }
                    }
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Distance to avoid players when target teleporting");

                    ImGui::Spacing();
                    ImGui::Text("FOV Circle Settings");
                    ImGui::Spacing();

                    IMGUI_CHECKBOX("Enable InteliAim Circle", &Configuration::bDrawFOVCircle, "Enable the smart FOV targeting system.Teleport to actors, enable aimbots, and more.");

                    if (Configuration::bDrawFOVCircle) {
                        IMGUI_CHECKBOX("Hide Circle", &Configuration::bHideFOVCircle, "Use the InteliTarget system without displaying the InteliAim Circle.");
                            IMGUI_CHECKBOX("Enable Crosshair", &Configuration::bDrawCrosshair, "Display a small purple crosshair in the middle of your screen. Nicely paired with Hide Circle.");

                        ImGui::SetNextItemWidth(200.0f);
                        IMGUI_SLIDER("InteliAim Radius", &Configuration::FOVRadius, 10.0f, 600.0f, "%1.0f", "Make your targeting area bigger or smaller. Set this before Hiding Circle.");

                        ImGui::SetNextItemWidth(200.0f);
                        IMGUI_SLIDER("Circle Transparency", &Configuration::CircleAlpha, 0.25f, 1.0f, "%.2f", "Adjust the transparency of the InteliAim Circle.");
                    }
                }
                else {
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
                if (MovementComponent) {
                    ImGui::Text("Map: %s", DetourManager::CurrentMap.c_str());
                    ImGui::Spacing();
                    
                    IMGUI_CHECKBOX("Enable Noclip", &MovementDetours::bEnableNoclip, "Allows you to fly and phase through anything.")

                    // Create a combo box for selecting the movement mode
                    static const char* movementModes[] = { "Walking", "Flying", "Fly No Collision" };
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
                        Configuration::Save();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##WalkSpeed")) {
                        Configuration::CustomWalkSpeed = MovementDetours::WalkSpeed;
                        Configuration::Save();
                    }

                    // Sprint Speed
                    ImGui::Text("Sprint Speed: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##SprintSpeedMultiplier", ImGuiDataType_Float, &Configuration::CustomSprintSpeedMultiplier, &f5)) {
                        Configuration::Save();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##SprintSpeedMultiplier")) {
                        Configuration::CustomSprintSpeedMultiplier = MovementDetours::SprintSpeedMultiplier;
                        Configuration::Save();
                    }

                    // Climbing Speed
                    ImGui::Text("Climbing Speed: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##ClimbingSpeed", ImGuiDataType_Float, &Configuration::CustomClimbingSpeed, &f5)) {
                        Configuration::Save();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##ClimbingSpeed")) {
                        Configuration::CustomClimbingSpeed = MovementDetours::ClimbingSpeed;
                        Configuration::Save();
                    }

                    // Gliding Speed
                    ImGui::Text("Gliding Speed: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##GlidingSpeed", ImGuiDataType_Float, &Configuration::CustomGlidingSpeed, &f5)) {
                        Configuration::Save();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##GlidingSpeed")) {
                        Configuration::CustomGlidingSpeed = MovementDetours::GlidingSpeed;
                        Configuration::Save();
                    }

                    // Gliding Fall Speed
                    ImGui::Text("Gliding Fall Speed: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##GlidingFallSpeed", ImGuiDataType_Float, &Configuration::CustomGlidingFallSpeed, &f5)) {
                        Configuration::Save();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##GlidingFallSpeed")) {
                        Configuration::CustomGlidingFallSpeed = MovementDetours::GlidingFallSpeed;
                        Configuration::Save();
                    }

                    // Jump Velocity
                    ImGui::Text("Jump Velocity: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##JumpVelocity", ImGuiDataType_Float, &Configuration::CustomJumpVelocity, &f5)) {
                        Configuration::Save();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##JumpVelocity")) {
                        Configuration::CustomJumpVelocity = MovementDetours::JumpVelocity;
                        Configuration::Save();
                    }

                    // Step Height
                    ImGui::Text("Step Height: ");
                    ImGui::SetNextItemWidth(325.0f);
                    if (ImGui::InputScalar("##MaxStepHeight", ImGuiDataType_Float, &Configuration::CustomMaxStepHeight, &f5)) {
                        Configuration::Save();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##MaxStepHeight")) {
                        Configuration::CustomMaxStepHeight = MovementDetours::MaxStepHeight;
                        Configuration::Save();
                    }
                }
                else {
                    if (!ValeriaCharacter) {
                        ImGui::Text("Waiting for character initialization...");
                    }
                    else {
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
                            if (DetourManager::CurrentMap == MapName || MapName == "UserDefined") {
                                if (ImGui::Selectable(Name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                                    if (ImGui::IsMouseDoubleClicked(0)) {
                                        if (Type == ELocation::Global_Home) {
                                            ValeriaCharacter->GetTeleportComponent()->RpcServerTeleport_Home();
                                        }
                                        else {
                                             TeleportDetours::TeleportPlayer(Location);
                                        }
                                    }
                                }
                            }
                        }
                        ImGui::EndListBox();
                    }
                    else {
                        ImGui::Text("Not enough space to display the list.");
                    }

                    auto [PlayerX, PlayerY, PlayerZ] = ValeriaCharacter->K2_GetActorLocation();
                    auto PlayerYaw = ValeriaCharacter->K2_GetActorRotation().Yaw;
                    ImGui::Text("Current Coords: %.3f, %.3f, %.3f, %.3f", PlayerX, PlayerY, PlayerZ, PlayerYaw);
                    ImGui::Spacing();

                    // Set the width for the labels and inputs
                    constexpr float labelWidth = 50.0f;
                    constexpr float inputWidth = 200.0f;

                    // 
                    static FVector TeleportLocation;
                    static FRotator TeleportRotate;

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

                    // Yaw
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("YAW: ");
                    ImGui::SameLine(labelWidth);
                    ImGui::SetNextItemWidth(inputWidth);
                    ImGui::InputScalar("##TeleportRotateYaw", ImGuiDataType_Double, &TeleportRotate.Yaw, &d1);

                    ImGui::Spacing();

                    if (ImGui::Button("Get Current Coordinates")) {
                        TeleportLocation = ValeriaCharacter->K2_GetActorLocation();
                        TeleportRotate = ValeriaCharacter->K2_GetActorRotation();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Teleport To Coordinates")) {
                        TeleportDetours::TeleportPlayer(TeleportLocation);
                    }
                    // Travel
                    ImGui::Text("Travel List");
                    ImGui::Text("Double-click a location listing to travel");
                    if (ImGui::BeginListBox("##TravelList", ImVec2(-1, 150))) {
                        for (auto& [Name, TeleportActor] : ActorHandler::GetTeleporters()) {
                            if (ImGui::Selectable(Name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                                if (ImGui::IsMouseDoubleClicked(0)) {
                                    auto Teleporter = static_cast<AVAL_TeleporterBase*>(TeleportActor);
                                    Teleporter->Client_ExecuteTeleport(ValeriaCharacter);
                                }
                            }
                        }
                        ImGui::EndListBox();
                    }
                    else {
                        ImGui::Text("Not enough space to display the list.");
                    }
                }
                else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }
        }
        // ==================================== 3 Selling & Player TAB
        else if (OpenTab == 3) {
            ImGui::Columns(2, nullptr, false);

            if (ImGui::CollapsingHeader("Selling Settings##SellingSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    UVillagerStoreComponent* StoreComponent = ValeriaCharacter->StoreComponent;

                    ImGui::Text("Quickly Sell Items - Bag 1");
                    ImGui::Spacing();
                    ImGui::Text("Select the bag, slot, and quantity to sell.");
                    ImGui::Spacing();
                    static int selectedSlot = 0;
                    static int selectedQuantity = 1;
                    static const char* quantities[] = { "1", "10", "50", "999", "Custom" };
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
                        FBagSlotLocation bag = {};
                        bag.BagIndex = 0;
                        bag.SlotIndex = selectedSlot;

                        const int quantityToSell = selectedQuantity < 4
                            ? atoi(quantities[selectedQuantity])
                            : atoi(customQuantity);

                        gDetourManager.Func_SellItem(bag, quantityToSell, StoreComponent, ValeriaCharacter, &cannotSell);
                    }

                    if (ImGui::BeginPopup("##cannotSell")) {
                        ImGui::Text("Cannot sell");
                        if (ImGui::Button("Close")) {
                            cannotSell = false;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }

                    if (cannotSell) {
                        ImGui::OpenPopup("##cannotSell");
                    }
                }
                else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            if (ImGui::CollapsingHeader("Villager Guild Stores##GuildStoresHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {

                    ImGuiStyle style = ImGui::GetStyle();
                    float width = ImGui::CalcItemWidth();
                    float spacing = style.ItemInnerSpacing.x;
                    float buttonSize = ImGui::GetFrameHeight();

                    static int selectedStore = 0;
                    static const char* stores[] = {
                        "Fishing Guild Store" /*ID = 2*/,
                        "Bug Catching Guild Store" /*ID = 3*/,
                        "Foraging Guild Store" /*ID = 6*/,
                        "Hunting Guild Store" /*ID = 7*/,
                        "Cooking Guild Store" /*ID = 10*/,
                        "Furniture Guild Store" /*ID = 12*/,
                        "Mining Guild Store" /*ID = 8806*/,
                        "Gardening Guild Store" /*ID = 19928*/
                    };

                    ImGui::Text("Open Remote Stores");

                    ImGui::PushItemWidth(width - spacing * 2.0f - buttonSize * 2.0f); //ImGui::PushItemWidth(CalcTextSize(stores[2])).x + style.FramePadding.x * 2.0f);
                    ImGui::SetNextItemWidth(250.0f);
                    if (ImGui::BeginCombo("##VillagerStores", stores[selectedStore])) {
                        for (int i = 0; i < IM_ARRAYSIZE(stores); i++) {
                            const bool isSelected = (selectedStore == i);
                            if (ImGui::Selectable(stores[i], isSelected)) {
                                selectedStore = i;
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::PopItemWidth();

                    if (WidgetDetours::GameplayUIManager && WidgetDetours::GameplayUIManager->IsValidLowLevel()) {
                        ImGui::SameLine();
                        if (ImGui::Button("Open Guild Store##OpenGuildStoreBtn")) {
                            switch (selectedStore) {
                            case 0:
                                gDetourManager.Func_DoOpenGuildStore(ValeriaCharacter, 2 /*Einar's Fishing Guild store*/);
                                show = false;
                                break;
                            case 1:
                                gDetourManager.Func_DoOpenGuildStore(ValeriaCharacter, 3 /*Auni's Bug Guild store*/);
                                show = false;
                                break;
                            case 2:
                                gDetourManager.Func_DoOpenGuildStore(ValeriaCharacter, 6 /*Ashura's Foraging Guild store*/);
                                show = false;
                                break;
                            case 3:
                                gDetourManager.Func_DoOpenGuildStore(ValeriaCharacter, 7 /*Hassian's Hunting Guild store*/);
                                show = false;
                                break;
                            case 4:
                                gDetourManager.Func_DoOpenGuildStore(ValeriaCharacter, 10 /*Reth's Cooking Guild store*/);
                                show = false;
                                break;
                            case 5:
                                gDetourManager.Func_DoOpenGuildStore(ValeriaCharacter, 12 /*Tish's Furniture Guild store*/);
                                show = false;
                                break;
                            case 6:
                                gDetourManager.Func_DoOpenGuildStore(ValeriaCharacter, 8806 /*Hodari's Mining Guild store*/);
                                show = false;
                                break;
                            case 7:
                                gDetourManager.Func_DoOpenGuildStore(ValeriaCharacter, 19928 /*Badruu's Gardening Guild store*/);
                                show = false;
                                break;
                            default:
                                ImGui::Text("Couldn't retrieve store (Index: %i)", selectedStore);
                                break;
                            }
                        }
                    }
                    else {
                        ImGui::SameLine();
                        ImGui::Text("Couldn't retrieve stores.");
                        // ======== END OF GUILD STORES
                    }

                    static std::map<std::string, std::string> configStores = {
                        {"/Game/Configs/RegisterStores/DA_Store_BlackMarket.DA_Store_BlackMarket", "Black Market"},
                        {"/Game/Configs/RegisterStores/DA_Store_Blacksmith.DA_Store_Blacksmith", "Blacksmith"},
                        {"/Game/Configs/RegisterStores/DA_Store_Housing.DA_Store_Housing", "City Hall"},
                        {"/Game/Configs/RegisterStores/DA_Store_Crops.DA_Store_Crops", "Farm"},
                        {"/Game/Configs/RegisterStores/DA_Store_Furniture.DA_Store_Furniture", "Furniture"},
                        {"/Game/Configs/RegisterStores/DA_Store_General.DA_Store_General", "General Store"},
                        {"/Game/Configs/RegisterStores/DA_Store_Inn.DA_Store_Inn", "Inn"},
                        {"/Game/Configs/RegisterStores/DA_Store_Library.DA_Store_Library", "Library"},
                        {"/Game/Configs/RegisterStores/DA_Store_Halloween.DA_Store_Halloween", "Spooky Moon Store"},
                        {"/Game/Configs/RegisterStores/DA_Store_Winter.DA_Store_Winter", "Winterlights Store"},
                        {"/Game/Configs/RegisterStores/DA_Store_MajiMarket_Fireworks.DA_Store_MajiMarket_Fireworks", "Maji Market Fireworks"},
                        {"/Game/Configs/RegisterStores/DA_Store_MajiMarket_ExperimentalFood.DA_Store_MajiMarket_ExperimentalFood", "Maji Market Reth"},
                        {"/Game/Configs/RegisterStores/DA_Store_MajiMarket_ChapaaBooth.DA_Store_MajiMarket_ChapaaBooth", "Maji Market Chapaa Booth"},
                        {"/Game/Configs/RegisterStores/DA_Store_LNY_ChapaaBooth.DA_Store_LNY_ChapaaBooth", "LNY - Chapaa Booth"},
                        {"/Game/Configs/RegisterStores/DA_Store_LNY_ChapaaChase.DA_Store_LNY_ChapaaChase", "LNY - Chapaa Chase"},
                        {"/Game/Configs/RegisterStores/DA_Store_LNY_Delaila.DA_Store_LNY_Delaila", "LNY - Delaila"},
                        {"/Game/Configs/RegisterStores/DA_Store_LNY_ExperimentalFood.DA_Store_LNY_ExperimentalFood", "LNY - Reth"},
                        {"/Game/Configs/RegisterStores/DA_Store_LNY_Fireworks.DA_Store_LNY_Fireworks", "LNY - Fireworks"},
                        {"/Game/Configs/RegisterStores/DA_Store_LNY_ZekiSnacks.DA_Store_LNY_ZekiSnacks", "LNY - Zeki"},
                    };
                    ImGui::PushItemWidth(width - spacing * 2.0f - buttonSize * 2.0f); //ImGui::PushItemWidth(CalcTextSize(stores[2])).x + style.FramePadding.x * 2.0f);
                    ImGui::SetNextItemWidth(250.0f);
                    if (ImGui::BeginCombo("##BuildingStores", configStores[BlackMarketDetours::SelectedStoreConfig].c_str())) {
                        for (auto& it : configStores) {
                            const std::string configStore = it.first;
                            const bool is_selected = (BlackMarketDetours::SelectedStoreConfig == configStore);
                            if (ImGui::Selectable(it.second.c_str(), is_selected)) {
                                BlackMarketDetours::SelectedStoreConfig = configStore;
                            }
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::PopItemWidth();

                    if (WidgetDetours::GameplayUIManager && WidgetDetours::GameplayUIManager->IsValidLowLevel()) {
                        ImGui::SameLine();
                        if (ImGui::Button("Open Building Store##OpenBuildingStoreBtn")) {
                            BlackMarketDetours::bShouldOpenConfigStore = true;
                            show = false;
                        }
                    }
                    else {
                        ImGui::SameLine();
                        ImGui::Text("Couldn't retrieve stores.");
                        // ======== END OF GUILD STORES
                    }
                    ImGui::Text("Some stores are only available at night.");
                    ImGui::PushTextWrapPos(350);
                    ImGui::Text("Opening during off hours will show no inventory, or the inventory of a previously opened store.");
                    ImGui::PopTextWrapPos();
                }
                else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Player Features##PlayerSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    if (WidgetDetours::GameplayUIManager && WidgetDetours::GameplayUIManager->IsValidLowLevel()) {
                        if (ImGui::Button("Open Player Storage##OpenPlayerStorageBtn")) {
                            WidgetDetours::GameplayUIManager->Implementation_OpenPlayerStorage();
                            show = false;
                        }
                    }
                }
                else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }
        }
        // ==================================== 4 Skills & Tools TAB
        else if (OpenTab == 4) {
            ImGui::Columns(2, nullptr, false);

            UFishingComponent* FishingComponent = nullptr;
            auto EquippedTool = ETools::None;
            std::string EquippedName;
            
            if (ValeriaCharacter) {
                FishingComponent = ValeriaCharacter->GetFishing();
                EquippedName = ValeriaCharacter->GetEquippedItem().ItemType->Name.ToString();
            }
            
            if (EquippedName.find("Tool_Axe_") != std::string::npos) {
                EquippedTool = ETools::Axe;
            } else if (EquippedName.find("Tool_InsectBallLauncher_") != std::string::npos) {
                EquippedTool = ETools::Belt;
            } else if (EquippedName.find("Tool_Bow_") != std::string::npos) {
                EquippedTool = ETools::Bow;
            } else if (EquippedName.find("Tool_Rod_") != std::string::npos) {
                EquippedTool = ETools::FishingRod;
            } else if (EquippedName.find("Tool_Hoe_") != std::string::npos) {
                EquippedTool = ETools::Hoe;
            } else if (EquippedName.find("Tool_Pick") != std::string::npos) {
                EquippedTool = ETools::Pick;
            } else if (EquippedName.find("Tool_WateringCan_") != std::string::npos) {
                EquippedTool = ETools::WateringCan;
            } else {
                EquippedTool = ETools::None;
            }
            
            if (ImGui::CollapsingHeader("Tool Settings##ToolsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    ImGui::Text("Equipped Tool : %s", STools[static_cast<int>(EquippedTool)]);
                    ImGui::Spacing();

#ifdef ENABLE_SUPPORTER_FEATURES
                    IMGUI_CHECKBOX("Auto FastTools (Experimental)", &Configuration::bEnableAutoToolUse, "Automatically chop or mine materials around you as you play.")
                    ImGui::Spacing();
#endif
                    IMGUI_CHECKBOX("Auto Gather Nearby Loot", &Configuration::bEnableAutoGather, "Automatically gather forageables (within range) when you play.")
                    ImGui::Spacing();
                    IMGUI_CHECKBOX("Skip Cooking Minigames", &Configuration::bEnableMinigameSkip, "Skips the cooking minigame process completely.")
                }
                else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Fishing Settings##FishingHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (FishingComponent) {
                    IMGUI_CHECKBOX("No Rod Durability Loss", &Configuration::bFishingNoDurability, "Never break your fishing rod.")
                    IMGUI_CHECKBOX("Enable Multiplayer Help", &Configuration::bFishingMultiplayerHelp, "Force fishing with other players for extra quest completion.")
                    IMGUI_CHECKBOX("Always Perfect Catch", &Configuration::bFishingPerfectCatch, "Fishing will always result in a perfect catch.")
                    IMGUI_CHECKBOX("Instant Catch", &Configuration::bFishingInstantCatch, "Catch fish as soon as your bobber hits the water.")
                    IMGUI_CHECKBOX("Sell All Fish", &Configuration::bFishingSell, "When fishing, automatically sell all fish from your inventory.")
                    IMGUI_CHECKBOX("Discard All Junk", &Configuration::bFishingDiscard, "When fishing, automatically remove junk items from your inventory.")
                    IMGUI_CHECKBOX("Open and Store Makeshift Decor", &Configuration::bFishingOpenStoreWaterlogged, "When fishing, automatically move valuables to your home base storage.")

                    if (EquippedTool == ETools::FishingRod) {
                        IMGUI_CHECKBOX("Auto Fast Fishing", &FishingDetours::bEnableAutoFishing, "Fish extremely fast. Pairs nicely with other fishing features.")
                        IMGUI_CHECKBOX("Require Holding Left-Click To Auto Fish", &Configuration::bRequireClickFishing, "Require holding the left mouse button to toggle the fast fishing.")
                    }
                    else {
                        ImGui::Spacing();
                        ImGui::Text("[ EQUIP FISHING ROD TO VIEW FAST FISHING OPTIONS ]");
                        ImGui::Spacing();
                        Configuration::bRequireClickFishing = true;
                    }

                    IMGUI_CHECKBOX("Force Fishing Pool", &FishingDetours::bOverrideFishingSpot, "Force all catches to result from the selected pool.")

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(200.0f);
                    if (ImGui::Combo("##FishingSpotsCombo", &FishingDetours::bSelectedFishingSpot, FishingDetours::bFishingSpots, IM_ARRAYSIZE(FishingDetours::bFishingSpots))) {
                        if (FishingDetours::bSelectedFishingSpot > 0) {
                            FishingDetours::sOverrideFishingSpot = UKismetStringLibrary::Conv_StringToName(FString(FishingDetours::bFishingSpotsFString[FishingDetours::bSelectedFishingSpot - 1]));
                        }
                        else {
                            FishingDetours::bSelectedFishingSpot = 0;
                            FishingDetours::sOverrideFishingSpot = {};
                            FishingDetours::bOverrideFishingSpot = false;
                        }
                    }
                }
                else {
                    if (!ValeriaCharacter) {
                        ImGui::Text("Waiting for character initialization...");
                    }
                    else {
                        ImGui::Text("Fishing component not available.");
                    }
                }
            }
        }
		// ==================================== 5 Mods & Settings TAB
		else if (OpenTab == 5) {
            ImGui::Columns(2, nullptr, false);

            if (ImGui::CollapsingHeader("Character Settings##CharacterSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    IMGUI_CHECKBOX("Anti AFK", &Configuration::bEnableAntiAfk, "Stop inactivity disconnects and play forever.")
                    ImGui::Spacing();
                    IMGUI_CHECKBOX("Unlimited Wardrobe Respecs", &Configuration::bEnableUnlimitedWardrobeRespec, "Allows yous to change genders/appereances without the cooldown.")
                    ImGui::Spacing();
                    
                    IMGUI_CHECKBOX("Further Interaction Distance", &Configuration::bEnableInteractionMods, "Interact with things from further distances.")
                    if (Configuration::bEnableInteractionMods) {
                        float step = 1.0f;
                        float step_fast = 100.0f;
                        ImGui::SetNextItemWidth(200.0f);
                        if (ImGui::InputScalar("Interaction Distance", ImGuiDataType_Float, &Configuration::InteractionRadius, &step, &step_fast, "%.1f", ImGuiInputTextFlags_None)) {
                            Configuration::InteractionRadius = std::clamp(Configuration::InteractionRadius, 1.0f, 99999.0f);
                            Configuration::Save();
                        }
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                            ImGui::SetTooltip("Adjust the maximum distance for interactions. Range: 1.0 to 99999.0");
                    }
                    
                    ImGui::Spacing();
                    IMGUI_CHECKBOX("Teleport To Waypoints", &Configuration::bEnableWaypointTeleport, "Automatically teleports you at your waypoints locations.")
                    ImGui::Spacing();
                    IMGUI_CHECKBOX("Easy Quest Mode", &Configuration::bEasyModeActive, "Experimental - May skip some questing requirements.")
                    ImGui::Spacing();
                    if (ImGui::Button("Open Player Storage##OpenPlayerStorageBtn")) {
                        if (WidgetDetours::GameplayUIManager && WidgetDetours::GameplayUIManager->IsValidLowLevel()) {
                            WidgetDetours::GameplayUIManager->Implementation_OpenPlayerStorage();
                            show = false;
                        }
                    }
                    ImGui::Spacing();
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            } 

		    if (ImGui::CollapsingHeader("Origin Settings##OriginSettingsHeader", ImGuiTreeNodeFlags_DefaultOpen)) {
		        IMGUI_CHECKBOX("Show Watermark", &Configuration::bShowWatermark, "Toggle display of the custom watermark on the screen.")
                ImGui::Spacing();
		        IMGUI_CHECKBOX("Active Mods Window", &Configuration::showEnabledFeaturesWindow, "Display a list of currently activated mods")
                ImGui::Spacing();
		        ImGui::SetNextItemWidth(200.0f);
		        if (ImGui::Combo("##ThemesCombo", &Configuration::selectedTheme, themes, IM_ARRAYSIZE(themes))) {
		            switch (Configuration::selectedTheme) {
		            case 0: // Origin
		                ImGui::StyleColorsDark();
		                Configuration::Save();
		                break;
		            case 1: // Spectrum Light
		                ImGui::Spectrum::StyleColorsSpectrum();
		                Configuration::Save();
		                break;
		            case 2: // Magma
		                ImGui::Magma::StyleColorsMagma();
		                Configuration::Save();
                        break;
                    case 3: // Custom theme
                        Configuration::ApplyCustomTheme();
                        Configuration::Save();
                        break;
		            default:
		                break;
		            }
		        }
                ImGui::SameLine();
                ImGui::Text("Select Theme");

                ImGuiStyle* style = &ImGui::GetStyle();
                style->Colors;

                static int selectedColor = -1;
                static std::string currentColorName = "Select a widget";

                static bool openCustomThemePopup = false;

                if (Configuration::selectedTheme != 3) {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, (Configuration::selectedTheme == 3));
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, style->Alpha * 0.25f);
                    ImGui::Button("Edit Custom Theme");
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();

                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select 'Custom Theme' to begin editing.");
                }
                else {
					if (ImGui::Button("Edit Custom Theme")) {
						openCustomThemePopup = true;

						previousColors.clear(); // Ensuring previousColors is fresh.

						for (int i = 0; i < ImGuiCol_COUNT; ++i) {
							ImVec4 col = style->Colors[i];
							previousColors[i] = col;
						}
					}
				}

				if (ImGui::BeginPopupModal("Custom Theme Editor##CustomThemeEditor", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
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
						if (ImGui::ColorEdit4("##GuiColorPicker", (float*)&color, ImGuiColorEditFlags_NoInputs)) {
							auto colorIt = Configuration::customColors.find(selectedColor);
							if (colorIt != Configuration::customColors.end()) { // Check if color already exists in customColors & replace instead of inserting.
								if (colorIt->second != color) {
									colorIt->second = color;
								}
							}
							else {
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
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Undo Changes.");

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
						Configuration::Save();
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
                    static bool showUnlockerConfirmationPopup = false;
                    
                    UPlacementComponent* PlacementComponent = ValeriaCharacter->GetPlacement();
                    if (PlacementComponent) {
                        IMGUI_CHECKBOX("Place Items Anywhere", &Configuration::bPlaceAnywhere, "Allow for placement of housing items anywhere.")
                        ImGui::Spacing();
                    } else {
                        ImGui::Text("No Placement Component available.");
                        ImGui::Spacing();
                    }
                    
#ifdef ENABLE_SUPPORTER_FEATURES
                    IMGUI_CHECKBOX("Temporarily Unlock Content", &Configuration::bEnableOutfitCustomization, "Temporarily unlocks some content like furniture and colors.")
                    ImGui::Spacing();
                    IMGUI_CHECKBOX("Unlock Landscapes", &Configuration::bEnableFreeLandscape, "Allows you to activate any landscape.")
                    ImGui::Spacing();
#endif
                    IMGUI_CHECKBOX("Fulfill Infinite Player Requests", &Configuration::bEnableRequestMods, "Fulfill as many requests as you want. May require multiple tries.")
                    ImGui::Spacing();
                    IMGUI_CHECKBOX("Black Market Prize Wheel", &Configuration::bEnablePrizeWheel, "Choose which positions on the wheel to accept.")
                    ImGui::Spacing();
                    if (Configuration::bEnablePrizeWheel) {
                        IMGUI_CHECKBOX("Use Locked Storage", &Configuration::bPrizeWheelUseLocked, "Use locked storage instead of general storage.")
                        ImGui::BeginGroupPanel("Prize Wheel Positions");
                        {
                            ImGui::Spacing();
                            IMGUI_CHECKBOX("0| Red    - Fireworks", &Configuration::PrizeWheelSlots[0])
                            IMGUI_CHECKBOX("1| Purple - Decor", &Configuration::PrizeWheelSlots[1])
                            IMGUI_CHECKBOX("2| Blue   - Ingredients", &Configuration::PrizeWheelSlots[2])
                            IMGUI_CHECKBOX("3| Green  - Gold", &Configuration::PrizeWheelSlots[3])
                            IMGUI_CHECKBOX("4| Red    - Fireworks", &Configuration::PrizeWheelSlots[4])
                            IMGUI_CHECKBOX("5| Brown  - Lucky Coins", &Configuration::PrizeWheelSlots[5])
                            IMGUI_CHECKBOX("6| Blue   - Ingredients", &Configuration::PrizeWheelSlots[6])
                            IMGUI_CHECKBOX("7| Green  - Gold", &Configuration::PrizeWheelSlots[7])
                            IMGUI_CHECKBOX("8| Yellow - Plushie", &Configuration::PrizeWheelSlots[8])
                            ImGui::Spacing();
                        }
                        ImGui::EndGroupPanel();
                        ImGui::Spacing();
                    }

                    static bool teleportLootDisabled = true;
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, teleportLootDisabled);
                    if (ImGui::Checkbox("[Disabled] Teleport Dropped Loot to Player (Currently Broken)", &Configuration::bEnableLootbagTeleportation)) {
                        Configuration::Save();
                    }
                    ImGui::PopItemFlag();
                    
#ifdef ENABLE_SUPPORTER_FEATURES
                    // Confirmation popup logic
                    if (showUnlockerConfirmationPopup) {
                        ImGui::OpenPopup("Confirm Activation");
                    }

                    if (ImGui::BeginPopupModal("Confirm Activation", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                        ImGui::Text("Do not use unreleased content - you may receive a ban.\nAre you sure you want to proceed?\n\n");
                        ImGui::Separator();
                        // Calculate button width for equal spacing
                        float buttonWidth = ImGui::GetContentRegionAvail().x * 0.5f - ImGui::GetStyle().ItemSpacing.x;
                        // Centering the buttons
                        ImGui::Dummy(ImVec2(0.0f, 0.0f)); // if additional spacing is needed above

                        ImGui::SetNextItemWidth(buttonWidth);
                        if (ImGui::Button("Yes, Proceed", ImVec2(buttonWidth, 0))) {
                            Configuration::Save();
                            showUnlockerConfirmationPopup = false;
                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::SameLine();

                        ImGui::SetNextItemWidth(buttonWidth);
                        if (ImGui::Button("No, Cancel", ImVec2(buttonWidth, 0))) {
                            Configuration::bEnableOutfitCustomization = false;
                            showUnlockerConfirmationPopup = false;
                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::Dummy(ImVec2(0.0f, 0.0f)); // if additional spacing is needed below
                        ImGui::EndPopup();
                    }
#endif
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