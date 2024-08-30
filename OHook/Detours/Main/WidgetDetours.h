#pragma once

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class WidgetDetours final {
public:
	WidgetDetours() = default;

	static void SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC);
	
	static void ReceiveDrawHUD(const UObject* Context, UFunction* Function, void* Parms);
	static void Func_NoSnitches();
	static void Func_RequestsPanelMod(UWBP_RequestsPanel_CM_C*);
	static void Func_CanMakeAnotherRequest(void*, void*, void*);
	static void Func_TryGetNumAvailableRequests(void*, void*, void*);
	static void Func_TryGetNumActiveRequests(void*, void*, void*);
	static void Func_TryGetNumMaxRequests(void*, void*, void*);
	static void Func_HasSeenItem(void*, void*, void*);
	static void Func_CanItemBeRequested(void*, void*, void*);

	static UClass* ReportButtonClass;
	static UFunction* ButtonPrimary_SetText;
	static UFunction* ButtonPrimary_SetEnabled;

	static EMainHudState CurrentHudState;
};

inline UClass* WidgetDetours::ReportButtonClass = nullptr;
inline UFunction* WidgetDetours::ButtonPrimary_SetText = nullptr;
inline UFunction* WidgetDetours::ButtonPrimary_SetEnabled = nullptr;
inline EMainHudState WidgetDetours::CurrentHudState;