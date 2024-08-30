#pragma once

#include "Core/Configuration.h"
#include "Detours/Main/HUDDetours.h"
#include "Misc/ImGuiExt.h"

#include "imgui.h"

namespace ESP_ForageableSettings {
    inline void Show() {
        if (ImGui::CollapsingHeader("Forageables##ForageablesSettingsHeader")) {
            if (ImGui::Button("Common##Forage")) {
                for (int pos : HUDDetours::ForageableCommon) {
                    HUDDetours::Forageables[pos][1] = HUDDetours::Forageables[pos][0] = !HUDDetours::Forageables[pos][0];
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Uncommon##Forage")) {
                for (int pos : HUDDetours::ForageableUncommon) {
                    HUDDetours::Forageables[pos][1] = HUDDetours::Forageables[pos][0] = !HUDDetours::Forageables[pos][0];
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Rare##Forage")) {
                for (int pos : HUDDetours::ForageableRare) {
                    HUDDetours::Forageables[pos][1] = HUDDetours::Forageables[pos][0] = !HUDDetours::Forageables[pos][0];
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Epic##Forage")) {
                for (int pos : HUDDetours::ForageableEpic) {
                    HUDDetours::Forageables[pos][1] = HUDDetours::Forageables[pos][0] = !HUDDetours::Forageables[pos][0];
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Star##Forage")) {
                for (auto& Forageable : HUDDetours::Forageables) {
                    Forageable[1] = !Forageable[1];
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }

            ImGui::BeginTable("Forageables", 4);
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 0);
                ImGui::TableSetupColumn("Normal", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Star", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);

                // + BEACH
                IMGUI_ADD_HEADERS("Beach", "Normal", "", "Color")
                // Coral
                IMGUI_ADD_ROW(
                    "Coral",
                    IMGUI_CHECKBOX("##Coral", &HUDDetours::Forageables[static_cast<int>(EForageableType::Coral)][0], true),
                    IMGUI_COLORPICKER("##CoralColor", &Configuration::ForageableColors.find(EForageableType::Coral)->second)
                )
                // Oyster
                IMGUI_ADD_ROW(
                    "Oyster",
                    IMGUI_CHECKBOX("##Oyster", &HUDDetours::Forageables[static_cast<int>(EForageableType::Oyster)][0], true),
                    IMGUI_COLORPICKER("##OysterColor", &Configuration::ForageableColors.find(EForageableType::Oyster)->second)
                )
                // Shell
                IMGUI_ADD_ROW(
                    "Shell",
                    IMGUI_CHECKBOX("##Shell", &HUDDetours::Forageables[static_cast<int>(EForageableType::Shell)][0], true),
                    IMGUI_COLORPICKER("##ShellColor", &Configuration::ForageableColors.find(EForageableType::Shell)->second)
                )

                // + FLOWER
                IMGUI_ADD_HEADERS("Flower", "Normal", "Star", "Color")
                // Briar Daisy
                IMGUI_ADD_ROW(
                    "Briar Daisy",
                    IMGUI_CHECKBOX("##PoisonFlower", &HUDDetours::Forageables[static_cast<int>(EForageableType::PoisonFlower)][0], true)
                    IMGUI_CHECKBOX("##PoisonFlowerC", &HUDDetours::Forageables[static_cast<int>(EForageableType::PoisonFlower)][1]),
                    IMGUI_COLORPICKER("##PoisonFlower", &Configuration::ForageableColors.find(EForageableType::PoisonFlower)->second)
                )
                // Crystal Lake Lotus
                IMGUI_ADD_ROW(
                    "Crystal Lake Lotus",
                    IMGUI_CHECKBOX("##WaterFlower", &HUDDetours::Forageables[static_cast<int>(EForageableType::WaterFlower)][0], true)
                    IMGUI_CHECKBOX("##WaterFlowerC", &HUDDetours::Forageables[static_cast<int>(EForageableType::WaterFlower)][1]),
                    IMGUI_COLORPICKER("##WaterFlower", &Configuration::ForageableColors.find(EForageableType::WaterFlower)->second)
                )
                // Heartdrop Lily
                IMGUI_ADD_ROW(
                    "Heartdrop Lily",
                    IMGUI_CHECKBOX("##Heartdrop", &HUDDetours::Forageables[static_cast<int>(EForageableType::Heartdrop)][0], true)
                    IMGUI_CHECKBOX("##HeartdropC", &HUDDetours::Forageables[static_cast<int>(EForageableType::Heartdrop)][1]),
                    IMGUI_COLORPICKER("##Heartdrop", &Configuration::ForageableColors.find(EForageableType::Heartdrop)->second)
                )
                // Sundrop Lily
                IMGUI_ADD_ROW(
                    "Sundrop Lily",
                    IMGUI_CHECKBOX("##Sundrop", &HUDDetours::Forageables[static_cast<int>(EForageableType::Sundrop)][0], true)
                    IMGUI_CHECKBOX("##SundropC", &HUDDetours::Forageables[static_cast<int>(EForageableType::Sundrop)][1]),
                    IMGUI_COLORPICKER("##Sundrop", &Configuration::ForageableColors.find(EForageableType::Sundrop)->second)
                )

                // + MOSS
                IMGUI_ADD_HEADERS("Moss", "Normal", "Star", "Color")
                // Dragon's Beard Peat
                IMGUI_ADD_ROW(
                    "Dragon's Beard Peat",
                    IMGUI_CHECKBOX("##DragonsBeard", &HUDDetours::Forageables[static_cast<int>(EForageableType::DragonsBeard)][0], true)
                    IMGUI_CHECKBOX("##DragonsBeardC", &HUDDetours::Forageables[static_cast<int>(EForageableType::DragonsBeard)][1]),
                    IMGUI_COLORPICKER("##DragonsBeard", &Configuration::ForageableColors.find(EForageableType::DragonsBeard)->second)
                )
                // Emerald Carpet Moss
                IMGUI_ADD_ROW(
                    "Emerald Carpet Moss",
                    IMGUI_CHECKBOX("##EmeraldCarpet", &HUDDetours::Forageables[static_cast<int>(EForageableType::EmeraldCarpet)][0], true)
                    IMGUI_CHECKBOX("##EmeraldCarpetC", &HUDDetours::Forageables[static_cast<int>(EForageableType::EmeraldCarpet)][1]),
                    IMGUI_COLORPICKER("##EmeraldCarpet", &Configuration::ForageableColors.find(EForageableType::EmeraldCarpet)->second)
                )

                // + MUSHROOM
                IMGUI_ADD_HEADERS("Mushroom", "Normal", "Star", "Color")
                // Brightshroom
                IMGUI_ADD_ROW(
                    "Brightshroom",
                    IMGUI_CHECKBOX("##MushroomBlue", &HUDDetours::Forageables[static_cast<int>(EForageableType::MushroomBlue)][0], true)
                    IMGUI_CHECKBOX("##MushroomBlueC", &HUDDetours::Forageables[static_cast<int>(EForageableType::MushroomBlue)][1]),
                    IMGUI_COLORPICKER("##MushroomBlue", &Configuration::ForageableColors.find(EForageableType::MushroomBlue)->second)
                )
                // Mountain Morel
                IMGUI_ADD_ROW(
                    "Mountain Morel",
                    IMGUI_CHECKBOX("##MushroomRed", &HUDDetours::Forageables[static_cast<int>(EForageableType::MushroomRed)][0], true)
                    IMGUI_CHECKBOX("##MushroomRedC", &HUDDetours::Forageables[static_cast<int>(EForageableType::MushroomRed)][1]),
                    IMGUI_COLORPICKER("##MushroomRed", &Configuration::ForageableColors.find(EForageableType::MushroomRed)->second)
                )

                // + SPICE
                IMGUI_ADD_HEADERS("Spice", "Normal", "Star", "Color")
                // Dari Cloves
                IMGUI_ADD_ROW(
                    "Dari Cloves",
                    IMGUI_CHECKBOX("##DariCloves", &HUDDetours::Forageables[static_cast<int>(EForageableType::DariCloves)][0], true)
                    IMGUI_CHECKBOX("##DariClovesC", &HUDDetours::Forageables[static_cast<int>(EForageableType::DariCloves)][1]),
                    IMGUI_COLORPICKER("##DariCloves", &Configuration::ForageableColors.find(EForageableType::DariCloves)->second)
                )
                // Heat Root
                IMGUI_ADD_ROW(
                    "Heat Root",
                    IMGUI_CHECKBOX("##HeatRoot", &HUDDetours::Forageables[static_cast<int>(EForageableType::HeatRoot)][0], true)
                    IMGUI_CHECKBOX("##HeatRootC", &HUDDetours::Forageables[static_cast<int>(EForageableType::HeatRoot)][1]),
                    IMGUI_COLORPICKER("##HeatRoot", &Configuration::ForageableColors.find(EForageableType::HeatRoot)->second)
                )
                // Spice Sprouts
                IMGUI_ADD_ROW(
                    "Spice Sprouts",
                    IMGUI_CHECKBOX("##SpicedSprouts", &HUDDetours::Forageables[static_cast<int>(EForageableType::SpicedSprouts)][0], true)
                    IMGUI_CHECKBOX("##SpicedSproutsC", &HUDDetours::Forageables[static_cast<int>(EForageableType::SpicedSprouts)][1]),
                    IMGUI_COLORPICKER("##SpicedSprouts", &Configuration::ForageableColors.find(EForageableType::SpicedSprouts)->second)
                )
                // Sweet Leaf
                IMGUI_ADD_ROW(
                    "Sweet Leaf",
                    IMGUI_CHECKBOX("##SweetLeaves", &HUDDetours::Forageables[static_cast<int>(EForageableType::SweetLeaves)][0], true)
                    IMGUI_CHECKBOX("##SweetLeavesC", &HUDDetours::Forageables[static_cast<int>(EForageableType::SweetLeaves)][1]),
                    IMGUI_COLORPICKER("##SweetLeaves", &Configuration::ForageableColors.find(EForageableType::SweetLeaves)->second)
                )

                // + VEGETABLE
                IMGUI_ADD_HEADERS("Vegetable", "Normal", "Star", "Color")
                // Wild Garlic
                IMGUI_ADD_ROW(
                    "Wild Garlic",
                    IMGUI_CHECKBOX("##Garlic", &HUDDetours::Forageables[static_cast<int>(EForageableType::Garlic)][0], true)
                    IMGUI_CHECKBOX("##GarlicC", &HUDDetours::Forageables[static_cast<int>(EForageableType::Garlic)][1]),
                    IMGUI_COLORPICKER("##Garlic", &Configuration::ForageableColors.find(EForageableType::Garlic)->second)
                )
                // Wild Ginger
                IMGUI_ADD_ROW(
                    "Wild Ginger",
                    IMGUI_CHECKBOX("##Ginger", &HUDDetours::Forageables[static_cast<int>(EForageableType::Ginger)][0], true)
                    IMGUI_CHECKBOX("##GingerC", &HUDDetours::Forageables[static_cast<int>(EForageableType::Ginger)][1]),
                    IMGUI_COLORPICKER("##Ginger", &Configuration::ForageableColors.find(EForageableType::Ginger)->second)
                )
                // Wild Green Onion
                IMGUI_ADD_ROW(
                    "Wild Green Onion",
                    IMGUI_CHECKBOX("##GreenOnion", &HUDDetours::Forageables[static_cast<int>(EForageableType::GreenOnion)][0], true)
                    IMGUI_CHECKBOX("##GreenOnionC", &HUDDetours::Forageables[static_cast<int>(EForageableType::GreenOnion)][1]),
                    IMGUI_COLORPICKER("##GreenOnion", &Configuration::ForageableColors.find(EForageableType::GreenOnion)->second)
                )
            }
            ImGui::EndTable();
        }
    }
}
