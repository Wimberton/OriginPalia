#include "Detours/Main/HousingDetours.h"
#include "Core/Configuration.h"
#include "Core/DetourManager.h"
#include "Core/HotkeysManager.h"
#include "Core/Handlers/WidgetHandler.h"
#include "Misc/Utils.h"

#include <SDK/Palia_parameters.hpp>
#include <bit>


using namespace SDK;

void HousingDetours::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
    DetourManager::AddInstance(VPC->MyHUD);
    DetourManager::AddFunctionListener("Function Engine.HUD.ReceiveDrawHUD", &Func_DoPlaceAnywhere, nullptr);
    DetourManager::AddInstance(VC->GetPlacement());
    DetourManager::AddFunctionListener("Function Palia.PlacementComponent.RpcServer_PlaceItem", &Func_PE_RpcServer_PlaceItem, nullptr);
    DetourManager::AddFunctionListener("Function Palia.PlacementComponent.RpcServer_UpdateLockedItemToPlace", &Func_PE_RpcServer_UpdateLockedItemToPlace, nullptr);
    DetourManager::AddFunctionListener("Function Palia.PlacementComponent.RpcServer_LockItemToPlace", &Func_PE_RpcServer_LockItemToPlace, nullptr);
}

void HousingDetours::Func_DoPlaceAnywhere(const UObject* Context, UFunction* Function, void* Parms) {
    if (!Configuration::bPlaceAnywhere)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    UPlacementComponent* PlacementComponent = ValeriaCharacter->GetPlacement();
    if (!IsComponentValid(PlacementComponent))
        return;

    if (!PlacementComponent->IsPlacing())
        return; // not currently placing anything

    if (PlacementComponent->CanPlaceHere)
        return; // already in placeable location

    if (PlacementComponent->ActiveLockedPlacementItem.ID != 0) {
        // has been placed in valid location before
        PlacementComponent->CanPlaceHere = true;
        return;
    }

    auto Manager = WidgetHandler::GameplayUIManager;
    if (!Manager) return;

    switch (WidgetHandler::CurrentViewModel) {
        case EViewModel::Main:
        {
            auto MainWidget = static_cast<UWBP_MainWidget_CM_C*>(Manager->GetCurrentStateWidget());
            if (!MainWidget) return;
            auto PlacementHud = MainWidget->PlacementHud;
            if (!PlacementHud && !PlacementHud->Text_Error) return;
            PlacementHud->Text_Error->SetText(UKismetTextLibrary::Conv_StringToText(L"Please place in a valid location first"));
        }
        break;
        case EViewModel::Housing:
        {

        }
        break;
    }
}

void HousingDetours::Func_PE_RpcServer_PlaceItem(const UObject* Context, UFunction* Function, void* Parms) {
    if (!Configuration::bPlaceAnywhere)
        return;
    auto Params = static_cast<Params::PlacementComponent_RpcServer_PlaceItem*>(Parms);
    Params->Rotation = PlacementActorRotation;
}

void HousingDetours::Func_PE_RpcServer_UpdateLockedItemToPlace(const UObject* Context, UFunction* Function, void* Parms) {
    if (!Configuration::bPlaceAnywhere)
        return;
    auto PlacementComponent = static_cast<const UPlacementComponent*>(Context);
    auto Params = static_cast<Params::PlacementComponent_RpcServer_UpdateLockedItemToPlace*>(Parms);
    if (NewItem) {
        PlacementActorRotation = Params->Rotation;
        PlacementPositionOffset = {0,0,0};
        LastRotation = Params->Rotation;
        NewItem = false;
    }
    if (nearbyint(LastRotation.Pitch) != nearbyint(Params->Rotation.Pitch) || 
        nearbyint(LastRotation.Roll) != nearbyint(Params->Rotation.Roll) || 
        nearbyint(LastRotation.Yaw) != nearbyint(Params->Rotation.Yaw)) 
    {
        std::cout << "NewRotation Pitch: " << Params->Rotation.Pitch << " Roll: " << Params->Rotation.Roll << " Yaw: " << Params->Rotation.Yaw << "\n";
        double Old, New = 0.0;
        if (nearbyint(LastRotation.Pitch) != nearbyint(Params->Rotation.Pitch)) {
            ModifiedAxis = 0;
            Old = LastRotation.Pitch;
            New = Params->Rotation.Pitch;
        } else if (nearbyint(LastRotation.Roll) != nearbyint(Params->Rotation.Roll)) {
            ModifiedAxis = 1;
            Old = LastRotation.Roll;
            New = Params->Rotation.Roll;
        } else if (nearbyint(LastRotation.Yaw) != nearbyint(Params->Rotation.Yaw)) {
            ModifiedAxis = 2;
            Old = LastRotation.Yaw;
            New = Params->Rotation.Yaw;
        }
        
        double Change = 0.0;
        if (nearbyint(Old) == 165.0 && nearbyint(New) == -180.0) {
            Change = 15.0;
        } else if (nearbyint(Old) == -180.0 && nearbyint(New) == 165.0) {
            Change = -15.0;
        } else if (Old < New) {
            Change = 15.0;
        } else if (Old > New) {
            Change = -15.0;
        }
        if (IsKeyOn(VK_CAPITAL)) {
            Change = Change / 15.0 * Configuration::PlacementRotation; // if caps lock on rotate config degrees
        }

        if (IsKeyHeld(VK_SHIFT)) {
            ChangeAxis((ModifiedAxis + 1) % 3, Change, &PlacementActorRotation);
        } else if (IsKeyHeld(VK_MENU)) { // Alt Key
            ChangeAxis((ModifiedAxis + 2) % 3, Change, &PlacementActorRotation);
        } else if (IsKeyHeld('Z')) {
            PlacementPositionOffset.Z += Change;
        } else if (IsKeyHeld('X')) {
            PlacementPositionOffset.X += Change;
        } else if (IsKeyHeld('C')) {
            PlacementPositionOffset.Y += Change;
        } else {
            ChangeAxis(ModifiedAxis, Change, &PlacementActorRotation);
        }

        LastRotation = Params->Rotation;
    }
    Params->Rotation = PlacementActorRotation;
    Params->Position += PlacementPositionOffset;
    PlacementComponent->ItemToPlaceView->K2_SetActorLocationAndRotation(Params->Position, Params->Rotation, false, nullptr, true);
}

void HousingDetours::Func_PE_RpcServer_LockItemToPlace(const UObject* Context, UFunction* Function, void* Parms) {
    NewItem = true;
}

void HousingDetours::ChangeAxis(int Axis, double Change, FRotator* Rotation) {
    switch (Axis) {
        case 0:
        Rotation->Pitch += Change;
        break;
        case 1:
        Rotation->Roll += Change;
        break;
        case 2:
        Rotation->Yaw += Change;
        break;
    }
}