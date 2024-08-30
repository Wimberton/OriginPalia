#include "Core/Handlers/WidgetHandler.h"
#include "Core/DetourManager.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"
#include <format>

#include <SDK/S6UICore_parameters.hpp>
#include <SDK/WBP_WorldMap_CM_parameters.hpp>
#include <SDK/WBP_PMenu_CM_parameters.hpp>
#include <SDK/WBP_Template_Button_Primary_01_CM_parameters.hpp>

#include "Detours/Main/ToolBrickerDetours.h"

#ifdef ENABLE_SUPPORTER_FEATURES
#include "Detours/Supporters/Entitlements/OutfitsDetours.h"
#endif

using namespace SDK;

void WidgetHandler::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
	DetourManager::AddInstance(VC);
	DetourManager::AddFunctionListener("Function Engine.Actor.ReceiveTick", &Tick, nullptr);
    DetourManager::AddFunctionListener("Function Engine.Actor.ReceiveBeginPlay", &ReceivePlay, nullptr);
    DetourManager::AddFunctionListener("Function Engine.Actor.ReceiveEndPlay", &ReceivePlay, nullptr);
    DetourManager::AddInstance(VPC->MyHUD);
    DetourManager::AddFunctionListener("Function Engine.HUD.ReceiveDrawHUD", &ReceiveDrawHUD, nullptr);

    DetourManager::AddFunctionDetour("Function S6UICore.S6UI_UIManagerWidgetBase.ActivateModalWidget", nullptr, &Func_ActivateModalWidget);
    DetourManager::AddFunctionDetour("Function S6UICore.S6UI_UIManagerWidgetBase.RemoveModalWidget", &Func_RemoveModalWidget, nullptr);
}

void WidgetHandler::Tick(const UObject* Context, UFunction* Function, void* Parms) {
    if (!GameplayUIManager) return;
    if (CurrentStateWidget != GameplayUIManager->GetCurrentStateWidget()) {
        CurrentViewModel = EViewModel::Unknown;
        CurrentStateWidget = GameplayUIManager->GetCurrentStateWidget();
        ViewModelContext = CurrentStateWidget->ViewModelContext.ContextName;
        std::cout << "\n****\nViewModel Changed: " << ViewModelContext.ToString() << "\n****\n";
        if (EViewModelMap.contains(ViewModelContext.ToString())) {
            CurrentViewModel = EViewModelMap[ViewModelContext.ToString()];
        }
    }
// #ifdef ENABLE_SUPPORTER_FEATURES
//     if (CurrentViewModel == EViewModel::PremiumStore) {
//         // keep checking the store until the data is received and the cache fills
//         if (CurrentState && CurrentState->IsA(UWBP_PremiumStoreScreen_CM_C::StaticClass())) {
//             ParseStoreContent(reinterpret_cast<UWBP_PremiumStoreScreen_CM_C*>(CurrentState));
//         }
//     }
//     else if (CurrentViewModel == EViewModel::CharacterCustomization) {
//         if (CurrentState && CurrentState->IsA(UWBP_CharacterCustomizationScreen_CM_C::StaticClass())) {
//             OutfitsDetours::CustomizationMode = reinterpret_cast<UWBP_CharacterCustomizationScreen_CM_C*>(CurrentState)->ModeConfig.Mode;
//         }
//     }
// #endif
}

void WidgetHandler::ReceivePlay(const UObject* Context, UFunction* Function, void* Parms) {
    GameplayUIManager = nullptr;
}

void WidgetHandler::ReceiveDrawHUD(const UObject* Context, UFunction* Function, void* Parms) {
    if (GameplayUIManager != nullptr) return;

    if (Context && Context->IsA(AS6UI_UIManagerHUDBase::StaticClass())) {
        auto GHUD = reinterpret_cast<const AS6UI_UIManagerHUDBase*>(Context);
        auto HUDWidget = GHUD->RootHUDWidgetInstance;
        if (GHUD->RootHUDWidgetType.Get() == UWBP_GameplayUIManager_C::StaticClass()) {
            GameplayUIManager = static_cast<UWBP_GameplayUIManager_C*>(HUDWidget);
            return;
        }
    }

    auto World = GetWorld();
    if (!World) return;

    UUserWidget* Widget = nullptr;

    FindWidget(UWBP_GameplayUIManager_C::StaticClass(), &Widget);
    GameplayUIManager = static_cast<UWBP_GameplayUIManager_C*>(Widget);
}

void WidgetHandler::FindWidget(UClass* DesiredWidgetClass, UUserWidget* *ReturnedWidget) {
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

void WidgetHandler::Func_ActivateModalWidget(void* Context, void* TheStack, void* Result) {
    ResetActiveWidget();
    auto Stack = static_cast<SDKExt::FFrame*>(TheStack);
    auto Params = reinterpret_cast<Params::S6UI_UIManagerWidgetBase_ActivateModalWidget*>(Stack->Locals);
    ActiveWidget = const_cast<UWidget*>(Params->Widget);
    ActiveWidgetType = ParseWidget(ActiveWidget->GetName());
    std::cout << "\n  ActivateWidget: " << ActiveWidget->GetName() << "\n";
    if (ActiveWidgetType == EWidgetType::PMenu) {
        PMenu = static_cast<UWBP_PMenu_CM_C*>(ActiveWidget);
        if (PMenuClass != PMenu->Class) {
            PMenuClass = PMenu->Class;
            PMenu_GetOrCreateContentForTab = nullptr;
        }
    }
    else if (ActiveWidgetType == EWidgetType::RepairStation) {
        ToolBrickerDetours::SetRepairStation(static_cast<UWBP_RepairStation_v3_CM_C*>(ActiveWidget));
    }
}

void WidgetHandler::Func_RemoveModalWidget(void* Context, void* TheStack, void* Result) {
    auto Stack = static_cast<SDKExt::FFrame*>(TheStack);
    auto Params = reinterpret_cast<Params::S6UI_UIManagerWidgetBase_RemoveModalWidget*>(Stack->Locals);
    std::cout << "\n  RemoveWidget: " << Params->Widget->GetName() << "\n";
    if (Params->Widget == ActiveWidget) {
        ResetActiveWidget();
    }
// #ifdef ENABLE_SUPPORTER_FEATURES
//     if (ParseWidget(Params->Widget->GetName()) == EWidgetType::CollectionsScreen) {
//         OutfitsDetours::Func_GetSpoofedLoadout_Collections();
//     }
// #endif
}

void WidgetHandler::ResetActiveWidget() {
    ActiveWidget = nullptr;
    ActiveWidgetType = EWidgetType::Unknown;
    PMenu = nullptr;
    WorldMap = nullptr;
    MapCenter = { 0,0 };
    ToolBrickerDetours::ClearRepairStation();
}

bool WidgetHandler::IsWidgetOpen() {
    return (CurrentViewModel != EViewModel::Main) || ActiveWidget;
}

bool WidgetHandler::IsMapOpen() {
    return PMenu && PMenu->IsActivated() && PMenu->CurrentMode == EVAL_GameplayMenuMode::Map;
}

UWBP_WorldMap_CM_C* WidgetHandler::GetMapWidget() {
    if (!IsMapOpen()) return nullptr;
    if (!WorldMap && PMenu) {
        if (!PMenu_GetOrCreateContentForTab) {
            PMenu_GetOrCreateContentForTab = UObject::FindObject<UFunction>("Function WBP_PMenu_CM.WBP_PMenu_CM_C.GetOrCreateContentForTab", EClassCastFlags::Function);
            if (!PMenu_GetOrCreateContentForTab) return nullptr;
        }
        Params::WBP_PMenu_CM_C_GetOrCreateContentForTab Parms{};
        Parms.TabName = UKismetStringLibrary::Conv_StringToName(FString(L"Map"));
        PMenu->ProcessEvent(PMenu_GetOrCreateContentForTab, &Parms);
        if (Parms.TabContentWidget) {
            WorldMap = static_cast<UWBP_WorldMap_CM_C*>(Parms.TabContentWidget);
            if (WorldMapClass != WorldMap->Class) {
                WorldMapClass = WorldMap->Class;
                WorldMap_WorldPositionToUiMapPosition = nullptr;
            }
        }
        else {
            return nullptr;
        }
    }
    return WorldMap;
}

void WidgetHandler::SetupPoints() {
    if (!PMenu->Box_MainContentHolder) return;
    auto Geometry = PMenu->Box_MainContentHolder->GetCachedGeometry();
    auto LocalSize = USlateBlueprintLibrary::GetLocalSize(Geometry);
    auto LocalTopLeft = USlateBlueprintLibrary::GetLocalTopLeft(Geometry);
    auto World = GetWorld();
    if(!World) return;
    FVector2D ViewportTopLeft;
    USlateBlueprintLibrary::LocalToViewport(World, Geometry, LocalTopLeft, nullptr, &ViewportTopLeft);
    MapUpperLeft = ViewportTopLeft;
    MapLowerRight = { MapUpperLeft.X + LocalSize.X, MapUpperLeft.Y + LocalSize.Y };
    MapCenter = { MapUpperLeft.X + LocalSize.X / 2, MapUpperLeft.Y + LocalSize.Y / 2 };
}

std::pair<bool, FVector2D> WidgetHandler::GetMapPoint(const FVector& ActorLocation) {
    auto MapWidget = GetMapWidget();
    if (!MapWidget || !MapWidget->IsActivated()) return { false, {0,0} };
    if (MapCenter.X <= 0 && MapCenter.Y <= 0) {
        SetupPoints();
        if (MapCenter.X <= 0 && MapCenter.Y <= 0) return { false, {0, 0} };
    }
    if (!WorldMap_WorldPositionToUiMapPosition) {
        WorldMap_WorldPositionToUiMapPosition = MapWidget->Class->GetFunction("WBP_WorldMap_CM_C", "WorldPositionToUiMapPosition");
    }
    if (!WorldMap_WorldPositionToUiMapPosition) return { false, {0, 0} };
    Params::WBP_WorldMap_CM_C_WorldPositionToUiMapPosition PositionFuncParams{};
    PositionFuncParams.WorldLocation = ActorLocation;
    MapWidget->ProcessEvent(WorldMap_WorldPositionToUiMapPosition, &PositionFuncParams);
    auto OffsetLocation = PositionFuncParams.Result;
    FVector2D ScreenLocation = { MapCenter.X + OffsetLocation.X, MapCenter.Y + OffsetLocation.Y };
    bool InBounds = true;
    if (ScreenLocation.X < MapUpperLeft.X || ScreenLocation.X > MapLowerRight.X) InBounds = false;
    if (ScreenLocation.Y < MapUpperLeft.Y || ScreenLocation.Y > MapLowerRight.Y) InBounds = false;
    return { InBounds, ScreenLocation };
}

void WidgetHandler::ParseStoreContent(UWBP_PremiumStoreScreen_CM_C* StoreScreen) {
    if (!StoreScreen) return;
#ifdef ENABLE_SUPPORTER_FEATURES
    if (!OutfitsDetours::KnownGoodGuids.empty()) return;
    for (auto WidgetModel : StoreScreen->Cached_Storefront_Response.Models) {
        for (auto Variant : WidgetModel.Variants) {
            for (auto ItemId : Variant.ItemIds) {
                OutfitsDetours::KnownGoodGuids.push_back(ItemId);
                //std::cout << "**** Adding Guid: " << UKismetGuidLibrary::Conv_GuidToString(ItemId).ToString() << " ****\n";
            }
        }
    }
    for (auto PurchaseReward : StoreScreen->Cached_Storefront_Response.PurchaseRewards.PurchaseRewards) {
        for (auto ItemId : PurchaseReward.RewardIds) {
            OutfitsDetours::KnownGoodGuids.push_back(ItemId);
            //std::cout << "**** Adding Guid: " << UKismetGuidLibrary::Conv_GuidToString(ItemId).ToString() << " ****\n";
        }
    }
#endif
};

EWidgetType WidgetHandler::ParseWidget(std::string WidgetName) {
    if (WidgetName.starts_with("WBP_PMenu_CM_C")) {
        return EWidgetType::PMenu;
    }
    else if (WidgetName.starts_with("WBP_RepairStation_v3_CM_C")) {
        return EWidgetType::RepairStation;
    }
    else if (WidgetName.starts_with("WBP_CollectionsScreen_CM_C")) {
        return EWidgetType::CollectionsScreen;
    }
    else if (WidgetName.starts_with("WBP_ReportPlayerModalDialog_CM_C")) {
        return EWidgetType::ReportPlayer;
    }
    else {
        return EWidgetType::Unknown;
    }
}
