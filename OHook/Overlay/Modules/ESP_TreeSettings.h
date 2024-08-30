#pragma once

#include "Core/Configuration.h"
#include "Detours/Main/HUDDetours.h"
#include "Misc/ImGuiExt.h"

#include "imgui.h"

namespace ESP_TreeSettings {
    inline void Show() {
        if (ImGui::CollapsingHeader("Trees##TreesSettingHeader")) {
            if (ImGui::Button("Bush##BushBtn")) {
                bool newState = !HUDDetours::Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Sapling)];
                HUDDetours::Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Sapling)] = newState;
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Sapwood##SapwoodBtn")) {
                for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                    HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(size.first)] =
                        !HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(size.first)];
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Heartwood##HeartwoodBtn")) {
                for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                    HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(size.first)] =
                        !HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(size.first)];
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Flow##FlowBtn")) {
                for (const auto& size : GATHERABLE_SIZE_MAPPINGS) {
                    HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(size.first)] =
                        !HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(size.first)];
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }

            ImGui::BeginTable("Trees", 6);
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 0);
                ImGui::TableSetupColumn("Sap", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Sm", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Med", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Lg", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);

                IMGUI_ADD_HEADERS("", "Sap","Sm", "Med", "Lg", "Color")

                // Bush
                IMGUI_ADD_ROW(
                    "Bush",
                    IMGUI_CHECKBOX("##BushSap", &HUDDetours::Trees[static_cast<int>(ETreeType::Bush)][static_cast<int>(EGatherableSize::Sapling)]),
                    IMGUI_TABLENEXTCOLUMN() // these are to separate the ColorPicker 3 more columns.
                    IMGUI_TABLENEXTCOLUMN()
                    IMGUI_TABLENEXTCOLUMN()
                    IMGUI_COLORPICKER("##Bush", &Configuration::TreeColors.find(ETreeType::Bush)->second)
                )
                // Sapwood
                IMGUI_ADD_ROW(
                    "Sapwood",
                    IMGUI_CHECKBOX("##SapwoodSap", &HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Sapling)], true)
                    IMGUI_CHECKBOX("##SapwoodSm", &HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Small)], true)
                    IMGUI_CHECKBOX("##SapwoodMed", &HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Medium)], true)
                    IMGUI_CHECKBOX("##SapwoodLg", &HUDDetours::Trees[static_cast<int>(ETreeType::Sapwood)][static_cast<int>(EGatherableSize::Large)]),
                    IMGUI_COLORPICKER("##Sapwood", &Configuration::TreeColors.find(ETreeType::Sapwood)->second)
                )
                // Heartwood
                IMGUI_ADD_ROW(
                    "Heartwood",
                    IMGUI_CHECKBOX("##HeartwoodSap", &HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Sapling)], true)
                    IMGUI_CHECKBOX("##HeartwoodSm", &HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Small)], true)
                    IMGUI_CHECKBOX("##HeartwoodMed", &HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Medium)], true)
                    IMGUI_CHECKBOX("##HeartwoodLg", &HUDDetours::Trees[static_cast<int>(ETreeType::Heartwood)][static_cast<int>(EGatherableSize::Large)]),
                    IMGUI_COLORPICKER("##Heartwood", &Configuration::TreeColors.find(ETreeType::Heartwood)->second)
                )
                // Flow-Infused
                IMGUI_ADD_ROW(
                    "Flow-Infused",
                    IMGUI_CHECKBOX("##FlowSap", &HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Sapling)], true)
                    IMGUI_CHECKBOX("##FlowSm", &HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Small)], true)
                    IMGUI_CHECKBOX("##FlowMed", &HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Medium)], true)
                    IMGUI_CHECKBOX("##FlowLg", &HUDDetours::Trees[static_cast<int>(ETreeType::Flow)][static_cast<int>(EGatherableSize::Large)]),
                    IMGUI_COLORPICKER("##Flow", &Configuration::TreeColors.find(ETreeType::Flow)->second)
                )
            }
            ImGui::EndTable();
        }
    }
}
