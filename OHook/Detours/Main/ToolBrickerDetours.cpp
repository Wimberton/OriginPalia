#include "Detours/Main/ToolBrickerDetours.h"
#include "Detours/Main/WidgetDetours.h"
#include "Core/Configuration.h"
#include "Core/DetourManager.h"

#include "SDK/WBP_RepairStation_v3_CM_parameters.hpp"
#include "SDK/WBP_CheckmarkButton_CM_parameters.hpp"
#include "SDK/WBP_Template_Button_Primary_02_parameters.hpp"

void ToolBrickerDetours::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
    DetourManager::AddInstance(VC);
	DetourManager::AddFunctionListener("Function Engine.Actor.ReceiveTick", &Func_DoToolBricker, nullptr);
    DetourManager::AddInstance(VC->GetItemUpgradeComponent());
    DetourManager::AddFunctionListener("Function Palia.ItemUpgradeComponent.RpcServer_PurchaseRepair", &Func_PurchaseRepair, nullptr);
}

void ToolBrickerDetours::Func_DoToolBricker(const UObject* Context, UFunction* Function, void* Parms) {
    if (!RepairStation) return;
    SetupRepairStation();

    // Set gold button text to default in case we've changed it
    SetGoldButton(RepairStation->Button_GoldRepair, L"Repair", true);

    // Get Player Gold
    int32 PlayerGold = -1;
    if (!RepairStation_GetPlayerGoldAmount) {
        RepairStation_GetPlayerGoldAmount = RepairStation->Class->GetFunction("WBP_RepairStation_v3_CM_C", "GetPlayerGoldAmount");
    }
    if (RepairStation_GetPlayerGoldAmount) {
        Params::WBP_RepairStation_v3_CM_C_GetPlayerGoldAmount Parms{};
        RepairStation->ProcessEvent(RepairStation_GetPlayerGoldAmount, &Parms);
        PlayerGold = Parms.GoldAmount;
    }

    if (PlayerGold < 0) return; // Weren't able to get the player's gold
    if (PlayerGold >= MaxGold) { // No need to brick a tool
        RepairAmount = 0;
        return;
    }

    // Check for Repair All
    if (RepairStation->Checkmark_RepairAll_CM->WBP_Checkmark_CM->Checked) {
        if (!Checkmark_SetText) {
            Checkmark_SetText = RepairStation->Checkmark_RepairAll_CM->Class->GetFunction("WBP_CheckmarkButton_CM_C", "SetText");
        }
        if (Checkmark_SetText) {
            Params::WBP_CheckmarkButton_CM_C_SetText Parms{};
            if (Configuration::bEnableToolBricker) {
                Parms.Text = UKismetTextLibrary::Conv_StringToText(L"Repair All (Uncheck for 'Unrepair')");
            }
            else {
                Parms.Text = UKismetTextLibrary::Conv_StringToText(L"Repair All Tools");
            }
            RepairStation->Checkmark_RepairAll_CM->ProcessEvent(Checkmark_SetText, &Parms);
        }
        RepairAmount = 0;
        return;
    }

    // Get Selected Tool Durability
    int32 Durability = 0;
    if (!RepairStation_GetToolDurability) {
        RepairStation_GetToolDurability = RepairStation->Class->GetFunction("WBP_RepairStation_v3_CM_C", "GetToolDurability");
    }
    if (RepairStation_GetToolDurability) {
        Params::WBP_RepairStation_v3_CM_C_GetToolDurability Parms{};
        Parms.Tool = RepairStation->SelectedTool;
        RepairStation->ProcessEvent(RepairStation_GetToolDurability, &Parms);
        Durability = Parms.Durability;
    }

    // Bricked Tool
    if (Durability < 0) {
        SetGoldButton(RepairStation->Button_GoldRepair, L"Bricked Tool", false);
        RepairAmount = 0;
        return;
    }

    if (Configuration::bEnableToolBricker) {
        RepairStation->Text_GoldCost->SetText(UKismetTextLibrary::Conv_IntToText(MaxGold - PlayerGold, false, true, 0, 6));
        SetGoldButton(RepairStation->Button_GoldRepair, L"Unrepair For", true);
        RepairAmount = PlayerGold - MaxGold;
    }    
}

void ToolBrickerDetours::SetGoldButton(UObject* Button, std::wstring Text, bool Enabled) {
    if (!Button_SetText) {
        Button_SetText = Button->Class->GetFunction("WBP_Template_Button_Primary_02_C", "SetText");
    }
    if (Button_SetText) {
        Params::WBP_Template_Button_Primary_02_C_SetText Parms{};
        Parms.Param_ButtonText = UKismetTextLibrary::Conv_StringToText(Text.c_str());
        Button->ProcessEvent(Button_SetText, &Parms);
    }
    if (!Button_SetEnabled) {
        Button_SetEnabled = Button->Class->GetFunction("WBP_Template_Button_Primary_02_C", "SetEnabled");
    }
    if (Button_SetEnabled) {
        Params::WBP_Template_Button_Primary_02_C_SetEnabled Parms{};
        Parms.Enable = Enabled;
        Button->ProcessEvent(Button_SetEnabled, &Parms);
    }
}

void ToolBrickerDetours::SetupRepairStation() {
    // always allow gold repair (even if not doing bricker)
    if (!RepairStation->ShouldEnableGoldRepair) {
        if (!RepairStation_EnableGoldRepair) {
            RepairStation_EnableGoldRepair = RepairStation->Class->GetFunction("WBP_RepairStation_v3_CM_C", "Enable Gold Repair");
        }
        if (RepairStation_EnableGoldRepair) {
            Params::WBP_RepairStation_v3_CM_C_Enable_Gold_Repair Parms{};
            Parms.Param_ShouldEnableGoldRepair = true;
            RepairStation->ProcessEvent(RepairStation_EnableGoldRepair, &Parms);
            if (!RepairStation_RefreshGoldRepairButton) {
                RepairStation_RefreshGoldRepairButton = RepairStation->Class->GetFunction("WBP_RepairStation_v3_CM_C", "RefreshGoldRepairButton");
            }
            if (RepairStation_RefreshGoldRepairButton) {
                RepairStation->ProcessEvent(RepairStation_RefreshGoldRepairButton, nullptr);
            }
        }
    }
    // alway allow repair kits from storage
    if (!RepairStation->AllowRepairKitsFromStorage) {
        if (!RepairStation_SetAllowRepairKitsFromStorage) {
            RepairStation_SetAllowRepairKitsFromStorage = RepairStation->Class->GetFunction("WBP_RepairStation_v3_CM_C", "SetAllowRepairKitsFromStorage");
        }
        if (RepairStation_SetAllowRepairKitsFromStorage) {
            Params::WBP_RepairStation_v3_CM_C_SetAllowRepairKitsFromStorage Parms{};
            Parms.Param_AllowRepairKitsFromStorage = true;
            RepairStation->ProcessEvent(RepairStation_SetAllowRepairKitsFromStorage, &Parms);
            if (!RepairStation_RefreshRepairKitRepairButton) {
                RepairStation_RefreshRepairKitRepairButton = RepairStation->Class->GetFunction("WBP_RepairStation_v3_CM_C", "RefreshRepairKitRepairButton");
            }
            if (RepairStation_RefreshRepairKitRepairButton) {
                RepairStation->ProcessEvent(RepairStation_RefreshRepairKitRepairButton, nullptr);
            }
        }
    }
}

void ToolBrickerDetours::SetRepairStation(UWBP_RepairStation_v3_CM_C* Widget) {
    RepairStation = Widget;
    if (RepairStationClass != Widget->Class) {
        RepairStationClass = Widget->Class;
        InvalidateFunctionPtrs();
    }
}

void ToolBrickerDetours::ClearRepairStation() {
    RepairStation = nullptr;
}

void ToolBrickerDetours::InvalidateFunctionPtrs() {
    Button_SetText = nullptr;
    Button_SetEnabled = nullptr;
    RepairStation_EnableGoldRepair = nullptr;
    RepairStation_RefreshGoldRepairButton = nullptr;
    RepairStation_SetAllowRepairKitsFromStorage = nullptr;
    RepairStation_RefreshRepairKitRepairButton = nullptr;
    RepairStation_GetPlayerGoldAmount = nullptr;
    RepairStation_GetGoldCostForAllTools = nullptr;
    RepairStation_FindMostDamagedTool = nullptr;
    RepairStation_GetToolDurability = nullptr;
    Checkmark_SetText = nullptr;
}

void ToolBrickerDetours::Func_PurchaseRepair(const UObject* Context, UFunction* Function, void* Parms) {
    // only overwrite if we've manually set the amount
    if (Configuration::bEnableToolBricker && RepairAmount < 0) {
        static_cast<Params::ItemUpgradeComponent_RpcServer_PurchaseRepair*>(Parms)->GoldSpent = RepairAmount;
        RepairAmount = 0;
        Configuration::bEnableToolBricker = false;
    }
}