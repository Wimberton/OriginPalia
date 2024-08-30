#pragma once

#include "Core/Configuration.h"
#include "Detours/Main/HUDDetours.h"
#include "Misc/ImGuiExt.h"

#include "imgui.h"

namespace ESP_SingleSettings {
    inline void Show() {
        if (ImGui::CollapsingHeader("Player & Entities##PlayerEntitiesSettingHeader")) {
            if (ImGui::Button("Toggle All##MiscBtn")) {
                bool newState = !HUDDetours::Singles[static_cast<int>(EOneOffs::Player)];

                HUDDetours::Singles[static_cast<int>(EOneOffs::Player)] = newState;
                HUDDetours::Singles[static_cast<int>(EOneOffs::NPC)] = newState;
                HUDDetours::Singles[static_cast<int>(EOneOffs::FishHook)] = newState;
                HUDDetours::Singles[static_cast<int>(EOneOffs::FishPool)] = newState;
                HUDDetours::Singles[static_cast<int>(EOneOffs::Loot)] = newState;
                HUDDetours::Singles[static_cast<int>(EOneOffs::Quest)] = newState;
                HUDDetours::Singles[static_cast<int>(EOneOffs::RummagePiles)] = newState;
                HUDDetours::Singles[static_cast<int>(EOneOffs::Treasure)] = newState;
                HUDDetours::Singles[static_cast<int>(EOneOffs::Stables)] = newState;
                HUDDetours::Singles[static_cast<int>(EOneOffs::TimedDrop)] = newState;
                HUDDetours::Singles[static_cast<int>(EOneOffs::Relic)] = newState;
                HUDDetours::Singles[static_cast<int>(EOneOffs::Others)] = newState;

                Configuration::Save(ESaveFile::ESPSettings);
            }

            ImGui::BeginTable("Odds", 3);
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 0);
                ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);
                IMGUI_ADD_HEADERS("Type", "Show", "Color")

                IMGUI_ADD_ROW(
                    "Players",
                    IMGUI_CHECKBOX("##Players", &HUDDetours::Singles[static_cast<int>(EOneOffs::Player)]),
                    IMGUI_COLORPICKER("##Players", &Configuration::SingleColors.find(EOneOffs::Player)->second)
                )
                IMGUI_ADD_ROW(
                    "NPCs",
                    IMGUI_CHECKBOX("##NPC", &HUDDetours::Singles[static_cast<int>(EOneOffs::NPC)]),
                    IMGUI_COLORPICKER("##NPC", &Configuration::SingleColors.find(EOneOffs::NPC)->second)
                )
                IMGUI_ADD_ROW(
                    "Fish",
                    IMGUI_CHECKBOX("##FishHook", &HUDDetours::Singles[static_cast<int>(EOneOffs::FishHook)]),
                    IMGUI_COLORPICKER("##FishHook", &Configuration::SingleColors.find(EOneOffs::FishHook)->second)
                )
                IMGUI_ADD_ROW(
                    "Fish Pools",
                    IMGUI_CHECKBOX("##FishPool", &HUDDetours::Singles[static_cast<int>(EOneOffs::FishPool)]),
                    IMGUI_COLORPICKER("##FishPool", &Configuration::SingleColors.find(EOneOffs::FishPool)->second)
                )
                IMGUI_ADD_ROW(
                    "Loot",
                    IMGUI_CHECKBOX("##Loot", &HUDDetours::Singles[static_cast<int>(EOneOffs::Loot)]),
                    IMGUI_COLORPICKER("##Loot", &Configuration::SingleColors.find(EOneOffs::Loot)->second)
                )
                IMGUI_ADD_ROW(
                    "Quests",
                    IMGUI_CHECKBOX("##Quest", &HUDDetours::Singles[static_cast<int>(EOneOffs::Quest)]),
                    IMGUI_COLORPICKER("##Quest", &Configuration::SingleColors.find(EOneOffs::Quest)->second)
                )
                IMGUI_ADD_ROW(
                    "Rummage Piles",
                    IMGUI_CHECKBOX("##RummagePiles", &HUDDetours::Singles[static_cast<int>(EOneOffs::RummagePiles)]),
                    IMGUI_COLORPICKER("##RummagePiles", &Configuration::SingleColors.find(EOneOffs::RummagePiles)->second)
                )
                IMGUI_ADD_ROW(
                    "Treasure",
                    IMGUI_CHECKBOX("##Treasure", &HUDDetours::Singles[static_cast<int>(EOneOffs::Treasure)]),
                    IMGUI_COLORPICKER("##Treasure", &Configuration::SingleColors.find(EOneOffs::Treasure)->second)
                )
                IMGUI_ADD_ROW(
                    "Stables",
                    IMGUI_CHECKBOX("##Stables", &HUDDetours::Singles[static_cast<int>(EOneOffs::Stables)]),
                    IMGUI_COLORPICKER("##Stables", &Configuration::SingleColors.find(EOneOffs::Stables)->second)
                )
                IMGUI_ADD_ROW(
                    "Timed Drops",
                    IMGUI_CHECKBOX("##TimedDrops", &HUDDetours::Singles[static_cast<int>(EOneOffs::TimedDrop)]),
                    IMGUI_COLORPICKER("##TimedDrops", &Configuration::SingleColors.find(EOneOffs::TimedDrop)->second)
                )
                IMGUI_ADD_ROW(
                    "Books & Lost Relics",
                    IMGUI_CHECKBOX("##LostRelics", &HUDDetours::Singles[static_cast<int>(EOneOffs::Relic)]),
                    IMGUI_COLORPICKER("##LostRelics", &Configuration::SingleColors.find(EOneOffs::Relic)->second)
                )
                IMGUI_ADD_ROW(
                    "Others",
                    IMGUI_CHECKBOX("##Others", &HUDDetours::Singles[static_cast<int>(EOneOffs::Others)]),
                    IMGUI_COLORPICKER("##Others", &Configuration::SingleColors.find(EOneOffs::Others)->second)
                )
            }
            ImGui::EndTable();
        }
    }
}
