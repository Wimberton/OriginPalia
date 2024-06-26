#include "TeleportDetours.h"
#include "Configuration.h"
#include "PaliaOverlay.h"
#include "SDKExt.h"
#include "Utils.h"

#include "functional"
#include <fstream>

#include <SDK/Palia_parameters.hpp>

using namespace SDK;

void TeleportDetours::Func_DoTeleportToTargeted(PaliaOverlay* Overlay) {
    if (!Configuration::bTeleportToTargeted || !IsGameWindowActive() || Overlay->ShowOverlay())
        return;

    // Avoid teleporting if the Hotkey / Score is not valid
    if (!IsKeyUp(VK_XBUTTON2) || !std::abs(Overlay->BestScore - DBL_MAX) > 0.0001f || Overlay->BestTargetActor == nullptr)
        return;

    // Avoid teleporting to players
    if (Configuration::bAvoidTeleportingToPlayers && Overlay->BestTargetActorType == EType::Players)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    // Avoid teleporting to targeted if there are nearby players
    if (Configuration::bDoRadiusPlayersAvoidance) {
        for (auto& [Actor, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, Distance, Texture] : Overlay->CachedActors) {
            if (!Actor || ActorType != EType::Players)
                continue;

            if (!IsActorValid(Actor) || !IsActorValid(Overlay->BestTargetActor) || WorldPosition.IsZero())
                continue;

            // Don't count itself or us
            if (Actor == Overlay->BestTargetActor || Actor == ValeriaCharacter)
                continue;

            // Check for actors within X meters of this actor
            if (WorldPosition.GetDistanceToInMeters(Overlay->BestTargetLocation) < Configuration::AvoidanceRadius) {
                return;
            }
        }
    }

    // Teleportation logic
    FVector TargetLocation = Overlay->BestTargetLocation;

    // Apply horizontal offset for animal targets
    if (Overlay->BestTargetActorType == EType::Animal || Overlay->BestTargetActorType == EType::Tree) {
        FVector RightVector = ValeriaCharacter->GetActorRightVector();
        TargetLocation += RightVector * 160.0f;
    }

    // Apply vertical offset for forage targets
    if (Overlay->BestTargetActorType == EType::Forage || Overlay->BestTargetActorType == EType::Loot) {
        FVector UpVector = ValeriaCharacter->GetActorUpVector();
        TargetLocation.Z += 200.0f;
    }

    TeleportPlayer(TargetLocation);
    ValeriaCharacter->FlushNetDormancy();
    ValeriaCharacter->ForceNetUpdate();

    // Interaction after teleportation
    if (!Configuration::bEnableAutoGather && (Overlay->BestTargetActorType == EType::Forage || Overlay->BestTargetActorType == EType::Loot)) {
        auto* interactableComp = static_cast<UInteractableComponent*>(Overlay->BestTargetActor->GetComponentByClass(UInteractableComponent::StaticClass()));
        
        if (interactableComp) {

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
        TargetLocation.Z += 200.0f;
        TeleportPlayer(TargetLocation);
    }
}

void TeleportDetours::Func_DoTeleportLoot(PaliaOverlay* Overlay) {
    if (!Configuration::bEnableLootbagTeleportation)
        return;

    auto* ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    FVector PawnLocation = ValeriaCharacter->K2_GetActorLocation();

    for (auto& Entry : Overlay->CachedActors) {
        if (Entry.ActorType == EType::Loot && Entry.Actor) {
            FHitResult HitResult;
            Entry.Actor->K2_SetActorLocation(PawnLocation, true, &HitResult, true);
        }
    }
}