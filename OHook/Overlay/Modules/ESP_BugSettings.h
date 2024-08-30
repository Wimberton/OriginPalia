#pragma once

#include "Core/Configuration.h"
#include "Detours/Main/HUDDetours.h"
#include "Misc/ImGuiExt.h"

#include "imgui.h"

namespace ESP_BugSettings {
    inline void Show() {
        if (ImGui::CollapsingHeader("Bugs##BugsSettingsHeader")) {
            if (ImGui::Button("Common##Bugs")) {
                for (auto& Bug : HUDDetours::Bugs) {
                    Bug[static_cast<int>(EBugQuality::Common)][1] = Bug[static_cast<int>(EBugQuality::Common)][0] = !Bug[static_cast<int>(EBugQuality::Common)][0];
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Uncommon##Bugs")) {
                for (auto& Bug : HUDDetours::Bugs) {
                    Bug[static_cast<int>(EBugQuality::Uncommon)][1] = Bug[static_cast<int>(EBugQuality::Uncommon)][0] = !Bug[static_cast<int>(EBugQuality::Uncommon)][0];
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Rare##Bugs")) {
                for (auto& Bug : HUDDetours::Bugs) {
                    Bug[static_cast<int>(EBugQuality::Rare)][1] = Bug[static_cast<int>(EBugQuality::Rare)][0] = !Bug[static_cast<int>(EBugQuality::Rare)][0];
                    Bug[static_cast<int>(EBugQuality::Rare2)][1] = Bug[static_cast<int>(EBugQuality::Rare2)][0] = !Bug[static_cast<int>(EBugQuality::Rare2)][0];
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Epic##Bugs")) {
                for (auto& Bug : HUDDetours::Bugs) {
                    Bug[static_cast<int>(EBugQuality::Epic)][1] = Bug[static_cast<int>(EBugQuality::Epic)][0] = !Bug[static_cast<int>(EBugQuality::Epic)][0];
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }
            ImGui::SameLine();
            if (ImGui::Button("Star##Bugs")) {
                for (auto& Bug : HUDDetours::Bugs) {
                    for (auto& j : Bug) {
                        j[1] = !j[1];
                    }
                }
                Configuration::Save(ESaveFile::ESPSettings);
            }

            ImGui::BeginTable("Bugs", 4);
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 0);
                ImGui::TableSetupColumn("Normal", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Star", ImGuiTableColumnFlags_WidthFixed, 40);
                ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 40);

                // + BEE
                IMGUI_ADD_HEADERS("Bee", "Normal", "Star", "Color")
                // Bahari Bee
                IMGUI_ADD_ROW(
                    "Bahari Bee",
                    IMGUI_CHECKBOX("##BahariBee", &HUDDetours::Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                    IMGUI_CHECKBOX("##BahariBeeC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Uncommon)][1]),
                    IMGUI_COLORPICKER("##BahariBee", &Configuration::BugColors.find(FBugType{EBugKind::Bee, EBugQuality::Uncommon})->second)
                )
                // Golden Glory Bee
                IMGUI_ADD_ROW(
                    "Golden Glory Bee",
                    IMGUI_CHECKBOX("##GoldenGloryBee", &HUDDetours::Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)][0], true)
                    IMGUI_CHECKBOX("##GoldenGloryBeeC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Bee)][static_cast<int>(EBugQuality::Rare)][1]),
                    IMGUI_COLORPICKER("##GoldenGloryBee", &Configuration::BugColors.find(FBugType{EBugKind::Bee, EBugQuality::Rare})->second)
                )

                // + BEETLE
                IMGUI_ADD_HEADERS("Beetle", "Normal", "Star", "Color")
                // Spotted Stink Bug
                IMGUI_ADD_ROW(
                    "Spotted Stink Bug",
                    IMGUI_CHECKBOX("##SpottedStinkBug", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Common)][0], true)
                    IMGUI_CHECKBOX("##SpottedStinkBugC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Common)][1]),
                    IMGUI_COLORPICKER("##SpottedStinkBug", &Configuration::BugColors.find(FBugType{EBugKind::Beetle, EBugQuality::Common})->second)
                )
                // Proudhorned Stag Beetle
                IMGUI_ADD_ROW(
                    "Proudhorned Stag Beetle",
                    IMGUI_CHECKBOX("##ProudhornedStag", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                    IMGUI_CHECKBOX("##ProudhornedStagC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Uncommon)][1]),
                    IMGUI_COLORPICKER("##ProudhornedStag", &Configuration::BugColors.find(FBugType{EBugKind::Beetle, EBugQuality::Uncommon})->second)
                )
                // Raspberry Beetle
                IMGUI_ADD_ROW(
                    "Raspberry Beetle",
                    IMGUI_CHECKBOX("##Raspberry", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Rare)][0], true)
                    IMGUI_CHECKBOX("##RaspberryC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Rare)][1]),
                    IMGUI_COLORPICKER("##Raspberry", &Configuration::BugColors.find(FBugType{EBugKind::Beetle, EBugQuality::Rare})->second)
                )
                // Ancient Amber Beetle
                IMGUI_ADD_ROW(
                    "Ancient Amber Beetle",
                    IMGUI_CHECKBOX("##AncientAmber", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Epic)][0], true)
                    IMGUI_CHECKBOX("##AncientAmberC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Beetle)][static_cast<int>(EBugQuality::Epic)][1]),
                    IMGUI_COLORPICKER("##AncientAmber", &Configuration::BugColors.find(FBugType{EBugKind::Beetle, EBugQuality::Epic})->second)
                )

                // + BUTTERFLY
                IMGUI_ADD_HEADERS("Butterfly", "Normal", "Star", "Color")
                // Common Blue Butterfly
                IMGUI_ADD_ROW(
                    "Common Blue Butterfly",
                    IMGUI_CHECKBOX("##CommonBlue", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Common)][0], true)
                    IMGUI_CHECKBOX("##CommonBlueC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Common)][1]),
                    IMGUI_COLORPICKER("##CommonBlue", &Configuration::BugColors.find(FBugType{EBugKind::Butterfly, EBugQuality::Common})->second)
                )
                // Duskwing Butterfly
                IMGUI_ADD_ROW(
                    "Duskwing Butterfly",
                    IMGUI_CHECKBOX("##Duskwing", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                    IMGUI_CHECKBOX("##DuskwingC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Uncommon)][1]),
                    IMGUI_COLORPICKER("##Duskwing", &Configuration::BugColors.find(FBugType{EBugKind::Butterfly, EBugQuality::Uncommon})->second)
                )
                // Brighteye Butterfly
                IMGUI_ADD_ROW(
                    "Brighteye Butterfly",
                    IMGUI_CHECKBOX("##Brighteye", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Rare)][0], true)
                    IMGUI_CHECKBOX("##BrighteyeC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Rare)][1]),
                    IMGUI_COLORPICKER("##Brighteye", &Configuration::BugColors.find(FBugType{EBugKind::Butterfly, EBugQuality::Rare})->second)
                )
                // Rainbow-Tipped Butterfly
                IMGUI_ADD_ROW(
                    "Rainbow-Tipped Butterfly",
                    IMGUI_CHECKBOX("##Rainbow", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Epic)][0], true)
                    IMGUI_CHECKBOX("##RainbowC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Butterfly)][static_cast<int>(EBugQuality::Epic)][1]),
                    IMGUI_COLORPICKER("##Rainbow", &Configuration::BugColors.find(FBugType{EBugKind::Butterfly, EBugQuality::Epic})->second)
                )

                // + CICADA
                IMGUI_ADD_HEADERS("Cicada", "Normal", "Star", "Color")
                // Common Bark Cicada
                IMGUI_ADD_ROW(
                    "Common Bark Cicada",
                    IMGUI_CHECKBOX("##CommonBark", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Common)][0], true)
                    IMGUI_CHECKBOX("##CommonBarkC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Common)][1]),
                    IMGUI_COLORPICKER("##CommonBark", &Configuration::BugColors.find(FBugType{EBugKind::Cicada, EBugQuality::Common})->second)
                )
                // Cerulean Cicada
                IMGUI_ADD_ROW(
                    "Cerulean Cicada",
                    IMGUI_CHECKBOX("##Cerulean", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                    IMGUI_CHECKBOX("##CeruleanC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Uncommon)][1]),
                    IMGUI_COLORPICKER("##Cerulean", &Configuration::BugColors.find(FBugType{EBugKind::Cicada, EBugQuality::Uncommon})->second)
                )
                // Spitfire Cicada
                IMGUI_ADD_ROW(
                    "Spitfire Cicada",
                    IMGUI_CHECKBOX("##Spitfire", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Rare)][0], true)
                    IMGUI_CHECKBOX("##SpitfireC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cicada)][static_cast<int>(EBugQuality::Rare)][1]),
                    IMGUI_COLORPICKER("##Spitfire", &Configuration::BugColors.find(FBugType{EBugKind::Cicada, EBugQuality::Rare})->second)
                )

                // + CRAB
                IMGUI_ADD_HEADERS("Crab", "Normal", "Star", "Color")
                // Bahari Crab
                IMGUI_ADD_ROW(
                    "Bahari Crab",
                    IMGUI_CHECKBOX("##BahariCrab", &HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)][0], true)
                    IMGUI_CHECKBOX("##BahariCrabC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Common)][1]),
                    IMGUI_COLORPICKER("##BahariCrab", &Configuration::BugColors.find(FBugType{EBugKind::Crab, EBugQuality::Common})->second)
                )
                // Spineshell Crab
                IMGUI_ADD_ROW(
                    "Spineshell Crab",
                    IMGUI_CHECKBOX("##SpineshellCrab", &HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                    IMGUI_CHECKBOX("##SpineshellCrabC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Uncommon)][1]),
                    IMGUI_COLORPICKER("##SpineshellCrab", &Configuration::BugColors.find(FBugType{EBugKind::Crab, EBugQuality::Uncommon})->second)
                )
                // Vampire Crab
                IMGUI_ADD_ROW(
                    "Vampire Crab",
                    IMGUI_CHECKBOX("##VampireCrab", &HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)][0], true)
                    IMGUI_CHECKBOX("##VampireCrabC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Crab)][static_cast<int>(EBugQuality::Rare)][1]),
                    IMGUI_COLORPICKER("##VampireCrab", &Configuration::BugColors.find(FBugType{EBugKind::Crab, EBugQuality::Rare})->second)
                )

                // + CRICKET
                IMGUI_ADD_HEADERS("Cricket", "Normal", "Star", "Color")
                // Common Field Cricket
                IMGUI_ADD_ROW(
                    "Common Field Cricket",
                    IMGUI_CHECKBOX("##CommonField", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Common)][0], true)
                    IMGUI_CHECKBOX("##CommonFieldC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Common)][1]),
                    IMGUI_COLORPICKER("##CommonField", &Configuration::BugColors.find(FBugType{EBugKind::Cricket, EBugQuality::Common})->second)
                )
                // Garden Leafhopper
                IMGUI_ADD_ROW(
                    "Garden Leafhopper",
                    IMGUI_CHECKBOX("##Leafhopper", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                    IMGUI_CHECKBOX("##LeafhopperC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Uncommon)][1]),
                    IMGUI_COLORPICKER("##Leafhopper", &Configuration::BugColors.find(FBugType{EBugKind::Cricket, EBugQuality::Uncommon})->second)
                )
                // Azure Stonehopper
                IMGUI_ADD_ROW(
                    "Azure Stonehopper",
                    IMGUI_CHECKBOX("##Stonehopper", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Rare)][0], true)
                    IMGUI_CHECKBOX("##StonehopperC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Cricket)][static_cast<int>(EBugQuality::Rare)][1]),
                    IMGUI_COLORPICKER("##Stonehopper", &Configuration::BugColors.find(FBugType{EBugKind::Cricket, EBugQuality::Rare})->second)
                )

                // + DRAGONFLY
                IMGUI_ADD_HEADERS("Dragonfly", "Normal", "Star", "Color")
                // Brushtail Dragonfly
                IMGUI_ADD_ROW(
                    "Brushtail Dragonfly",
                    IMGUI_CHECKBOX("##Brushtail", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Common)][0], true)
                    IMGUI_CHECKBOX("##BrushtailC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Common)][1]),
                    IMGUI_COLORPICKER("##Brushtail", &Configuration::BugColors.find(FBugType{EBugKind::Dragonfly, EBugQuality::Common})->second)
                )
                // Inky Dragonfly
                IMGUI_ADD_ROW(
                    "Inky Dragonfly",
                    IMGUI_CHECKBOX("##Inky", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                    IMGUI_CHECKBOX("##InkyC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Uncommon)][1]),
                    IMGUI_COLORPICKER("##Inky", &Configuration::BugColors.find(FBugType{EBugKind::Dragonfly, EBugQuality::Uncommon})->second)
                )
                // Firebreathing Dragonfly
                IMGUI_ADD_ROW(
                    "Firebreathing Dragonfly",
                    IMGUI_CHECKBOX("##Firebreathing", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Rare)][0], true)
                    IMGUI_CHECKBOX("##FirebreathingC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Rare)][1]),
                    IMGUI_COLORPICKER("##Firebreathing", &Configuration::BugColors.find(FBugType{EBugKind::Dragonfly, EBugQuality::Rare})->second)
                )
                // Jewelwing Dragonfly
                IMGUI_ADD_ROW(
                    "Jewelwing Dragonfly",
                    IMGUI_CHECKBOX("##Jewelwing", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Epic)][0], true)
                    IMGUI_CHECKBOX("##JewelwingC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Dragonfly)][static_cast<int>(EBugQuality::Epic)][1]),
                    IMGUI_COLORPICKER("##Jewelwing", &Configuration::BugColors.find(FBugType{EBugKind::Dragonfly, EBugQuality::Epic})->second)
                )

                // + GLOWBUG
                IMGUI_ADD_HEADERS("Glowbug", "Normal", "Star", "Color")
                // Paper Lantern Bug
                IMGUI_ADD_ROW(
                    "Paper Lantern Bug",
                    IMGUI_CHECKBOX("##PaperLantern", &HUDDetours::Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Common)][0], true)
                    IMGUI_CHECKBOX("##PaperLanternC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Common)][1]),
                    IMGUI_COLORPICKER("##PaperLantern", &Configuration::BugColors.find(FBugType{EBugKind::Glowbug, EBugQuality::Common})->second)
                )
                // Bahari Glowbug
                IMGUI_ADD_ROW(
                    "Bahari Glowbug",
                    IMGUI_CHECKBOX("##BahariGlowbug", &HUDDetours::Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Rare)][0], true)
                    IMGUI_CHECKBOX("##BahariGlowbugC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Glowbug)][static_cast<int>(EBugQuality::Rare)][1]),
                    IMGUI_COLORPICKER("##BahariGlowbug", &Configuration::BugColors.find(FBugType{EBugKind::Glowbug, EBugQuality::Rare})->second)
                )

                // + LADYBUG
                IMGUI_ADD_HEADERS("Ladybug", "Normal", "Star", "Color")
                // Garden Ladybug
                IMGUI_ADD_ROW(
                    "Garden Ladybug",
                    IMGUI_CHECKBOX("##GardenLadybug", &HUDDetours::Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                    IMGUI_CHECKBOX("##GardenLadybugC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Uncommon)][1]),
                    IMGUI_COLORPICKER("##GardenLadybug", &Configuration::BugColors.find(FBugType{EBugKind::Ladybug, EBugQuality::Uncommon})->second)
                )
                // Princess Ladybug 
                IMGUI_ADD_ROW(
                    "Princess Ladybug",
                    IMGUI_CHECKBOX("##PrincessLadybug", &HUDDetours::Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Rare)][0], true)
                    IMGUI_CHECKBOX("##PrincessLadybugC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Ladybug)][static_cast<int>(EBugQuality::Rare)][1]),
                    IMGUI_COLORPICKER("##PrincessLadybug", &Configuration::BugColors.find(FBugType{EBugKind::Ladybug, EBugQuality::Rare})->second)
                )

                // + MANTIS
                IMGUI_ADD_HEADERS("Mantis", "Normal", "Star", "Color")
                // Garden Mantis U
                IMGUI_ADD_ROW(
                    "Garden Mantis",
                    IMGUI_CHECKBOX("##GardenMantis", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                    IMGUI_CHECKBOX("##GardenMantisC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Uncommon)][1]),
                    IMGUI_COLORPICKER("##GardenMantis", &Configuration::BugColors.find(FBugType{EBugKind::Mantis, EBugQuality::Uncommon})->second)
                )
                // Spotted Mantis R
                IMGUI_ADD_ROW(
                    "Spotted Mantis",
                    IMGUI_CHECKBOX("##SpottedMantis", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare)][0], true)
                    IMGUI_CHECKBOX("##SpottedMantisC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare)][1]),
                    IMGUI_COLORPICKER("##SpottedMantis", &Configuration::BugColors.find(FBugType{EBugKind::Mantis, EBugQuality::Rare})->second)
                )
                // Leafstalker Mantis R2
                IMGUI_ADD_ROW(
                    "Leafstalker Mantis",
                    IMGUI_CHECKBOX("##LeafstalkerMantis", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare2)][0], true)
                    IMGUI_CHECKBOX("##LeafstalkerMantisC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Rare2)][1]),
                    IMGUI_COLORPICKER("##LeafstalkerMantis", &Configuration::BugColors.find(FBugType{EBugKind::Mantis, EBugQuality::Rare2})->second)
                )
                // Fairy Mantis E
                IMGUI_ADD_ROW(
                    "Fairy Mantis",
                    IMGUI_CHECKBOX("##FairyMantis", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Epic)][0], true)
                    IMGUI_CHECKBOX("##FairyMantisC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Mantis)][static_cast<int>(EBugQuality::Epic)][1]),
                    IMGUI_COLORPICKER("##FairyMantis", &Configuration::BugColors.find(FBugType{EBugKind::Mantis, EBugQuality::Epic})->second)
                )

                // + MOTH
                IMGUI_ADD_HEADERS("Moth", "Normal", "Star", "Color")
                // Kilima Night Moth
                IMGUI_ADD_ROW(
                    "Kilima Night Moth",
                    IMGUI_CHECKBOX("##KilimaNightMoth", &HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)][0], true)
                    IMGUI_CHECKBOX("##KilimaNightMothC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Common)][1]),
                    IMGUI_COLORPICKER("##KilimaNightMoth", &Configuration::BugColors.find(FBugType{EBugKind::Moth, EBugQuality::Common})->second)
                )
                // Lunar Fairy Moth
                IMGUI_ADD_ROW(
                    "Lunar Fairy Moth",
                    IMGUI_CHECKBOX("##LunarFairyMoth", &HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                    IMGUI_CHECKBOX("##LunarFairyMothC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Uncommon)][1]),
                    IMGUI_COLORPICKER("##LunarFairyMoth", &Configuration::BugColors.find(FBugType{EBugKind::Moth, EBugQuality::Uncommon})->second)
                )
                // Gossamer Veil Moth
                IMGUI_ADD_ROW(
                    "Gossamer Veil Moth",
                    IMGUI_CHECKBOX("##GossamerVeilMoth", &HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)][0], true)
                    IMGUI_CHECKBOX("##GossamerVeilMothC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Moth)][static_cast<int>(EBugQuality::Rare)][1]),
                    IMGUI_COLORPICKER("##GossamerVeilMoth", &Configuration::BugColors.find(FBugType{EBugKind::Moth, EBugQuality::Rare})->second)
                )

                // + Pede
                IMGUI_ADD_HEADERS("Pede", "Normal", "Star", "Color")
                // Garden Millipede
                IMGUI_ADD_ROW(
                    "Garden Millipede",
                    IMGUI_CHECKBOX("##GardenMillipede", &HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                    IMGUI_CHECKBOX("##GardenMillipedeC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Uncommon)][1]),
                    IMGUI_COLORPICKER("##GardenMillipede", &Configuration::BugColors.find(FBugType{EBugKind::Pede, EBugQuality::Uncommon})->second)
                )
                // Hairy Millipede
                IMGUI_ADD_ROW(
                    "Hairy Millipede",
                    IMGUI_CHECKBOX("##HairyMillipede", &HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)][0], true)
                    IMGUI_CHECKBOX("##HairyMillipedeC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare)][1]),
                    IMGUI_COLORPICKER("##HairyMillipede", &Configuration::BugColors.find(FBugType{EBugKind::Pede, EBugQuality::Rare})->second)
                )
                // Scintillating Centipede
                IMGUI_ADD_ROW(
                    "Scintillating Centipede",
                    IMGUI_CHECKBOX("##ScintillatingCentipede", &HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)][0], true)
                    IMGUI_CHECKBOX("##ScintillatingCentipedeC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Pede)][static_cast<int>(EBugQuality::Rare2)][1]),
                    IMGUI_COLORPICKER("##ScintillatingCentipede", &Configuration::BugColors.find(FBugType{EBugKind::Pede, EBugQuality::Rare2})->second)
                )

                // + SNAIL
                IMGUI_ADD_HEADERS("Snail", "Normal", "Star", "Color")
                // Garden Snail U
                IMGUI_ADD_ROW(
                    "Garden Snail",
                    IMGUI_CHECKBOX("##GardenSnail", &HUDDetours::Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)][0], true)
                    IMGUI_CHECKBOX("##GardenSnailC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Uncommon)][1]),
                    IMGUI_COLORPICKER("##GardenSnail", &Configuration::BugColors.find(FBugType{EBugKind::Snail, EBugQuality::Uncommon})->second)
                )
                // Stripeshell Snail R
                IMGUI_ADD_ROW(
                    "Stripeshell Snail",
                    IMGUI_CHECKBOX("##StripeshellSnail", &HUDDetours::Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)][0], true)
                    IMGUI_CHECKBOX("##StripeshellSnailC", &HUDDetours::Bugs[static_cast<int>(EBugKind::Snail)][static_cast<int>(EBugQuality::Rare)][1]),
                    IMGUI_COLORPICKER("##StripeshellSnail", &Configuration::BugColors.find(FBugType{EBugKind::Snail, EBugQuality::Rare})->second)
                )
            }
            ImGui::EndTable();
        }
    }
}
