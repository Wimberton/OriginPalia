#include "Detours/Main/WidgetDetours.h"
#include "Core/Configuration.h"
#include "Core/DetourManager.h"
#include "Core/Handlers/WidgetHandler.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"
#include <format>

#include <SDK/S6UICore_parameters.hpp>
#include <SDK/WBP_WorldMap_CM_parameters.hpp>
#include <SDK/WBP_PMenu_CM_parameters.hpp>
#include <SDK/WBP_Template_Button_Primary_01_CM_parameters.hpp>
#include <SDK/WBP_Template_Button_Secondary_01_CM_parameters.hpp>
#include <SDK/BPFL_UiHelpers_parameters.hpp>

#include "Detours/Main/ToolBrickerDetours.h"

#ifdef ENABLE_SUPPORTER_FEATURES
#include "Detours/Supporters/Entitlements/OutfitsDetours.h"
#endif

using namespace SDK;

void WidgetDetours::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
    DetourManager::AddInstance(VPC->MyHUD);
    DetourManager::AddFunctionListener("Function Engine.HUD.ReceiveDrawHUD", &ReceiveDrawHUD, nullptr);
    //DetourManager::AddFunctionDetour("Function Palia.InventoryRequestManager.CanMakeAnotherRequest", nullptr, &Func_CanMakeAnotherRequest);
    //DetourManager::AddFunctionDetour("Function Palia.InventoryRequestManager.TryGetNumAvailableRequests", nullptr, &Func_TryGetNumAvailableRequests);
    //DetourManager::AddFunctionDetour("Function Palia.InventoryRequestManager.TryGetNumActiveRequests", nullptr, &Func_TryGetNumActiveRequests);
    //DetourManager::AddFunctionDetour("Function Palia.InventoryRequestManager.TryGetNumMaxRequests", nullptr, &Func_TryGetNumMaxRequests);
    //DetourManager::AddFunctionDetour("Function Palia.VAL_EncyclopediaManagerComponent.HasSeenItem", nullptr, &Func_HasSeenItem);
    //DetourManager::AddFunctionDetour("Function BPFL_UiHelpers.BPFL_UiHelpers_C.CanItemBeRequested?", nullptr, &Func_CanItemBeRequested);
}


void WidgetDetours::ReceiveDrawHUD(const UObject* Context, UFunction* Function, void* Parms) {
    if (!WidgetHandler::GameplayUIManager) return;

    if (WidgetHandler::CurrentViewModel == EViewModel::Main) {
        if (WidgetHandler::ActiveWidgetType == EWidgetType::ReportPlayer) {
            Func_NoSnitches();
        }
        auto MainWidget = static_cast<UWBP_MainWidget_CM_C*>(WidgetHandler::GameplayUIManager->GetCurrentStateWidget());
        if (MainWidget && MainWidget->CurrentMainHUDState.bSocialPanelOpened && Configuration::bEnableRequestMods) {
            if (IsValidPtr(MainWidget->WBP_SocialWindow_CM) && IsValidPtr(MainWidget->WBP_SocialWindow_CM->WBP_RequestsPanel_CM)) {
                Func_RequestsPanelMod(MainWidget->WBP_SocialWindow_CM->WBP_RequestsPanel_CM);
            }
        }
    }
}

void WidgetDetours::Func_NoSnitches() {
    if (!WidgetHandler::ActiveWidget) return;
    auto ReportWindow = static_cast<UWBP_ReportPlayerModalDialog_CM_C*>(WidgetHandler::ActiveWidget);
    if (!ReportWindow || !ReportWindow->IsValidLowLevel() || ReportWindow->IsDefaultObject() || !ReportWindow->IsVisible()) return;
    auto ReportBtn = ReportWindow->Button_Submit;
    if (ReportButtonClass != ReportBtn->Class) {
        ReportButtonClass = ReportBtn->Class;
        ButtonPrimary_SetEnabled = nullptr;
        ButtonPrimary_SetText = nullptr;
    }
    if (ReportBtn && ReportBtn->IsValidLowLevel() && !ReportBtn->IsDefaultObject()) {
        if (!ButtonPrimary_SetText) {
            ButtonPrimary_SetText = ReportButtonClass->GetFunction("WBP_Template_Button_Primary_01_CM_C", "SetText");
            if (!ButtonPrimary_SetText) return;
        }
        Params::WBP_Template_Button_Primary_01_CM_C_SetText Parms{};
        Parms.Param_ButtonText = UKismetTextLibrary::Conv_StringToText(L"No Snitching :)");
        ReportBtn->ProcessEvent(ButtonPrimary_SetText, &Parms);
        if (!ButtonPrimary_SetEnabled) {
            ButtonPrimary_SetEnabled = ReportButtonClass->GetFunction("WBP_Template_Button_Primary_01_CM_C", "SetEnabled");
            if (!ButtonPrimary_SetEnabled) return;
        }
        Params::WBP_Template_Button_Primary_01_CM_C_SetEnabled Parms2{};
        Parms2.Enable = false;
        ReportBtn->ProcessEvent(ButtonPrimary_SetEnabled, &Parms2);
    }
}

void WidgetDetours::Func_RequestsPanelMod(UWBP_RequestsPanel_CM_C* Panel) {
    if (!Panel || !Panel->IsValidLowLevel()) return;
    TArray<UUserWidget*> FoundWidgets;
    TSubclassOf<UUserWidget> WidgetClass = UWBP_ListEntry_Request_CM_C::StaticClass();
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), &FoundWidgets, WidgetClass, false);

    for (auto Widget : FoundWidgets) {
        auto RequestEntry = static_cast<UWBP_ListEntry_Request_CM_C*>(Widget);
        if (!RequestEntry || !RequestEntry->IsValidLowLevel() || RequestEntry->IsDefaultObject() || !RequestEntry->IsVisible()) continue;
        RequestEntry->GiftButton->SetIsEnabled(true);
    }
}

// Called when 'request item' button pressed
void WidgetDetours::Func_CanMakeAnotherRequest(void* Context, void* TheStack, void* Result) {
    if (*static_cast<bool*>(Result)) {
        // already true, don't need to mess with it
        return;
    }
    *static_cast<bool*>(Result) = true;
}

// Called by the active requests widget
void WidgetDetours::Func_TryGetNumAvailableRequests(void* Context, void* TheStack, void* Result) {
    if (*static_cast<int32*>(Result) < 4) {
        *static_cast<int32*>(Result) = 4;
    }
}

void WidgetDetours::Func_TryGetNumActiveRequests(void* Context, void* TheStack, void* Result) {
    if (*static_cast<int32*>(Result) > 0) {
        *static_cast<int32*>(Result) = 0;
    }
}

void WidgetDetours::Func_TryGetNumMaxRequests(void* Context, void* TheStack, void* Result) {
    if (*static_cast<int32*>(Result) <= 4) {
        *static_cast<int32*>(Result) = 5;
    }
}

void WidgetDetours::Func_HasSeenItem(void* Context, void* TheStack, void* Result) {
    if (*static_cast<bool*>(Result)) {
        // already true, don't need to mess with it
        return;
    }
    if (Configuration::bEnableUnseenItems) {
        *static_cast<bool*>(Result) = true;
    }
}

void WidgetDetours::Func_CanItemBeRequested(void* Context, void* TheStack, void* Result) {
	auto Stack = static_cast<SDKExt::FFrame*>(TheStack);
	auto Func = Stack->Code ? *std::bit_cast<UFunction**>(&Stack->Code[0-sizeof(uint64)]) : Stack->CurrentNativeFunction;
	std::cout << "______\nFunction Called: " << Func->GetName() << "\n  Calling Object: " << static_cast<UObject*>(Context)->GetName();
	if (Stack->Node && Stack->Node != Func) {
		std::cout << "\n  Calling Function: " << Stack->Node->GetName();
	}
	std::cout << "\n------\n";
}