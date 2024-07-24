#include "TeleportDetours.h"
#include "Configuration.h"
#include "PaliaOverlay.h"
#include "ActorHandler.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"

#include "functional"
#include <fstream>

#include <SDK/Palia_parameters.hpp>

#include "AimDetours.h"

using namespace SDK;

void TeleportDetours::Func_DoTeleportToTargeted() {
    if (!Configuration::bTeleportToTargeted || !IsGameWindowActive() || Menu::bShowMenu)
        return;

    // Avoid teleporting if the Hotkey / Score is not valid
    if (!IsKeyUp(VK_XBUTTON2) || !std::abs(AimDetours::BestScore - DBL_MAX) > 0.0001f || AimDetours::BestTargetActor == nullptr)
        return;

    // Avoid teleporting to players
    if (Configuration::bAvoidTeleportingToPlayers && AimDetours::BestTargetActorType == EType::Players)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    // Teleportation logic
    FVector TargetLocation = AimDetours::BestTargetLocation;
    
    // Avoid teleporting to targeted if there are nearby players
    if (Configuration::bDoRadiusPlayersAvoidance) {
        for (auto& [Player, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, Distance, Texture] : ActorHandler::GetActors()) {
            if (!Player || ActorType != EType::Players)
                continue;

            if (!IsActorValid(Player) || !IsActorValid(AimDetours::BestTargetActor) || WorldPosition.IsZero())
                continue;

            // Don't count itself or us
            if (Player == AimDetours::BestTargetActor || Player == ValeriaCharacter)
                continue;

            // Check for actors within X meters of this target
            if (TargetLocation.GetDistanceToInMeters(WorldPosition) < Configuration::AvoidanceRadius) {
                return;
            }
        }
    }

    TeleportPlayer(TargetLocation, 180.0f, ETeleportDirection::Forward);
    
    // // Apply horizontal offset for animal/tree targets
    // if (AimDetours::BestTargetActorType == EType::Animal || AimDetours::BestTargetActorType == EType::Tree) {
    //     TeleportPlayer(TargetLocation, 200.0f, ETeleportDirection::Forward);
    // }
    // // Apply vertical offset for everything to prevent going through the ground
    // else {
    //     TeleportPlayer(TargetLocation, 200.0f, ETeleportDirection::Upwards);
    // }

    // Interaction after teleportation
    if (!Configuration::bEnableAutoGather && (AimDetours::BestTargetActorType == EType::Forage || AimDetours::BestTargetActorType == EType::Loot)) {
        auto* interactableComp = static_cast<UInteractableComponent*>(AimDetours::BestTargetActor->GetComponentByClass(UInteractableComponent::StaticClass()));
        
        if (interactableComp && interactableComp->IsValidLowLevel() && !interactableComp->IsDefaultObject()) {
            interactableComp->bSendTelemetryEventOnServerInteract = false;

            ValeriaCharacter->Interactor->HighlightInteractable(interactableComp);

            FInteractEventParams interactionParams;
            interactionParams.Type = EInteractEventType::Pressed;
            interactionParams.Index = EInteractEventIndex::Primary;
            interactionParams.InputType = EInputType::Keyboard;
            interactionParams.Handled = false;

            ValeriaCharacter->Interactor->InteractWithHighlighted(interactionParams);
        }
    }
}

void TeleportDetours::Func_DoTeleportToWaypoint(void* Params) {
    if (!Configuration::bEnableWaypointTeleport)
        return;

    FVector TargetLocation;
    if (auto WaypointParams = static_cast<Params::TrackingComponent_RpcServer_SetUserMarker*>(Params)) {
        TargetLocation = WaypointParams->Location;
    }
    else if (auto WorldMapParams = static_cast<Params::TrackingComponent_RpcServer_SetUserMarkerViaWorldMap*>(Params)) {
        FVector2D WorldMapMarkerLocation = WorldMapParams->WorldMapMarkerLocation;
        TargetLocation = { WorldMapMarkerLocation.X, WorldMapMarkerLocation.Y, 0.0f };
    }

    if (!TargetLocation.IsZero()) {
        TeleportPlayer(TargetLocation, 200.0f, ETeleportDirection::Upwards);
    }
}


void TeleportDetours::TeleportPlayer(const FVector& Destination, float Offset = 0.0f, ETeleportDirection Direction = ETeleportDirection::None) {
    // Character Validation
    const auto World = GetWorld();
    if (!World)
        return;
    
    const auto PlayerController = GetPlayerController();
    if (!PlayerController)
        return;
    
    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    FVector CharacterLocation = ValeriaCharacter->K2_GetActorLocation();
    if (CharacterLocation.IsZero())
        return;
    
    auto Movement = ValeriaCharacter->MovementComponent;
    if (!Movement || !Movement->IsValidLowLevel() || Movement->IsDefaultObject())
        return;
    
    FVector DirectionVector = (Destination - CharacterLocation).GetNormalized();
    FVector Dest = Destination;

    switch (Direction) {
    case ETeleportDirection::Forward:
        Dest -= DirectionVector * Offset;
        break;
    case ETeleportDirection::Back:
        Dest += DirectionVector * Offset;
        break;
    case ETeleportDirection::Right:
        Dest += {-DirectionVector.Y, DirectionVector.X, 0} * Offset;
        break;
    case ETeleportDirection::Left:
        Dest += {DirectionVector.Y, -DirectionVector.X, 0} * Offset;
        break;
    case ETeleportDirection::Upwards:
        Dest.Z += Offset;
        break;
    case ETeleportDirection::Downwards:
        Dest.Z -= Offset;
        break;
    case ETeleportDirection::None:
        break;
    }
    Dest.Z += Offset;

    FRotator CharacterRotation = ValeriaCharacter->K2_GetActorRotation();
    FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, Destination);

    NewRotation = {CharacterRotation.Pitch, NewRotation.Yaw, 0};
    ValeriaCharacter->MovementComponent->RpcClient_ApplyTeleport(Dest, NewRotation);

    DoGCLogic(ValeriaCharacter);
}

inline int teleportingFlushCounter = 0;
void TeleportDetours::DoGCLogic(const AValeriaCharacter* ValeriaCharacter) {
    teleportingFlushCounter++;
    if (teleportingFlushCounter >= 3) {
        // Helps with frames over time
        ValeriaCharacter->ValeriaPlayerController->ClientFlushLevelStreaming();
        ValeriaCharacter->ValeriaPlayerController->ClientForceGarbageCollection();
        teleportingFlushCounter = 0;
    }
}