#include "WidgetDetours.h"
#include "Misc/SDKExt.h"

#include <SDK/Palia_parameters.hpp>

using namespace SDK;

void WidgetDetours::RefreshWidgets() {
    GameplayUIManager = nullptr;
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

void WidgetDetours::Func_DoGrabGameplayUIWidget() {
    if (GameplayUIManager != nullptr) return;

    auto World = GetWorld();
    if (!World) return;

    UUserWidget* Widget = nullptr;

    FindWidget(UWBP_GameplayUIManager_C::StaticClass(), &Widget);
    GameplayUIManager = static_cast<UWBP_GameplayUIManager_C*>(Widget);
}