#include "WidgetDetours.h"
#include "Configuration.h"
#include "PaliaOverlay.h"
#include "SDKExt.h"
#include "Utils.h"

#include <algorithm>
#include "functional"
#include <sstream>
#include <fstream>

#include <SDK/Palia_parameters.hpp>

using namespace SDK;

void WidgetDetours::RefreshWidgets(PaliaOverlay* Overlay) {
    Overlay->GameplayUIManager = nullptr;
}

void WidgetDetours::FindWidget(UClass* DesiredWidgetClass, UUserWidget* *ReturnedWidget) {
    auto World = GetWorld();
    if (!World) return;

    TArray<UUserWidget*> FoundWidgets;
    TSubclassOf<UUserWidget> WidgetClass = DesiredWidgetClass;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(World, &FoundWidgets, WidgetClass, false);

    for (UUserWidget* Widget : FoundWidgets) {
        if (!Widget) continue;
        if (!Widget->IsA(WidgetClass)) continue;
        if (!Widget->IsValidLowLevel()) continue;
        if (Widget->IsDefaultObject()) continue;

        *ReturnedWidget = Widget;
    }
}

void WidgetDetours::Func_DoGrabGameplayUIWidget(PaliaOverlay* Overlay) {
    if (Overlay->GameplayUIManager != nullptr) return;

    auto World = GetWorld();
    if (!World) return;

    UUserWidget* Widget = nullptr;

    FindWidget(UWBP_GameplayUIManager_C::StaticClass(), &Widget);
    if (auto GameplayUIManager = static_cast<UWBP_GameplayUIManager_C*>(Widget); GameplayUIManager != nullptr)
        Overlay->GameplayUIManager = GameplayUIManager;
}