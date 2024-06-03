#include "ConfigHandler.h"
#include "PaliaOverlay.h"
#include "DetourManager.h"
#include <SDK/Palia_parameters.hpp>

#include "ImGuiExt.h"
#include "SDKExt.h"
#include "Utils.h"

#include<iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <algorithm>
#include <imgui_internal.h>

using namespace SDK;

std::vector<std::string> debugger;
DetourManager gDetourManager;

std::map<int, std::string> PaliaOverlay::CreatureQualityNames = {
    {0, "Unknown"},
    {1, "T1"},
    {2, "T2"},
    {3, "T3"},
    {4, "Chase"}
};
std::map<int, std::string> PaliaOverlay::BugQualityNames = {
    {0, "Unknown"},
    {1, "Common"},
    {2, "Uncommon"},
    {3, "Rare"},
    {4, "Rare2"},
    {5, "Epic"}
};
std::map<int, std::string> PaliaOverlay::GatherableSizeNames = {
    {0, "Unknown"},
    {1, "Sm"},
    {2, "Md"},
    {3, "Lg"},
    {4, "Bush"}
};

inline ConfigHandler configHandler("C:/ProgramData/OriginPalia/config", "/overlay_config.json");

float PaliaOverlay::ESPTextScale = 1.0f;

void PaliaOverlay::SetupColors() {
    // Forageable colors
    for (const int pos : ForageableCommon) {
        ForageableColors[pos] = IM_COL32(0xCD, 0xCD, 0xCD, 0xFF); // Light Gray
    }
    for (const int pos : ForageableUncommon) {
        ForageableColors[pos] = IM_COL32(0x32, 0xCD, 0x32, 0xFF); // Lime Green
    }
    for (const int pos : ForageableRare) {
        ForageableColors[pos] = IM_COL32(0x1E, 0x90, 0xFF, 0xFF); // Dodger Blue
    }
    for (const int pos : ForageableEpic) {
        ForageableColors[pos] = IM_COL32(0xFF, 0xD7, 0x00, 0xFF); // Gold
    }

    // Animal colors
    for (auto& AnimalColor : AnimalColors) {
        AnimalColor[static_cast<int>(ECreatureQuality::Tier1)] = IM_COL32(0xCD, 0xCD, 0xCD, 0xFF); // Light Gray
        AnimalColor[static_cast<int>(ECreatureQuality::Tier2)] = IM_COL32(0x32, 0xCD, 0x32, 0xFF); // Lime Green
        AnimalColor[static_cast<int>(ECreatureQuality::Tier3)] = IM_COL32(0x1E, 0x90, 0xFF, 0xFF); // Dodger Blue
        AnimalColor[static_cast<int>(ECreatureQuality::Chase)] = IM_COL32(0xFF, 0xD7, 0x00, 0xFF); // Gold
    }

    // Bug colors
    for (auto& BugColor : BugColors) {
        BugColor[static_cast<int>(EBugQuality::Common)] = IM_COL32(0xCD, 0xCD, 0xCD, 0xFF); // Light Gray
        BugColor[static_cast<int>(EBugQuality::Uncommon)] = IM_COL32(0x32, 0xCD, 0x32, 0xFF); // Lime Green
        BugColor[static_cast<int>(EBugQuality::Rare)] = IM_COL32(0x1E, 0x90, 0xFF, 0xFF); // Dodger Blue
        BugColor[static_cast<int>(EBugQuality::Rare2)] = IM_COL32(0x00, 0xBF, 0xFF, 0xFF); // Deep Sky Blue
        BugColor[static_cast<int>(EBugQuality::Epic)] = IM_COL32(0xFF, 0xD7, 0x00, 0xFF); // Gold
    }

    // Player & Entities colors
    SingleColors[static_cast<int>(EOneOffs::Player)] = IM_COL32(0xFF, 0x63, 0x47, 0xFF); // Tomato Red
    SingleColors[static_cast<int>(EOneOffs::NPC)] = IM_COL32(0xDE, 0xB8, 0x87, 0xFF); // Burly Wood
    SingleColors[static_cast<int>(EOneOffs::Loot)] = IM_COL32(0xEE, 0x82, 0xEE, 0xFF); // Violet
    SingleColors[static_cast<int>(EOneOffs::Quest)] = IM_COL32(0xFF, 0xA5, 0x00, 0xFF); // Orange
    SingleColors[static_cast<int>(EOneOffs::RummagePiles)] = IM_COL32(0xFF, 0x45, 0x00, 0xFF); // Orange Red
    SingleColors[static_cast<int>(EOneOffs::Others)] = IM_COL32(0xFF, 0xFF, 0xFF, 0xFF); // White

    // Define a different color for Stables
    SingleColors[static_cast<int>(EOneOffs::Stables)] = IM_COL32(0x8B, 0x45, 0x13, 0xFF); // Saddle Brown
};

void PaliaOverlay::DrawHUD() {
    // Loading configuration or setting defaults
    auto Overlay = static_cast<PaliaOverlay*>(Instance);
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard navigation once at initialization if possible.

    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGuiStyle& style = ImGui::GetStyle();
    const float prevWindowRounding = style.WindowRounding;
    style.WindowRounding = 5.0f; // Temporary change of style.

    // Calculate watermark text only once, not in the drawing loop.
    std::string watermarkText = "OriginPalia Menu By Wimberton & The UnknownCheats Community";
    if (CurrentLevel && (CurrentMap == "MAP_PreGame" || CurrentMap == "Unknown")) {
        watermarkText = "Waiting for the game to load...";
    }

    ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - ImGui::CalcTextSize(watermarkText.c_str()).x) * 0.5f, 10.0f));
    ImGui::Begin("Watermark", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
    ImGui::Text("%s", watermarkText.c_str());
    ImGui::End();

    style.WindowRounding = prevWindowRounding; // Restore style after the temporary change.

    APlayerController* PlayerController = nullptr;
    AValeriaPlayerController* ValeriaController = nullptr;
    AValeriaCharacter* ValeriaCharacter = nullptr;

    if (UWorld* World = GetWorld()) {
        if (UGameInstance* GameInstance = World->OwningGameInstance; GameInstance && GameInstance->LocalPlayers.Num() > 0) {
            if (ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0]) {
                if (PlayerController = LocalPlayer->PlayerController; PlayerController && PlayerController->Pawn) {
                    ValeriaController = static_cast<AValeriaPlayerController*>(PlayerController);
                    if (ValeriaController) {
                        ValeriaCharacter = static_cast<AValeriaPlayerController*>(PlayerController)->GetValeriaCharacter();
                    }
                }
            }
        }
    }

    if (ValeriaController) {
        if (UTrackingComponent* TrackingComponent = ValeriaController->GetTrackingComponent(); TrackingComponent != nullptr) {
            gDetourManager.SetupDetour(TrackingComponent);
        }
    }

    // HOOKS
    if (ValeriaCharacter) {
        // INVENTORY COMPONENT
        if (UInventoryComponent* InventoryComponent = ValeriaCharacter->GetInventory(); InventoryComponent != nullptr) {
            gDetourManager.SetupDetour(InventoryComponent);
        }

        // FISHING COMPONENT
        if (UFishingComponent* FishingComponent = ValeriaCharacter->GetFishing(); FishingComponent != nullptr) {
            gDetourManager.SetupDetour(FishingComponent);
        }

        // FIRING COMPONENT
        if (UProjectileFiringComponent* FiringComponent = ValeriaCharacter->GetFiringComponent(); FiringComponent != nullptr) {
            gDetourManager.SetupDetour(FiringComponent);
        }

        // MOVEMENT COMPONENT
        if (UValeriaCharacterMoveComponent* ValeriaMovementComponent = ValeriaCharacter->GetValeriaCharacterMovementComponent(); ValeriaMovementComponent != nullptr) {
            gDetourManager.SetupDetour(ValeriaMovementComponent);
        }

        // PLACEMENT COMPONENT
        if (UPlacementComponent* PlacementComponent = ValeriaCharacter->GetPlacement(); PlacementComponent != nullptr) {
            gDetourManager.SetupDetour(PlacementComponent);
        }
    }

    // HOOKING PROCESSEVENT IN AHUD
    if (PlayerController && HookedClient != PlayerController->MyHUD && PlayerController->MyHUD != nullptr) {
        gDetourManager.SetupDetour(PlayerController->MyHUD);
    }

    configHandler.UpdateConfiguration(Overlay);
}

void PaliaOverlay::DrawOverlay() {
    bool show = true;
    const ImGuiIO& io = ImGui::GetIO();
    constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;

    // Calculate the center position for the window
    const auto center_pos = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    const auto window_size = ImVec2(1450, 950); // Set the initial window size
    const auto window_pos = ImVec2(center_pos.x - window_size.x * 0.5f, center_pos.y - window_size.y * 0.5f);

    // Set the initial window position to the center of the screen
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.98f);

    const auto WindowTitle = std::string("OriginPalia Menu V2.2 - Game Version 0.180.0");
    PaliaOverlay* Overlay = static_cast<PaliaOverlay*>(OverlayBase::Instance);

    if (ImGui::Begin(WindowTitle.data(), &show, window_flags)) {
        static int OpenTab = 0;

        // Draw tabs
        if (ImGui::BeginTabBar("OverlayTabs")) {
            if (ImGui::BeginTabItem("ESP & Visuals")) {
                OpenTab = 0;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Aimbots & Fun")) {
                OpenTab = 1;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Movement & Teleport")) {
                OpenTab = 2;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Selling & Items")) {
                OpenTab = 3;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Skills & Tools")) {
                OpenTab = 4;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Housing & Decorating")) {
                OpenTab = 5;
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        // ==================================== 0 Visuals & ESPs TAB
        if (OpenTab == 0) {
            ImGui::Columns(3, nullptr, false);

            // Base ESP controls
            if (ImGui::CollapsingHeader("Visual Settings - General", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Enable ESP", &bEnableESP);
                
                ImGui::SliderFloat("ESP Text Scale", &ESPTextScale, 0.5f, 3.0f, "%.1f");
                
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                    ImGui::SetTooltip("Adjust the scale of ESP text size.");

                ImGui::Checkbox("Limit Distance", &bEnableESPCulling);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                    ImGui::SetTooltip("Limit the maximum distance the ESP will render. Turn this down to a low value if you're having performance problems.");

                ImGui::InputInt("Distance", &CullDistance);

                ImGui::Checkbox("Enable InteliAim Circle", &bDrawFOVCircle);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                    ImGui::SetTooltip("Enable the smart FOV targeting system. Teleport to actors, enable aimbots, and more.");

                ImGui::SliderFloat("InteliAim Radius", &FOVRadius, 10.0f, 600.0f, "%1.0f");
            }

            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Animals")) {
                ImGui::BeginTable("Animals", 3);
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Text("Show");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Sernuk")) {
                        Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)] = !Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)];
                    }
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("##Sernuk", &Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)])) {
                    }
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Sernuk", &AnimalColors[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Elder Sernuk")) {
                        Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier2)] = !Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier2)];
                    }
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("##ElderSernuk", &Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier2)])) {
                    }
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##ElderSernuk", &AnimalColors[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier2)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Proudhorn Sernuk")) {
                        Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier3)] = !Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier3)];
                    }
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("##ProudhornSernuk", &Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier3)])) {
                    }
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##ProudhornSernuk", &AnimalColors[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier3)]);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Text("Show");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Chapaa")) {
                        Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)] = !Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)];
                    }
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("##Chapaa", &Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)])) {
                    }
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Chapaa", &AnimalColors[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Striped Chapaa")) {
                        Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier2)] = !Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier2)];
                    }
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("##StripedChapaa", &Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier2)])) {
                    }
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##StripedChapaa", &AnimalColors[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier2)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Azure Chapaa")) {
                        Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier3)] = !Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier3)];
                    }
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("##AzureChapaa", &Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier3)])) {
                    }
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##AzureChapaa", &AnimalColors[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier3)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Minigame Chapaa")) {
                        Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Chase)] = !Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Chase)];
                    }
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("##MinigameChapaa", &Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Chase)])) {
                    }
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MinigameChapaa", &AnimalColors[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Chase)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Text("Show");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    if (ImGui::Button("Muujin")) {
                        Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier1)] = !Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier1)];
                    }
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("##Muujin", &Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier1)])) {
                    }
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Muujin", &AnimalColors[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier1)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Banded Muujin")) {
                        Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier2)] = !Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier2)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BandedMuujin", &Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier2)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##BandedMuujin", &AnimalColors[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier2)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Bluebristle Muujin")) {
                        Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier3)] = !Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier3)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BluebristleMuujin", &Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier3)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##BluebristleMuujin", &AnimalColors[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier3)]);
                }
                ImGui::EndTable();
            }
            if (ImGui::CollapsingHeader("Ores")) {
                ImGui::BeginTable("Ores", 5);
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Sm", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Med", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Lg", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Text("Sm");
                    ImGui::TableNextColumn();
                    ImGui::Text("Med");
                    ImGui::TableNextColumn();
                    ImGui::Text("Lg");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Clay")) {
                        Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(EGatherableSize::Large)] = !Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(EGatherableSize::Large)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ClayLg", &Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Clay", &OreColors[static_cast<int>(EOreType::Clay)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Stone")) {
                        Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Large)] =
                            Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Medium)] =
                            Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Small)] =
                            !Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##StoneSm", &Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##StoneMed", &Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##StoneLg", &Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Stone", &OreColors[static_cast<int>(EOreType::Stone)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Copper")) {
                        Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Large)] =
                            Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Medium)] =
                            Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Small)] =
                            !Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CopperSm", &Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CopperMed", &Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CopperLg", &Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Copper", &OreColors[static_cast<int>(EOreType::Copper)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Iron")) {
                        Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Large)] =
                            Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Medium)] =
                            Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Small)] =
                            !Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##IronSm", &Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##IronMed", &Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##IronLg", &Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Iron", &OreColors[static_cast<int>(EOreType::Iron)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Palium")) {
                        Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Large)] =
                            Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Medium)] =
                            Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Small)] =
                            !Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PaliumSm", &Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PaliumMed", &Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PaliumLg", &Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Palium", &OreColors[static_cast<int>(EOreType::Palium)]);
                }
                ImGui::EndTable();
            }
            if (ImGui::CollapsingHeader("Forageables")) {
                ImGui::Text("Enable all:");
                ImGui::SameLine();

                if (ImGui::Button("Common##Forage")) {
                    for (int pos : ForageableCommon) {
                        Forageables[pos][1] = Forageables[pos][0] = !Forageables[pos][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Uncommon##Forage")) {
                    for (int pos : ForageableUncommon) {
                        Forageables[pos][1] = Forageables[pos][0] = !Forageables[pos][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Rare##Forage")) {
                    for (int pos : ForageableRare) {
                        Forageables[pos][1] = Forageables[pos][0] = !Forageables[pos][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Epic##Forage")) {
                    for (int pos : ForageableEpic) {
                        Forageables[pos][1] = Forageables[pos][0] = !Forageables[pos][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Star##Forage")) {
                    for (auto& Forageable : Forageables) {
                        Forageable[1] = !Forageable[1];
                    }
                }
                ImGui::BeginTable("Forageables", 4);
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Normal", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Star", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Beach");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Coral");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Coral", &Forageables[static_cast<int>(EForageableType::Coral)][0]);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Coral", &ForageableColors[static_cast<int>(EForageableType::Coral)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Oyster");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Oyster", &Forageables[static_cast<int>(EForageableType::Oyster)][0]);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Oyster", &ForageableColors[static_cast<int>(EForageableType::Oyster)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Shell");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Shell", &Forageables[static_cast<int>(EForageableType::Shell)][0]);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Shell", &ForageableColors[static_cast<int>(EForageableType::Shell)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Flower");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Briar Daisy");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PoisonFlower", &Forageables[static_cast<int>(EForageableType::PoisonFlower)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PoisonFlowerP", &Forageables[static_cast<int>(EForageableType::PoisonFlower)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##PoisonFlower", &ForageableColors[static_cast<int>(EForageableType::PoisonFlower)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Crystal Lake Lotus");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##WaterFlower", &Forageables[static_cast<int>(EForageableType::WaterFlower)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##WaterFlowerP", &Forageables[static_cast<int>(EForageableType::WaterFlower)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##WaterFlower", &ForageableColors[static_cast<int>(EForageableType::WaterFlower)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Heartdrop Lily");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Heartdrop", &Forageables[static_cast<int>(EForageableType::Heartdrop)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##HeartdropP", &Forageables[static_cast<int>(EForageableType::Heartdrop)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Heartdrop", &ForageableColors[static_cast<int>(EForageableType::Heartdrop)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Sundrop Lily");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Sundrop", &Forageables[static_cast<int>(EForageableType::Sundrop)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SundropP", &Forageables[static_cast<int>(EForageableType::Sundrop)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Sundrop", &ForageableColors[static_cast<int>(EForageableType::Sundrop)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Moss");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Dragon's Beard Peat");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonsBeard", &Forageables[static_cast<int>(EForageableType::DragonsBeard)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonsBeardP", &Forageables[static_cast<int>(EForageableType::DragonsBeard)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##DragonsBeard", &ForageableColors[static_cast<int>(EForageableType::DragonsBeard)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Emerald Carpet Moss");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##EmeraldCarpet", &Forageables[static_cast<int>(EForageableType::EmeraldCarpet)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##EmeraldCarpetP", &Forageables[static_cast<int>(EForageableType::EmeraldCarpet)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##EmeraldCarpet", &ForageableColors[static_cast<int>(EForageableType::EmeraldCarpet)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Mushroom");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Brightshroom");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MushroomBlue", &Forageables[static_cast<int>(EForageableType::MushroomBlue)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MushroomBlueP", &Forageables[static_cast<int>(EForageableType::MushroomBlue)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MushroomBlue", &ForageableColors[static_cast<int>(EForageableType::MushroomBlue)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Mountain Morel");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MushroomRed", &Forageables[static_cast<int>(EForageableType::MushroomRed)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MushroomRedP", &Forageables[static_cast<int>(EForageableType::MushroomRed)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MushroomRed", &ForageableColors[static_cast<int>(EForageableType::MushroomRed)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Spice");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Dari Cloves");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DariCloves", &Forageables[static_cast<int>(EForageableType::DariCloves)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DariClovesP", &Forageables[static_cast<int>(EForageableType::DariCloves)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##DariCloves", &ForageableColors[static_cast<int>(EForageableType::DariCloves)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Heat Root");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##HeatRoot", &Forageables[static_cast<int>(EForageableType::HeatRoot)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##HeatRootP", &Forageables[static_cast<int>(EForageableType::HeatRoot)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##HeatRoot", &ForageableColors[static_cast<int>(EForageableType::HeatRoot)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Spice Sprouts");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SpicedSprouts", &Forageables[static_cast<int>(EForageableType::SpicedSprouts)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SpicedSproutsP", &Forageables[static_cast<int>(EForageableType::SpicedSprouts)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##SpicedSprouts", &ForageableColors[static_cast<int>(EForageableType::SpicedSprouts)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Sweet Leaf");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SweetLeaves", &Forageables[static_cast<int>(EForageableType::SweetLeaves)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SweetLeavesP", &Forageables[static_cast<int>(EForageableType::SweetLeaves)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##SweetLeaves", &ForageableColors[static_cast<int>(EForageableType::SweetLeaves)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Vegetable");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Wild Garlic");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Garlic", &Forageables[static_cast<int>(EForageableType::Garlic)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GarlicP", &Forageables[static_cast<int>(EForageableType::Garlic)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Garlic", &ForageableColors[static_cast<int>(EForageableType::Garlic)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Wild Ginger");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Ginger", &Forageables[static_cast<int>(EForageableType::Ginger)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GingerP", &Forageables[static_cast<int>(EForageableType::Ginger)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Ginger", &ForageableColors[static_cast<int>(EForageableType::Ginger)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Wild Green Onion");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GreenOnion", &Forageables[static_cast<int>(EForageableType::GreenOnion)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GreenOnionP", &Forageables[static_cast<int>(EForageableType::GreenOnion)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##GreenOnion", &ForageableColors[static_cast<int>(EForageableType::GreenOnion)]);
                }
                ImGui::EndTable();
            }

            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Bugs")) {
                ImGui::Text("Enable all:");
                ImGui::SameLine();
                if (ImGui::Button("Common##Bugs")) {
                    for (auto& Bug : Bugs) {
                        Bug[static_cast<int>(EBugQuality::Common)][1] = Bug[static_cast<int>(EBugQuality::Common)][0] = !Bug[static_cast<int>(EBugQuality::Common)][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Uncommon##Bugs")) {
                    for (auto& Bug : Bugs) {
                        Bug[static_cast<int>(EBugQuality::Uncommon)][1] = Bug[static_cast<int>(EBugQuality::Uncommon)][0] = !Bug[static_cast<int>(EBugQuality::Uncommon)][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Rare##Bugs")) {
                    for (auto& Bug : Bugs) {
                        Bug[static_cast<int>(EBugQuality::Rare)][1] = Bug[static_cast<int>(EBugQuality::Rare)][0] = !Bug[static_cast<int>(EBugQuality::Rare)][0];
                        Bug[static_cast<int>(EBugQuality::Rare2)][1] = Bug[static_cast<int>(EBugQuality::Rare2)][0] = !Bug[static_cast<int>(EBugQuality::Rare2)][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Epic##Bugs")) {
                    for (auto& Bug : Bugs) {
                        Bug[static_cast<int>(EBugQuality::Epic)][1] = Bug[static_cast<int>(EBugQuality::Epic)][0] = !Bug[static_cast<int>(EBugQuality::Epic)][0];
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Star##Bugs")) {
                    for (auto& Bug : Bugs) {
                        for (auto& j : Bug) {
                            j[1] = !j[1];
                        }
                    }
                }
                ImGui::BeginTable("Bugs", 4);
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Normal", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Star", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Bee");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Bahari Bee");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeeU", &Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeeUP", &Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##BeeU", &BugColors[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Golden Glory Bee");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeeR", &Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeeRP", &Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Bee", &BugColors[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Beetle");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Spotted Stink Bug");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleC", &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Common)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleCP", &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Common)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##BeetleC", &BugColors[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Proudhorned Stag Beetle");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleU", &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleUP", &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##BeetleU", &BugColors[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Raspberry Beetle");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleR", &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleRP", &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##BeetleR", &BugColors[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Rare)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Ancient Amber Beetle");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleE", &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Epic)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BeetleEP", &Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Epic)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##BeetleE", &BugColors[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Epic)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Butterfly");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Common Blue Butterfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyC", &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Common)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyCP", &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Common)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##ButterflyC", &BugColors[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Duskwing Butterfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyU", &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyUP", &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##ButterflyU", &BugColors[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Brighteye Butterfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyR", &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyRP", &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##ButterflyR", &BugColors[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Rare)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Rainbow-Tipped Butterfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyE", &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Epic)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##ButterflyEP", &Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Epic)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##ButterflyE", &BugColors[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Epic)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Cicada");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Common Bark Cicada");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CicadaC", &Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Common)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CicadaCP", &Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Common)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CicadaC", &BugColors[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Cerulean Cicada");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CicadaU", &Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CicadaUP", &Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CicadaU", &BugColors[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Spitfire Cicada");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CicadaR", &Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CicadaRP", &Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CicadaR", &BugColors[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Rare)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Crab");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Bahari Crab");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CrabC", &Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CrabCP", &Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CrabC", &BugColors[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Spineshell Crab");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CrabU", &Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CrabUP", &Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CrabU", &BugColors[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Vampire Crab");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CrabR", &Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CrabRP", &Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CrabR", &BugColors[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Cricket");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Common Field Cricket");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CricketC", &Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Common)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CricketCP", &Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Common)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CricketC", &BugColors[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Garden Leafhopper");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CricketU", &Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CricketUP", &Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CricketU", &BugColors[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Azure Stonehopper");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CricketR", &Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##CricketRP", &Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##CricketR", &BugColors[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Rare)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Dragonfly");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Brushtail Dragonfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyC", &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Common)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyCP", &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Common)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##DragonflyC", &BugColors[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Inky Dragonfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyU", &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyUP", &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##DragonflyU", &BugColors[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Firebreathing Dragonfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyR", &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyRP", &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##DragonflyR", &BugColors[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Rare)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Jewelwing Dragonfly");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyE", &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Epic)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##DragonflyEP", &Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Epic)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##DragonflyE", &BugColors[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Epic)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Glowbug");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Paper Lantern Bug");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GlowbugC", &Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Common)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GlowbugCP", &Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Common)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##GlowbugC", &BugColors[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Bahari Glowbug");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GlowbugU", &Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##GlowbugUP", &Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##GlowbugU", &BugColors[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Uncommon)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Ladybug");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Garden Ladybug");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##LadybugC", &Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Common)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##LadybugCP", &Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Common)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##LadybugC", &BugColors[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Princess Ladybug");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##LadybugU", &Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##LadybugUP", &Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##LadybugU", &BugColors[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Uncommon)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Mantis");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Garden Mantis");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisU", &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisUP", &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MantisU", &BugColors[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Spotted Mantis");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisR", &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisRP", &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MantisR", &BugColors[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Leafstalker Mantis");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisR2", &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare2)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisR2P", &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare2)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MantisR2", &BugColors[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare2)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Fairy Mantis");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisE", &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Epic)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MantisEP", &Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Epic)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MantisE", &BugColors[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Epic)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Moth");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Kilima Night Moth");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MothC", &Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MothCP", &Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MothC", &BugColors[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Lunar Fairy Moth");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MothU", &Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MothUP", &Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MothU", &BugColors[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Gossamer Veil Moth");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MothR", &Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##MothRP", &Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##MothR", &BugColors[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)]);

                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Pede");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Garden Millipede");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PedeU", &Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PedeUP", &Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##PedeU", &BugColors[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Hairy Millipede");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PedeR", &Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PedeRP", &Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##PedeR", &BugColors[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Scintillating Centipede");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PedeR2", &Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##PedeR2P", &Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##PedeR2", &BugColors[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)]);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::Text("Snail");
                    ImGui::TableNextColumn();
                    ImGui::Text("Normal");
                    ImGui::TableNextColumn();
                    ImGui::Text("Star");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Garden Snail");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SnailU", &Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SnailUP", &Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##SnailU", &BugColors[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Stripeshell Snail");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SnailR", &Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)][0]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SnailRP", &Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)][1]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##SnailR", &BugColors[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)]);
                }
                ImGui::EndTable();
            }
            if (ImGui::CollapsingHeader("Trees")) {
                ImGui::BeginTable("Trees", 5);
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Sm", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Med", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Lg", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Text("Sm");
                    ImGui::TableNextColumn();
                    ImGui::Text("Med");
                    ImGui::TableNextColumn();
                    ImGui::Text("Lg");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Bush")) {
                        Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Bush)] = !Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Bush)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##BushSm", &Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Bush)]);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Bush", &TreeColors[static_cast<int>(ETreeType::Bush)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Sapwood")) {
                        Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Large)] =
                            Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Medium)] =
                            Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Small)] =
                            !Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SapwoodSm", &Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SapwoodMed", &Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##SapwoodLg", &Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Sapwood", &TreeColors[static_cast<int>(ETreeType::Sapwood)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Heartwood")) {
                        Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Large)] =
                            Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Medium)] =
                            Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Small)] =
                            !Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##HeartwoodSm", &Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##HeartwoodMed", &Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##HeartwoodLg", &Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Heartwood", &TreeColors[static_cast<int>(ETreeType::Heartwood)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Flow-Infused")) {
                        Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Large)] =
                            Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Medium)] =
                            Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Small)] =
                            !Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Small)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##FlowSm", &Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Small)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##FlowMed", &Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Medium)]);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##FlowLg", &Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Large)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Flow", &TreeColors[static_cast<int>(ETreeType::Flow)]);
                }
                ImGui::EndTable();
            }
            if (ImGui::CollapsingHeader("Player & Entities")) {
                ImGui::BeginTable("Odds", 3);
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);
                    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                    ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::Text("Show");
                    ImGui::TableNextColumn();
                    ImGui::Text("Color");
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Players")) {
                        Singles[static_cast<int>(EOneOffs::Player)] = !Singles[static_cast<int>(EOneOffs::Player)];
                    }
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("##Players", &Singles[static_cast<int>(EOneOffs::Player)])) {
                    }
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Players", &SingleColors[static_cast<int>(EOneOffs::Player)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("NPCs")) {
                        Singles[static_cast<int>(EOneOffs::NPC)] = !Singles[static_cast<int>(EOneOffs::NPC)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##NPC", &Singles[static_cast<int>(EOneOffs::NPC)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##NPC", &SingleColors[static_cast<int>(EOneOffs::NPC)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Fish")) {
                        Fish[static_cast<int>(EFishType::Hook)] = !Fish[static_cast<int>(EFishType::Hook)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Fish", &Fish[static_cast<int>(EFishType::Hook)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Fish", &FishColors[static_cast<int>(EFishType::Hook)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Fish Pools")) {
                        Fish[static_cast<int>(EFishType::Node)] = !Fish[static_cast<int>(EFishType::Node)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Pools", &Fish[static_cast<int>(EFishType::Node)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Pools", &FishColors[static_cast<int>(EFishType::Node)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Loot")) {
                        Singles[static_cast<int>(EOneOffs::Loot)] = !Singles[static_cast<int>(EOneOffs::Loot)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Loot", &Singles[static_cast<int>(EOneOffs::Loot)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Loot", &SingleColors[static_cast<int>(EOneOffs::Loot)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Quests")) {
                        Singles[static_cast<int>(EOneOffs::Quest)] = !Singles[static_cast<int>(EOneOffs::Quest)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Quest", &Singles[static_cast<int>(EOneOffs::Quest)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Quest", &SingleColors[static_cast<int>(EOneOffs::Quest)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Rummage Piles")) {
                        Singles[static_cast<int>(EOneOffs::RummagePiles)] = !Singles[static_cast<int>(EOneOffs::RummagePiles)];
                    }

                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##RummagePiles", &Singles[static_cast<int>(EOneOffs::RummagePiles)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##RummagePiles", &SingleColors[static_cast<int>(EOneOffs::RummagePiles)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Stables")) {
                        Singles[static_cast<int>(EOneOffs::Stables)] = !Singles[static_cast<int>(EOneOffs::Stables)];
                    }
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Stables", &Singles[static_cast<int>(EOneOffs::Stables)]);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Stables", &SingleColors[static_cast<int>(EOneOffs::Stables)]);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Others")) {
                        bVisualizeDefault = !bVisualizeDefault;
                    }
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        ImGui::SetTooltip("Shows other gatherables or creatures that were not successfully categorized.");
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##Others", &bVisualizeDefault);
                    ImGui::TableNextColumn();
                    ImGui::ColorPicker("##Others", &SingleColors[static_cast<int>(EOneOffs::Others)]);
                }
                ImGui::EndTable();
            }
        }
        // ==================================== 1 Aimbots & Fun TAB
        else if (OpenTab == 1) {
            ImGui::Columns(2, nullptr, false);

            AValeriaCharacter* ValeriaCharacter = nullptr;
            if (UWorld* World = GetWorld()) {
                if (UGameInstance* GameInstance = World->OwningGameInstance; GameInstance && GameInstance->LocalPlayers.Num() > 0) {
                    if (ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0]) {
                        if (APlayerController* PlayerController = LocalPlayer->PlayerController) {
                            if (PlayerController && PlayerController->Pawn) {
                                ValeriaCharacter = static_cast<AValeriaPlayerController*>(PlayerController)->GetValeriaCharacter();
                            }
                        }
                    }
                }
            }

            // InteliTarget Controls
            if (ImGui::CollapsingHeader("InteliTarget Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    ImGui::Checkbox("Enable Silent Aimbot", &bEnableSilentAimbot);
                    ImGui::Checkbox("Enable Legacy Aimbot", &bEnableAimbot);

                    if (bEnableAimbot) {
                        ImGui::Text("Aim Smoothing:");
                        ImGui::SliderFloat("Smoothing Factor", &SmoothingFactor, 1.0f, 100.0f, "%1.0f");
                        ImGui::Text("Aim Offset Adjustment (Drag Point):");
                        const auto canvas_size = ImVec2(200, 200); // Canvas size
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
                            ImGui::SliderFloat2("Horizontal & Vertical", reinterpret_cast<float*>(&cursor_pos), -canvas_size.x * 0.5f, canvas_size.x * 0.5f, "H: %.1f, V: %.1f");
                        }
                        ImGui::EndChild();

                        // Convert cursor_pos to AimOffset affecting Pitch and Yaw
                        AimOffset = {cursor_pos.x * scaling_factor, cursor_pos.y * scaling_factor, 0.0f};
                        ImGui::Text("Current Offset: Pitch: %.2f, Yaw: %.2f", AimOffset.X, AimOffset.Y);
                    }
                    ImGui::Checkbox("Teleport to Targeted", &bTeleportToTargeted);

                    ImGui::Checkbox("Avoid Teleporting To Targeted Players", &bAvoidTeleportingToPlayers);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        ImGui::SetTooltip("Don't teleport to players.");

                    ImGui::Checkbox("Avoid Teleporting To Targeted When Players Are Near", &bDoRadiusPlayersAvoidance);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        ImGui::SetTooltip("Don't teleport if a player is detected near your destination.");
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            ImGui::NextColumn();

            // Fun Mods - Entities column
            if (ImGui::CollapsingHeader("Fun Mods - Entities", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    static bool teleportLootDisabled = true;
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, teleportLootDisabled);
                    ImGui::Checkbox("[Disabled] Teleport Dropped Loot to Player", &bEnableLootbagTeleportation);
                    ImGui::PopItemFlag();
                    //if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Automatically teleport dropped loot to your current location.");

                    ImGui::Checkbox("Teleport To Map Waypoint", &bEnableWaypointTeleport);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        ImGui::SetTooltip("Automatically teleports you at your world map's waypoint.");
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }
        }
        // ==================================== 2 Movement & Teleport TAB
        else if (OpenTab == 2) {
            // Setting the columns layout
            ImGui::Columns(2, nullptr, false);

            //ULocalPlayer* LocalPlayer = nullptr;
            //APlayerController* PlayerController = nullptr;
            AValeriaCharacter* ValeriaCharacter = nullptr;

            UWorld* World = GetWorld();
            if (World) {
                if (UGameInstance* GameInstance = World->OwningGameInstance; GameInstance && GameInstance->LocalPlayers.Num() > 0) {
                    if (ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0]) {
                        if (APlayerController* PlayerController = LocalPlayer->PlayerController) {
                            if (PlayerController && PlayerController->Pawn) {
                                ValeriaCharacter = static_cast<AValeriaPlayerController*>(PlayerController)->GetValeriaCharacter();
                            }
                        }
                    }
                }
            }

            UValeriaCharacterMoveComponent* MovementComponent = nullptr;
            if (ValeriaCharacter) {
                MovementComponent = ValeriaCharacter->GetValeriaCharacterMovementComponent();
            }

            // Movement settings column
            if (ImGui::CollapsingHeader("Movement Settings - General", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (MovementComponent) {
                    ImGui::Text("Map: %s", CurrentMap.c_str());
                    ImGui::Spacing();
                    static const char* movementModes[] = {"Walking", "Flying", "Fly No Collision"};
                    // Dropdown menu options

                    ImGui::Checkbox("Enable Noclip", &bEnableNoclip);

                    // Create a combo box for selecting the movement mode
                    ImGui::Text("Movement Mode");
                    ImGui::SetNextItemWidth(200.0f); // Adjust the width as needed
                    if (ImGui::BeginCombo("##MovementMode", movementModes[currentMovementModeIndex])) {
                        for (int n = 0; n < IM_ARRAYSIZE(movementModes); n++) {
                            const bool isSelected = (currentMovementModeIndex == n);
                            if (ImGui::Selectable(movementModes[n], isSelected)) {
                                currentMovementModeIndex = n;
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
                        switch (currentMovementModeIndex) {
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
                    if (ImGui::InputScalar("##GlobalGameSpeed", ImGuiDataType_Float, &CustomGameSpeed, &f1, &f1000, "%.2f", ImGuiInputTextFlags_None)) {
                        static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject)->SetGlobalTimeDilation(World, CustomGameSpeed);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##GlobalGameSpeed")) {
                        CustomGameSpeed = GameSpeed;
                        static_cast<UGameplayStatics*>(UGameplayStatics::StaticClass()->DefaultObject)->SetGlobalTimeDilation(World, GameSpeed);
                    }

                    // Walk Speed
                    ImGui::Text("Walk Speed: ");
                    if (ImGui::InputScalar("##WalkSpeed", ImGuiDataType_Float, &CustomWalkSpeed, &f5)) {
                        MovementComponent->MaxWalkSpeed = CustomWalkSpeed;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##WalkSpeed")) {
                        CustomWalkSpeed = WalkSpeed;
                        MovementComponent->MaxWalkSpeed = WalkSpeed;
                    }

                    // Sprint Speed
                    ImGui::Text("Sprint Speed: ");
                    if (ImGui::InputScalar("##SprintSpeedMultiplier", ImGuiDataType_Float, &CustomSprintSpeedMultiplier, &f5)) {
                        MovementComponent->SprintSpeedMultiplier = CustomSprintSpeedMultiplier;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##SprintSpeedMultiplier")) {
                        CustomSprintSpeedMultiplier = SprintSpeedMultiplier;
                        MovementComponent->SprintSpeedMultiplier = SprintSpeedMultiplier;
                    }

                    // Climbing Speed
                    ImGui::Text("Climbing Speed: ");
                    if (ImGui::InputScalar("##ClimbingSpeed", ImGuiDataType_Float, &CustomClimbingSpeed, &f5)) {
                        MovementComponent->ClimbingSpeed = CustomClimbingSpeed;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##ClimbingSpeed")) {
                        CustomClimbingSpeed = ClimbingSpeed;
                        MovementComponent->ClimbingSpeed = ClimbingSpeed;
                    }

                    // Gliding Speed
                    ImGui::Text("Gliding Speed: ");
                    if (ImGui::InputScalar("##GlidingSpeed", ImGuiDataType_Float, &CustomGlidingSpeed, &f5)) {
                        MovementComponent->GlidingMaxSpeed = CustomGlidingSpeed;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##GlidingSpeed")) {
                        CustomGlidingSpeed = GlidingSpeed;
                        MovementComponent->GlidingMaxSpeed = GlidingSpeed;
                    }

                    // Gliding Fall Speed
                    ImGui::Text("Gliding Fall Speed: ");
                    if (ImGui::InputScalar("##GlidingFallSpeed", ImGuiDataType_Float, &CustomGlidingFallSpeed, &f5)) {
                        MovementComponent->GlidingFallSpeed = CustomGlidingFallSpeed;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##GlidingFallSpeed")) {
                        CustomGlidingFallSpeed = GlidingFallSpeed;
                        MovementComponent->GlidingFallSpeed = GlidingFallSpeed;
                    }

                    // Jump Velocity
                    ImGui::Text("Jump Velocity: ");
                    if (ImGui::InputScalar("##JumpVelocity", ImGuiDataType_Float, &CustomJumpVelocity, &f5)) {
                        MovementComponent->JumpZVelocity = CustomJumpVelocity;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##JumpVelocity")) {
                        CustomJumpVelocity = JumpVelocity;
                        MovementComponent->JumpZVelocity = JumpVelocity;
                    }

                    // Step Height
                    ImGui::Text("Step Height: ");
                    if (ImGui::InputScalar("##MaxStepHeight", ImGuiDataType_Float, &CustomMaxStepHeight, &f5)) {
                        MovementComponent->MaxStepHeight = CustomMaxStepHeight;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("R##MaxStepHeight")) {
                        CustomMaxStepHeight = MaxStepHeight;
                        MovementComponent->MaxStepHeight = MaxStepHeight;
                    }
                    if (!ValeriaCharacter) {
                        ImGui::Text("Waiting for character initialization...");
                    } else {
                        ImGui::Text("Movement component not available.");
                    }
                }
            }

            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Locations & Coordinates", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    // Locations and exploits column
                    ImGui::Text("Teleport List");
                    ImGui::Text("Double-click a location listing to teleport");
                    ImGui::ListBoxHeader("##TeleportList", ImVec2(-1, 150));
                    for (auto& [MapName, Type, Name, Location, Rotate] : TeleportLocations) {
                        if (CurrentMap == MapName || MapName == "UserDefined") {
                            if (ImGui::Selectable(Name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                                if (ImGui::IsMouseDoubleClicked(0)) {
                                    if (Type == ELocation::Global_Home) {
                                        ValeriaCharacter->GetTeleportComponent()->RpcServerTeleport_Home();
                                    } else {
                                        FHitResult HitResult;
                                        ValeriaCharacter->K2_SetActorLocation(Location, false, &HitResult, true);
                                        // NOTE: Disabled for now. (testing)
                                        //PaliaContext.PlayerController->ClientForceGarbageCollection();
                                        //PaliaContext.PlayerController->ClientFlushLevelStreaming();
                                    }
                                }
                            }
                        }
                    }
                    ImGui::ListBoxFooter();

                    // Buttons for coordinate actions
                    if (ImGui::Button("Add New Location")) {
                        ImGui::OpenPopup("Add New Location");
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
                        FHitResult HitResult;
                        ValeriaCharacter->K2_SetActorLocation(TeleportLocation, false, &HitResult, true);
                        // NOTE: Disabled, testing for now.
                        // PaliaContext.PlayerController->ClientForceGarbageCollection();
                        // PaliaContext.PlayerController->ClientFlushLevelStreaming();
                    }
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            if (ImGui::CollapsingHeader("Gatherable Items Options")) {
                if (ValeriaCharacter) {
                    ImGui::Text("Pickable List. Double-click a pickable to teleport to it.");
                    ImGui::Text("Populates from enabled Forageable ESP options.");

                    // Automatically sort by name before showing the list
                    std::ranges::sort(CachedActors, [](const FEntry& a, const FEntry& b) {
                        return a.DisplayName < b.DisplayName;
                    });

                    if (ImGui::ListBoxHeader("##PickableTeleportList", ImVec2(-1, 150))) {
                        for (auto& [Actor, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, shouldAdd] : CachedActors) {
                            if (shouldAdd && (ActorType == EType::Forage || ActorType == EType::Loot)) {
                                // Enabled ESP options only
                                if (ActorType == EType::Forage && !Forageables[Type][Quality])
                                    continue;

                                if (Actor && Actor->IsValidLowLevel() && !Actor->IsDefaultObject()) {
                                    FVector PickableLocation = Actor->K2_GetActorLocation();

                                    if (ImGui::Selectable(DisplayName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                                        if (ImGui::IsMouseDoubleClicked(0)) {
                                            PickableLocation.Z += 150;

                                            FHitResult PickableHitResult;
                                            ValeriaCharacter->K2_SetActorLocation(PickableLocation, false, &PickableHitResult, true);
                                        }
                                    }
                                }
                            }
                        }
                        ImGui::ListBoxFooter();
                    }

                    // Begin List adding Popup
                    if (ImGui::BeginPopupModal("Add New Location", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                        static int selectedWorld = 0; // 0 for Kilima, 1 for Bahari
                        static char locationName[128] = "";

                        // World selection dropdown
                        ImGui::Combo("World", &selectedWorld, "Kilima\0Bahari\0");
                        ImGui::InputText("Location Name", locationName, IM_ARRAYSIZE(locationName));

                        // Button to submit the new location
                        if (ImGui::Button("Add to List")) {
                            FVector newLocation = ValeriaCharacter->K2_GetActorLocation();
                            FRotator newRotation = ValeriaCharacter->K2_GetActorRotation();
                            std::string mapRoot = selectedWorld == 0 ? "Village_Root" : "AZ1_01_Root";
                            std::string mapName = selectedWorld == 0 ? "Kilima" : "Bahari";
                            std::string locationNameStr(locationName);

                            TeleportLocations.push_back({mapRoot, ELocation::UserDefined, mapName + " - " + locationNameStr + " [USER]", newLocation, newRotation});
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }
        }
        // ==================================== 3 Selling & Items TAB
        else if (OpenTab == 3) {
            ImGui::Columns(2, nullptr, false);

            AValeriaCharacter* ValeriaCharacter = nullptr;
            if (UWorld* World = GetWorld()) {
                if (UGameInstance* GameInstance = World->OwningGameInstance; GameInstance && GameInstance->LocalPlayers.Num() > 0) {
                    if (ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0]) {
                        if (APlayerController* PlayerController = LocalPlayer->PlayerController) {
                            if (PlayerController && PlayerController->Pawn) {
                                ValeriaCharacter = static_cast<AValeriaPlayerController*>(PlayerController)->GetValeriaCharacter();
                            }
                        }
                    }
                }
            }

            if (ImGui::CollapsingHeader("Selling Settings - Bag 1", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    ImGui::Text("Quickly Sell Items - Bag 1");
                    ImGui::Spacing();
                    ImGui::Text("Select the bag, slot, and quantity to sell.");
                    ImGui::Spacing();
                    static int selectedSlot = 0;
                    static int selectedQuantity = 1;
                    static const char* quantities[] = {"1", "10", "50", "999", "Custom"};
                    static char customQuantity[64] = "100";

                    // Slot selection dropdown
                    if (ImGui::BeginCombo("Slot", std::to_string(selectedSlot).c_str())) {
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

                    if (ImGui::Button("Sell Items")) {
                        FBagSlotLocation bag = {};
                        bag.BagIndex = 0;
                        bag.SlotIndex = selectedSlot;

                        const int quantityToSell = selectedQuantity < 4
                                                       ? atoi(quantities[selectedQuantity])
                                                       : atoi(customQuantity);

                        ValeriaCharacter->StoreComponent->RpcServer_SellItem(bag, quantityToSell);
                    }
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        ImGui::SetTooltip("Visit a storefront first, then the sell button will function.");
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Player Features", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    if (ImGui::Button("Toggle Challenge Easy Mode")) {
                        ValeriaCharacter->RpcServer_ToggleDevChallengeEasyMode();
                        bEasyModeActive = !bEasyModeActive;
                    }
                    if (bEasyModeActive) {
                        ImGui::Text("CHALLENGE EASY MODE ON");
                    } else {
                        ImGui::Text("CHALLENGE EASY MODE OFF");
                    }
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }

            if (ImGui::CollapsingHeader("Selling Hotkeys - Quickselling", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    ImGui::Text("Quicksell All - Bag 1 Slots");
                    ImGui::Text("Visit a storefront then use the hotkeys to sell your inventory quickly");
                    ImGui::Checkbox("Enable Quicksell Hotkeys", &bEnableQuicksellHotkeys);
                    ImGui::Spacing();
                    ImGui::Text("NUM1 - NUM8 | Sell All Items, Slots 1 through 8");

                    const int numpadKeys[] = {VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8};
                    if (bEnableQuicksellHotkeys) {
                        for (int i = 0; i < 8; ++i) {
                            if (IsKeyHeld(numpadKeys[i])) {
                                FBagSlotLocation quicksellBag = {};
                                quicksellBag.BagIndex = 0;
                                quicksellBag.SlotIndex = i + 1;

                                ValeriaCharacter->StoreComponent->RpcServer_SellItem(quicksellBag, 5);
                            }
                        }
                    }
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }
        }
        // ==================================== 4 Skills & Tools TAB
        else if (OpenTab == 4) {
            ImGui::Columns(2, nullptr, false);

            AValeriaCharacter* ValeriaCharacter = nullptr;
            if (UWorld* World = GetWorld()) {
                if (UGameInstance* GameInstance = World->OwningGameInstance; GameInstance && GameInstance->LocalPlayers.Num() > 0) {
                    if (ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0]) {
                        if (APlayerController* PlayerController = LocalPlayer->PlayerController) {
                            if (PlayerController && PlayerController->Pawn) {
                                ValeriaCharacter = static_cast<AValeriaPlayerController*>(PlayerController)->GetValeriaCharacter();
                            }
                        }
                    }
                }
            }

            UFishingComponent* FishingComponent = nullptr;
            auto EquippedTool = ETools::None;

            if (ImGui::CollapsingHeader("Skill Settings - General", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ValeriaCharacter) {
                    std::string EquippedName;
                    EquippedName = ValeriaCharacter->GetEquippedItem().ItemType->Name.ToString();

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

                    ImGui::Text("Equipped Tool : %s", STools[static_cast<int>(EquippedTool)]);
                    FishingComponent = ValeriaCharacter->GetFishing();
                } else {
                    ImGui::Text("Waiting for character initialization...");
                }
            }
            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Fishing Settings - General", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (FishingComponent) {
                    ImGui::Checkbox("Disable Durability Loss", &bFishingNoDurability);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        ImGui::SetTooltip("Prevents your durability from being damaged.");

                    ImGui::Checkbox("Enable Multiplayer Help", &bFishingMultiplayerHelp);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        ImGui::SetTooltip("Counts as fishing with others for weekly challenges.");

                    ImGui::Checkbox("Always Perfect Catch", &bFishingPerfectCatch);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        ImGui::SetTooltip("Choose whether to catch all fish perfectly or not.");

                    ImGui::Checkbox("Instant Catch", &bFishingInstantCatch);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        ImGui::SetTooltip("Instantly catch fish when your bobber hits the water.");

                    ImGui::Checkbox("Sell All Fish", &bFishingSell);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        ImGui::SetTooltip("Visit the fishing store for this feature to work.");

                    ImGui::Checkbox("Discard All Junk", &bFishingDiscard);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        ImGui::SetTooltip("Discards Junk from your inventory to free-up space.");

                    ImGui::Checkbox("Open & Store Makeshift Decor", &bFishingOpenStoreWaterlogged);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        ImGui::SetTooltip("Opens the waterlogged chests and sends the decor back home.");

                    ImGui::Checkbox("Capture Fishing Pool", &bCaptureFishingSpot);
                    ImGui::Checkbox("Override Fishing Pool", &bOverrideFishingSpot);
                    ImGui::SameLine();
                    ImGui::Text("[Captured: %s]", sOverrideFishingSpot.ToString().c_str());

                    ImGui::Spacing();

                    if (EquippedTool != ETools::FishingRod) {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);
                        bEnableAutoFishing = false;
                    }

                    ImGui::Checkbox("Auto Fast Fishing", &bEnableAutoFishing);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                        ImGui::SetTooltip("Cast your fishing rod to start.");

                    if (EquippedTool != ETools::FishingRod) {
                        ImGui::Spacing();
                        ImGui::Text("[Equip your fishing rod to enable Auto Fast Fishing]");
                    }

                    gDetourManager.ToggleFishingDelays(bEnableAutoFishing);
                } else {
                    if (!ValeriaCharacter) {
                        ImGui::Text("Waiting for character initialization...");
                    } else {
                        ImGui::Text("Fishing component not available.");
                    }
                }
            }
        }
        // ==================================== 5 Housing & Decorating TAB
        else if (OpenTab == 5) {
            ImGui::Columns(1, nullptr, false);

            AValeriaCharacter* ValeriaCharacter = nullptr;
            if (UWorld* World = GetWorld()) {
                if (UGameInstance* GameInstance = World->OwningGameInstance; GameInstance && GameInstance->LocalPlayers.Num() > 0) {
                    if (ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0]) {
                        if (APlayerController* PlayerController = LocalPlayer->PlayerController) {
                            if (PlayerController && PlayerController->Pawn) {
                                ValeriaCharacter = static_cast<AValeriaPlayerController*>(PlayerController)->GetValeriaCharacter();
                            }
                        }
                    }
                }
            }

            if (ValeriaCharacter) {
                //UPlacementComponent* PlacementComponent = ValeriaCharacter->GetPlacement();

                if (ValeriaCharacter->GetPlacement()) {
                    ImGui::Checkbox("Place Items Anywhere", &bPlaceAnywhere);
                } else {
                    ImGui::Text("No Placement Component available.");
                }
            } else {
                ImGui::Text("Waiting for character initialization...");
            }
        }
    }

    ImGui::End();

    if (!show)
        ShowOverlay(false);
}

void PaliaOverlay::ProcessActors(int step) {
    std::erase_if(CachedActors, [this, step](const FEntry& Entry) {
        return static_cast<int>(Entry.ActorType) == step;
    });

    auto World = GetWorld();
    if (!World)
        return;

    const auto ActorType = static_cast<EType>(step);
    std::vector<AActor*> Actors;
    std::vector<UClass*> SearchClasses;

    // What are gates anyways?
    STATIC_CLASS("BP_Stables_FrontGate_01_C", SearchClasses)
    STATIC_CLASS("BP_Stables_FrontGate_02_C", SearchClasses)

    switch (ActorType) {
    case EType::Tree:
        if (AnyTrue2D(Trees)) {
            STATIC_CLASS("BP_ValeriaGatherableLoot_Lumber_C", SearchClasses)
        }
        break;
    case EType::Ore:
        if (AnyTrue2D(Ores)) {
            STATIC_CLASS("BP_ValeriaGatherableLoot_Mining_Base_C", SearchClasses)
        }
        break;
    case EType::Bug:
        if (AnyTrue3D(Bugs)) {
            STATIC_CLASS("BP_ValeriaBugCatchingCreature_C", SearchClasses)
        }
        break;
    case EType::Animal:
        if (AnyTrue2D(Animals)) {
            STATIC_CLASS("BP_ValeriaHuntingCreature_C", SearchClasses)
        }
        break;
    case EType::Forage:
        if (AnyTrue2D(Forageables)) {
            STATIC_CLASS("BP_Valeria_Gatherable_Placed_C", SearchClasses)
        }
        break;
    case EType::Loot:
        if (Singles[static_cast<int>(EOneOffs::Loot)] || bEnableLootbagTeleportation) {
            STATIC_CLASS("BP_Loot_C", SearchClasses)
        }
        break;
    case EType::Players:
        if (Singles[static_cast<int>(EOneOffs::Player)]) {
            SearchClasses.push_back(AValeriaCharacter::StaticClass());
        }
        break;
    case EType::NPCs:
        if (Singles[static_cast<int>(EOneOffs::NPC)]) {
            SearchClasses.push_back(AValeriaVillagerCharacter::StaticClass());
        }
        break;
    case EType::Quest:
        if (Singles[static_cast<int>(EOneOffs::Quest)]) {
            STATIC_CLASS("BP_SimpleInspect_Base_C", SearchClasses)
            STATIC_CLASS("BP_QuestInspect_Base_C", SearchClasses)
            STATIC_CLASS("BP_QuestItem_BASE_C", SearchClasses)
        }
        break;
    case EType::RummagePiles:
        if (Singles[static_cast<int>(EOneOffs::RummagePiles)]) {
            STATIC_CLASS("BP_BeachPile_C", SearchClasses)
            STATIC_CLASS("BP_ChapaaPile_C", SearchClasses)
        }
        break;
    case EType::Stables:
        if (Singles[static_cast<int>(EOneOffs::Stables)]) {
            STATIC_CLASS("BP_Stables_Sign_C", SearchClasses)
        }
        break;
    case EType::Fish:
        if (AnyTrue(Fish)) {
            STATIC_CLASS("BP_WaterPlane_Fishing_Base_SQ_C", SearchClasses)
            STATIC_CLASS("BP_Minigame_Fish_C", SearchClasses)
        }
        break;
    default:
        break;
    }

    if (!SearchClasses.empty()) {
        if (ActorType == EType::RummagePiles || ActorType == EType::Stables) {
            Actors = FindAllActorsOfTypes(World, SearchClasses);
        } else {
            Actors = FindActorsOfTypes(World, SearchClasses);
        }
    }

    for (AActor* Actor : Actors) {
        auto ClassName = Actor->Class->GetName();

        // [HACK] Gates-Begone
        if (ClassName.find("_FrontGate_") != std::string::npos) {
            // Destroy and move on, no caching.
            Actor->K2_DestroyActor();
            continue;
        }

        if (!Actor || !Actor->IsValidLowLevel() || Actor->IsDefaultObject())
            continue;

        const FVector ActorPosition = Actor->K2_GetActorLocation();
        if (ActorPosition.IsZero() || ActorPosition == FVector{2, 0, -9900})
            continue;

        int Type = 0;
        int Quality = 0;
        int Variant = 0;

        bool shouldAdd = false;

        switch (ActorType) {
        case EType::Tree: {
            if (auto Tree = GetFlagSingle(ClassName, TREE_TYPE_MAPPINGS); Tree != ETreeType::Unknown) {
                if (auto Size = GetFlagSingle(ClassName, GATHERABLE_SIZE_MAPPINGS); Size != EGatherableSize::Unknown) {
                    shouldAdd = true;
                    Type = static_cast<int>(Tree);
                    Variant = static_cast<int>(Size);
                }
            }
            break;
        }
        case EType::Ore: {
            if (auto OreType = GetFlagSingle(ClassName, MINING_TYPE_MAPPINGS); OreType != EOreType::Unknown) {
                auto Size = GetFlagSingle(ClassName, GATHERABLE_SIZE_MAPPINGS);
                if (OreType == EOreType::Clay)
                    Size = EGatherableSize::Large;
                if (Size != EGatherableSize::Unknown) {
                    shouldAdd = true;
                    Type = static_cast<int>(OreType);
                    Variant = static_cast<int>(Size);
                }
            }
            break;
        }
        case EType::Bug: {
            if (auto BugType = GetFlagSingle(ClassName, CREATURE_BUGKIND_MAPPINGS); BugType != EBugKind::Unknown) {
                if (auto BVar = GetFlagSingleEnd(ClassName, CREATURE_BUGQUALITY_MAPPINGS); BVar != EBugQuality::Unknown) {
                    shouldAdd = true;
                    Type = static_cast<int>(BugType);
                    Variant = static_cast<int>(BVar);
                    if (ClassName.ends_with("+_C")) {
                        Quality = 1;
                    }
                }
            }
            break;
        }
        case EType::Animal: {
            if (auto CKType = GetFlagSingle(ClassName, CREATURE_KIND_MAPPINGS); CKType != ECreatureKind::Unknown) {
                if (auto CQType = GetFlagSingleEnd(ClassName, CREATURE_KINDQUALITY_MAPPINGS); CQType != ECreatureQuality::Unknown) {
                    shouldAdd = true;
                    Type = static_cast<int>(CKType);
                    Variant = static_cast<int>(CQType);
                }
            }
            break;
        }
        case EType::Forage: {
            if (!Actor->bActorEnableCollision)
                continue;

            if (auto ForageType = GetFlagSingle(ClassName, FORAGEABLE_TYPE_MAPPINGS); ForageType != EForageableType::Unknown) {
                shouldAdd = true;
                Type = static_cast<int>(ForageType);
                if (ClassName.ends_with("+_C")) {
                    Quality = 1;
                }
            }
            break;
        }
        case EType::Loot: {
            shouldAdd = true;
            Type = 1; // doesn't matter, but isn't "unknown"
            break;
        }
        case EType::Players: {
            shouldAdd = true;
            Type = 1; // doesn't matter, but isn't "unknown"
            const auto VActor = static_cast<AValeriaCharacter*>(Actor);
            ClassName = VActor->CharacterName.ToString();
            break;
        }
        case EType::NPCs: {
            shouldAdd = true;
            Type = 1; // doesn't matter, but isn't "unknown"
            break;
        }
        case EType::Quest: {
            if (!Actor->bActorEnableCollision)
                continue;

            shouldAdd = true;
            Type = 1;
            break;
        }
        case EType::RummagePiles: {
            shouldAdd = true;
            Type = 1;
            break;
        }
        case EType::Stables: {
            shouldAdd = true;
            Type = 1;
            break;
        }
        case EType::Fish: {
            if (auto FishType = GetFlagSingle(ClassName, FISH_TYPE_MAPPINGS); FishType != EFishType::Unknown) {
                shouldAdd = true;
                Type = static_cast<int>(FishType);
            }
            break;
        }
        default:
            break;
        }

        if (!shouldAdd && !bVisualizeDefault)
            continue;

        const std::string Name = CLASS_NAME_ALIAS.contains(ClassName) ? CLASS_NAME_ALIAS[ClassName] : ClassName;
        CachedActors.push_back({Actor, ActorPosition, Name, ActorType, Type, Quality, Variant, shouldAdd});
    }
}
