#pragma once

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class WidgetDetours final {
public:
	WidgetDetours() = default;

	static void RefreshWidgets();
	static void FindWidget(UClass* DesiredWidgetClass, UUserWidget* *ReturnedWidget);

	static void Func_DoGrabGameplayUIWidget();

	static UWBP_GameplayUIManager_C* GameplayUIManager;
};

inline UWBP_GameplayUIManager_C* WidgetDetours::GameplayUIManager = nullptr;