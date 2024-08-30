#pragma once

#include "Core/Configuration.h"
#include "Misc/ImGuiExt.h"

#include "imgui.h"

namespace Store_AutoSellSettings {
    inline void Show() {
        ImGui::BeginTable("AutoSellTableRarity", 6);
        {
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None, 0);
            ImGui::TableSetupColumn("Common", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("Uncommon", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("Rare", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("Epic", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("SQ", ImGuiTableColumnFlags_WidthFixed, 60);

            IMGUI_ADD_HEADERS("Type", "Common", "Uncommon", "Rare", "Epic", "SQ")

            IMGUI_ADD_BLANK_ROW(
                IMGUI_CHECKBOX("Bugs", &Configuration::bBugSell, true, "Automatically sell bugs from your inventory.")
                IMGUI_CHECKBOX("##RarityBugCommon", &Configuration::bBugSellRarity[static_cast<int>(EItemRarity::Common)], true, "Automatically sell common bugs.")
                IMGUI_CHECKBOX("##RarityBugUncommon", &Configuration::bBugSellRarity[static_cast<int>(EItemRarity::Uncommon)], true, "Automatically sell uncommon bugs.")
                IMGUI_CHECKBOX("##RarityBugRare", &Configuration::bBugSellRarity[static_cast<int>(EItemRarity::Rare)], true, "Automatically sell rare bugs.")
                IMGUI_CHECKBOX("##RarityBugEpic", &Configuration::bBugSellRarity[static_cast<int>(EItemRarity::Epic)], true, "Automatically sell epic bugs.")
                IMGUI_CHECKBOX("##RarityBugSQ", &Configuration::bBugSellSQ, "Automatically sell star quality bugs.")
            )

            IMGUI_ADD_BLANK_ROW(
                IMGUI_CHECKBOX("Fish", &Configuration::bFishingSell, true, "Automatically sell fish from your inventory.")
                IMGUI_CHECKBOX("##RarityFishCommon", &Configuration::bFishingSellRarity[static_cast<int>(EItemRarity::Common)], true, "Automatically sell common fish.")
                IMGUI_CHECKBOX("##RarityFishUncommon", &Configuration::bFishingSellRarity[static_cast<int>(EItemRarity::Uncommon)], true, "Automatically sell uncommon fish.")
                IMGUI_CHECKBOX("##RarityFishRare", &Configuration::bFishingSellRarity[static_cast<int>(EItemRarity::Rare)], true, "Automatically sell rare fish.")
                IMGUI_CHECKBOX("##RarityFishEpic", &Configuration::bFishingSellRarity[static_cast<int>(EItemRarity::Epic)], true, "Automatically sell epic fish.")
                IMGUI_CHECKBOX("##RarityFishSQ", &Configuration::bFishingSellSQ, "Automatically sell star quality fish.")
            )

            IMGUI_ADD_BLANK_ROW(
                IMGUI_CHECKBOX("Forageables", &Configuration::bForageableSell, true, "Automatically sell forageables from your inventory.")
                IMGUI_CHECKBOX("##RarityForageableCommon", &Configuration::bForageableSellRarity[static_cast<int>(EItemRarity::Common)], true, "Automatically sell common forageables.")
                IMGUI_CHECKBOX("##RarityForageableUncommon", &Configuration::bForageableSellRarity[static_cast<int>(EItemRarity::Uncommon)], true, "Automatically sell uncommon forageables.")
                IMGUI_CHECKBOX("##RarityForageableRare", &Configuration::bForageableSellRarity[static_cast<int>(EItemRarity::Rare)], true, "Automatically sell rare forageables.")
                IMGUI_CHECKBOX("##RarityForageableEpic", &Configuration::bForageableSellRarity[static_cast<int>(EItemRarity::Epic)], true, "Automatically sell epic forageables.")
                IMGUI_CHECKBOX("##RarityForageableSQ", &Configuration::bForageableSellSQ, "Automatically sell star quality forageables.")
            )

            IMGUI_ADD_BLANK_ROW(
                IMGUI_CHECKBOX("Hunting", &Configuration::bHuntingSell, true, "Automatically sell hunting loot from your inventory.")
                IMGUI_CHECKBOX("##RarityHuntingCommon", &Configuration::bHuntingSellRarity[static_cast<int>(EItemRarity::Common)], true, "Automatically sell common hunting loot.")
                IMGUI_CHECKBOX("##RarityHuntingUncommon", &Configuration::bHuntingSellRarity[static_cast<int>(EItemRarity::Uncommon)], true, "Automatically sell uncommon hunting loot.")
                IMGUI_CHECKBOX("##RarityHuntingRare", &Configuration::bHuntingSellRarity[static_cast<int>(EItemRarity::Rare)], true, "Automatically sell rare hunting loot.")
                IMGUI_CHECKBOX("##RarityHuntingEpic", &Configuration::bHuntingSellRarity[static_cast<int>(EItemRarity::Epic)], true, "Automatically sell epic hunting loot.")
                IMGUI_CHECKBOX("##RarityHuntingSQ", &Configuration::bHuntingSellSQ, "Automatically sell star quality hunting loot.")
            )
        }
        ImGui::EndTable();

        ImGui::BeginTable("AutoSellTableTrees", 5);
        {
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None, 0);
            ImGui::TableSetupColumn("Fiber", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("Sapwood", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("Heartwood", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("Flow", ImGuiTableColumnFlags_WidthFixed, 60);

            IMGUI_ADD_HEADERS("", "Fiber", "Sapwood", "Heartwood", "Flow")

            IMGUI_ADD_BLANK_ROW(
                IMGUI_CHECKBOX("Trees", &Configuration::bTreeSell, true, "Automatically sell logs from your inventory.")
                IMGUI_CHECKBOX("##TreeFiber", &Configuration::bTreeSellRarity[0], true, "Automatically sell plant fiber.")
                IMGUI_CHECKBOX("##TreeSap", &Configuration::bTreeSellRarity[1], true, "Automatically sell sapwood logs.")
                IMGUI_CHECKBOX("##TreeHeart", &Configuration::bTreeSellRarity[2], true, "Automatically sell heartwood logs.")
                IMGUI_CHECKBOX("##TreeFlow", &Configuration::bTreeSellRarity[3], "Automatically sell flow-infused logs.")
            )
        }
        ImGui::EndTable();

        IMGUI_CHECKBOX("Flowers", &Configuration::bFlowerSell, "Automatically sell flowers from your inventory.")
    }
}
