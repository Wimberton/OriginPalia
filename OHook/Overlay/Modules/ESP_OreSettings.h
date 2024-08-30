#pragma once

#include "Core/Configuration.h"
#include "Detours/Main/HUDDetours.h"
#include "Misc/ImGuiExt.h"

#include "imgui.h"

namespace ESP_OreSettings {
    inline void Show() {
        if (ImGui::CollapsingHeader("Ores##OresSettingsHeader")) {
            if (ImGui::Button("Clay##ClayBtn")) {
                bool newState = !HUDDetours::Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(EGatherableSize::Large)];
                for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                    HUDDetours::Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(size.first)] = newState;
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Stone##StoneBtn")) {
                bool newState = !HUDDetours::Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Large)];
                for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                    HUDDetours::Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(size.first)] = newState;
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Copper##CopperBtn")) {
                bool newState = !HUDDetours::Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Large)];
                for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                    HUDDetours::Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(size.first)] = newState;
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Iron##IronBtn")) {
                bool newState = !HUDDetours::Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Large)];
                for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                    HUDDetours::Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(size.first)] = newState;
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Palium##PaliumBtn")) {
                bool newState = !HUDDetours::Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Large)];
                for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                    HUDDetours::Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(size.first)] = newState;
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }

            ImGui::BeginTable("Ores", 5);
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 0);
                ImGui::TableSetupColumn("Sm", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Med", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Lg", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);

                IMGUI_ADD_HEADERS("", "Sm", "Med", "Lg", "Color")

                // CLAY
                IMGUI_ADD_ROW(
                    "Clay",
                    IMGUI_TABLENEXTCOLUMN()
                    IMGUI_TABLENEXTCOLUMN()
                    IMGUI_CHECKBOX("##ClayLg", &HUDDetours::Ores[static_cast<int>(EOreType::Clay)][static_cast<int>(EGatherableSize::Large)]),
                    IMGUI_COLORPICKER("##Clay", &Configuration::OreColors.find(EOreType::Clay)->second)
                )
                //Stone
                IMGUI_ADD_ROW(
                    "Stone",
                    IMGUI_CHECKBOX("##StoneSm", &HUDDetours::Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Small)], true)
                    IMGUI_CHECKBOX("##StoneMed", &HUDDetours::Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Medium)], true)
                    IMGUI_CHECKBOX("##StoneLg", &HUDDetours::Ores[static_cast<int>(EOreType::Stone)][static_cast<int>(EGatherableSize::Large)]),
                    IMGUI_COLORPICKER("##Stone", &Configuration::OreColors.find(EOreType::Stone)->second)
                )
                //Copper
                IMGUI_ADD_ROW(
                    "Copper",
                    IMGUI_CHECKBOX("##CopperSm", &HUDDetours::Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Small)], true)
                    IMGUI_CHECKBOX("##CopperMed", &HUDDetours::Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Medium)], true)
                    IMGUI_CHECKBOX("##CopperLg", &HUDDetours::Ores[static_cast<int>(EOreType::Copper)][static_cast<int>(EGatherableSize::Large)]),
                    IMGUI_COLORPICKER("##Copper", &Configuration::OreColors.find(EOreType::Copper)->second)
                )
                //Iron
                IMGUI_ADD_ROW(
                    "Iron",
                    IMGUI_CHECKBOX("##IronSm", &HUDDetours::Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Small)], true)
                    IMGUI_CHECKBOX("##IronMed", &HUDDetours::Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Medium)], true)
                    IMGUI_CHECKBOX("##IronLg", &HUDDetours::Ores[static_cast<int>(EOreType::Iron)][static_cast<int>(EGatherableSize::Large)]),
                    IMGUI_COLORPICKER("##Iron", &Configuration::OreColors.find(EOreType::Iron)->second)
                )
                //Palium
                IMGUI_ADD_ROW(
                    "Palium",
                    IMGUI_CHECKBOX("##PaliumSm", &HUDDetours::Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Small)], true)
                    IMGUI_CHECKBOX("##PaliumMed", &HUDDetours::Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Medium)], true)
                    IMGUI_CHECKBOX("##PaliumLg", &HUDDetours::Ores[static_cast<int>(EOreType::Palium)][static_cast<int>(EGatherableSize::Large)]),
                    IMGUI_COLORPICKER("##Palium", &Configuration::OreColors.find(EOreType::Palium)->second)
                )
            }
            ImGui::EndTable();
        }
    }
}
