#pragma once

#include "Core/Configuration.h"
#include "Detours/Main/HUDDetours.h"
#include "Misc/ImGuiExt.h"

#include "imgui.h"

namespace ESP_AnimalSettings {
    inline void Show() {
        if (ImGui::CollapsingHeader("Animals##AnimalsSettingsHeader")) {
            if (ImGui::Button("Sernuk##SernukBtn")) {
                HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)];
                HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier2)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier2)];
                HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier3)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier3)];
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Chapaa##ChapaaBtn")) {
                HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)];
                HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier2)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier2)];
                HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier3)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier3)];
                HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Chase)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Chase)];
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Muujin##MuujinBtn")) {
                HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier1)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier1)];
                HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier2)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier2)];
                HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier3)] = !HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier3)];
                Configuration::Save(ESaveFile::ESPSettings);
            }

            ImGui::BeginTable("Animals", 3);
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_None, 0);
                ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);

                // SERNUKS
                IMGUI_ADD_HEADERS("", "Show", "Color")
                IMGUI_ADD_ROW(
                    "Sernuk",
                    IMGUI_CHECKBOX("##Sernuk", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier1)]),
                    IMGUI_COLORPICKER("##SernukColor", &Configuration::AnimalColors.find(FCreatureType{ECreatureKind::Cearnuk, ECreatureQuality::Tier1})->second)
                )
                IMGUI_ADD_ROW(
                    "Elder Sernuk",
                    IMGUI_CHECKBOX("##ElderSernuk", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier2)]),
                    IMGUI_COLORPICKER("##ElderSernukColor", &Configuration::AnimalColors.find(FCreatureType{ECreatureKind::Cearnuk, ECreatureQuality::Tier2})->second)
                )
                IMGUI_ADD_ROW(
                    "Proudhorn Sernuk",
                    IMGUI_CHECKBOX("##ProudhornSernuk", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Cearnuk)][static_cast<int>(ECreatureQuality::Tier3)]),
                    IMGUI_COLORPICKER("##ProudhornSernukColor", &Configuration::AnimalColors.find(FCreatureType{ECreatureKind::Cearnuk, ECreatureQuality::Tier3})->second)
                )

                // CHAPAAS
                IMGUI_ADD_HEADERS("", "Show", "Color")
                IMGUI_ADD_ROW(
                    "Chapaa",
                    IMGUI_CHECKBOX("##Chapaa", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier1)]),
                    IMGUI_COLORPICKER("##ChapaaColor", &Configuration::AnimalColors.find(FCreatureType{ECreatureKind::Chapaa, ECreatureQuality::Tier1})->second)
                )
                IMGUI_ADD_ROW(
                    "Striped Chapaa",
                    IMGUI_CHECKBOX("##StripedChapaa", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier2)]),
                    IMGUI_COLORPICKER("##StripedChapaaColor", &Configuration::AnimalColors.find(FCreatureType{ECreatureKind::Chapaa, ECreatureQuality::Tier2})->second)
                )
                IMGUI_ADD_ROW(
                    "Azure Chapaa",
                    IMGUI_CHECKBOX("##AzureChapaa", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Tier3)]),
                    IMGUI_COLORPICKER("##AzureChapaaColor", &Configuration::AnimalColors.find(FCreatureType{ECreatureKind::Chapaa, ECreatureQuality::Tier3})->second)
                )
                IMGUI_ADD_ROW(
                    "Minigame Chapaa",
                    IMGUI_CHECKBOX("##MinigameChapaa", &HUDDetours::Animals[static_cast<int>(ECreatureKind::Chapaa)][static_cast<int>(ECreatureQuality::Chase)]),
                    IMGUI_COLORPICKER("##MinigameChapaaColor", &Configuration::AnimalColors.find(FCreatureType{ECreatureKind::Chapaa, ECreatureQuality::Chase})->second)
                )

                // MUUJINS
                IMGUI_ADD_HEADERS("", "Show", "Color")
                IMGUI_ADD_ROW(
                    "Muujin",
                    IMGUI_CHECKBOX("##Muujin", &HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier1)]),
                    IMGUI_COLORPICKER("##MuujinColor", &Configuration::AnimalColors.find(FCreatureType{ECreatureKind::TreeClimber, ECreatureQuality::Tier1})->second)
                )
                IMGUI_ADD_ROW(
                    "Banded Muujin",
                    IMGUI_CHECKBOX("##BandedMuujin", &HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier2)]),
                    IMGUI_COLORPICKER("##BandedMuujinColor", &Configuration::AnimalColors.find(FCreatureType{ECreatureKind::TreeClimber, ECreatureQuality::Tier2})->second)
                )
                IMGUI_ADD_ROW(
                    "Bluebristle Muujin",
                    IMGUI_CHECKBOX("##BluebristleMuujin", &HUDDetours::Animals[static_cast<int>(ECreatureKind::TreeClimber)][static_cast<int>(ECreatureQuality::Tier3)]),
                    IMGUI_COLORPICKER("##BluebristleMuujinColor", &Configuration::AnimalColors.find(FCreatureType{ECreatureKind::TreeClimber, ECreatureQuality::Tier3})->second)
                )
            }
            ImGui::EndTable();
        }
    }
}
