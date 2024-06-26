#pragma once

#include "PaliaOverlay.h"

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class WidgetDetours final {
public:
	WidgetDetours() = default;

	static void RefreshWidgets(PaliaOverlay* Overlay);
	static void FindWidget(UClass* DesiredWidgetClass, UUserWidget* *ReturnedWidget);

	static void Func_DoGrabGameplayUIWidget(PaliaOverlay* Overlay);
};