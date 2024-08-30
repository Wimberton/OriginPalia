#include "Detours/Main/AimDetours.h"
#include "Core/Configuration.h"
#include "Core/Handlers/ActorHandler.h"
#include "Core/Handlers/ToolHandler.h"
#include "Overlay/PaliaOverlay.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"

#include "functional"

#include <SDK/Palia_parameters.hpp>

using namespace SDK;

void AimDetours::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
    DetourManager::AddInstance(VPC);
	DetourManager::AddFunctionListener("Function Engine.PlayerController.ClientTravelInternal", &ClientTravelInternal, nullptr);
    DetourManager::AddInstance(VPC->MyHUD);
    DetourManager::AddFunctionListener("Function Engine.HUD.ReceiveDrawHUD", &Func_DoInteliAim, nullptr);
    DetourManager::AddInstance(VC->GetFiringComponent());
	DetourManager::AddFunctionListener("Function Palia.ProjectileFiringComponent.RpcServer_FireProjectile", &Func_DoSilentAim, nullptr);
}

void AimDetours::Func_DoInteliAim(const UObject* Context, UFunction* Function, void* Parms) {
    if (!Configuration::bEnableInteliAim)
        return;

    const auto PlayerController = GetPlayerController();
    if (!PlayerController)
        return;
    
    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;
    
    auto AimingComponent = ValeriaCharacter->GetAimingComponent();
    if (!AimingComponent)
        return;

    bool isPlayerAiming = AimingComponent->IsAiming();

    FVector CharacterLocation = ValeriaCharacter->K2_GetActorLocation();
    FRotator CharacterRotation = PlayerController->GetControlRotation();
    FVector ForwardVector = UKismetMathLibrary::GetForwardVector(CharacterRotation);

    //FVector ZeroV(0, 0, 0);
    //FRotator ZeroR(0, 0, 0);

    //BestTargetActor = nullptr;
    //BestTargetActorType = EType::MAX;
    //BestTargetLocation = ZeroV;
    //BestTargetRotation = ZeroR;
    double currentBestScore = DBL_MAX;

    for (auto& Entry : ActorHandler::GetActors()) {
        if (!IsActorValid(Entry.Actor) || Entry.WorldPosition.IsZero()) {
            continue;
        }

        auto ActorType = Entry.ActorType;

        bool bShouldConsider = false;

        if (isPlayerAiming) {
            switch (ActorType) {
            case EType::Animal:
                bShouldConsider = Entry.ShouldShow(ValeriaCharacter) && (ToolHandler::EquippedTool == ETools::Bow) && CharacterLocation.GetDistanceToInMeters(Entry.WorldPosition) <= 22.0f;
                break;
            case EType::Bug:
                bShouldConsider = Entry.ShouldShow(ValeriaCharacter) && (ToolHandler::EquippedTool == ETools::Belt);
                break;
            default:
                continue;
            }
        }
        else {
            bShouldConsider = Entry.ShouldShow(ValeriaCharacter);
            if (!Configuration::bAvoidTeleportingToPlayers && Entry.ActorType == EType::Players) bShouldConsider = false;
        }

        if (!bShouldConsider)
            continue;

        FVector ActorLocation = Entry.WorldPosition;
        FVector DirectionToActor = (ActorLocation - CharacterLocation).GetNormalized();
        FVector TargetVelocity = Entry.Actor->GetVelocity();

        FVector RelativeVelocity = TargetVelocity - ValeriaCharacter->GetVelocity();
        FVector RelativeDirection = RelativeVelocity.GetNormalized();

        auto Distance = Entry.Distance;
        float Angle = CustomMath::RadiansToDegrees(acosf(static_cast<float>(ForwardVector.Dot(DirectionToActor))));

        if (ActorLocation.IsZero())
            continue;

        if (Entry.Actor == ValeriaCharacter)
            continue;

        if (Distance > Configuration::CullDistance)
            continue;

        // Weighting factors for different factors
        double AngleWeight, DistanceWeight, MovementWeight;

        // Adjust weighting factors based on EType
        switch (ActorType) {
        case EType::Animal:
            AngleWeight = 0.10;
            DistanceWeight = 0.0;
            MovementWeight = 0.0;
            break;
        case EType::Ore:
            AngleWeight = 0.10;
            DistanceWeight = 0.0;
            MovementWeight = 0.0;
            break;
        case EType::Bug:
            AngleWeight = 0.10;
            DistanceWeight = 0.0;
            MovementWeight = 0.0;
            break;
        default:
            AngleWeight = 0.10;
            DistanceWeight = 0.0;
            MovementWeight = 0.0;
            break;
        }

        // Calculate score based on weighted sum of factors
        if (double Score = AngleWeight * Angle + DistanceWeight * Distance + MovementWeight * RelativeDirection.Magnitude(); Angle <= Configuration::FOVRadius / 2.0 && Score < 50.0f) {
            if (Score < currentBestScore) {
                currentBestScore = Score;
                BestScore = Score;
                BestTargetActor = Entry.Actor;
                BestTargetActorType = ActorType;
                BestTargetLocation = ActorLocation;
                BestTargetRotation = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, ActorLocation);
            }
        }
    }
}

void AimDetours::Func_DoSilentAim(const UObject* Context, UFunction* Function, void* Parms) {
    if (!Configuration::bEnableSilentAimbot || !BestTargetActor || !IsActorValid(BestTargetActor))
        return;

    if (!(BestTargetActorType == EType::Animal || BestTargetActorType == EType::Bug))
        return;

    auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    auto FiringComponent = ValeriaCharacter->GetFiringComponent();
    if (!IsComponentValid(FiringComponent))
        return;

    auto FireProjectile = static_cast<Params::ProjectileFiringComponent_RpcServer_FireProjectile*>(Parms);

    for (auto& [ProjectileId, Pad_22C8, ProjectileActor, HasHit, Pad_22C9] : FiringComponent->FiredProjectiles) {
        if (ProjectileActor && IsActorValid(ProjectileActor) && ProjectileId == FireProjectile->ProjectileId) {
            
            AValeriaProjectile* ValeriaProjectile = ProjectileActor;
            if (!ValeriaProjectile)
                continue;

            FVector TargetLocation = BestTargetActor->K2_GetActorLocation();
            float DistanceToTarget = ValeriaCharacter->K2_GetActorLocation().GetDistanceToInMeters(TargetLocation);

            if (DistanceToTarget <= 23.0f && BestTargetActorType == EType::Animal) {

                FVector DirectionToTarget = (TargetLocation - ValeriaProjectile->K2_GetActorLocation()).GetNormalized();
                FVector NewProjectileLocation = TargetLocation - (DirectionToTarget * 50.0f);

                FHitResult HitResult;

                ValeriaProjectile->K2_SetActorLocation(TargetLocation, false, &HitResult, false);
                ValeriaProjectile->HitActorAccordingToServer = BestTargetActor;
                HasHit = true;

                // FiringComponent->RpcServer_NotifyProjectileHit(FireProjectile->ProjectileId, HUDDetours::BestTargetActor, TargetLocation);
            }
            else if (DistanceToTarget <= 120.0f && BestTargetActorType == EType::Bug) {
                ValeriaProjectile->bInstantCatchTrappedInsects = true;

                FVector DirectionToTarget = (TargetLocation - ValeriaProjectile->K2_GetActorLocation()).GetNormalized();
                FVector NewProjectileLocation = TargetLocation - (DirectionToTarget * 50.0f);

                FHitResult HitResult;

                ValeriaProjectile->K2_SetActorLocation(TargetLocation, false, &HitResult, false);
                ValeriaProjectile->HitActorAccordingToServer = BestTargetActor;
                HasHit = true;
            }
        }
    }
}

void AimDetours::ClientTravelInternal(const UObject* Context, UFunction* Function, void* Parms) {
    BestScore = 0.0f;
    BestTargetActorAnimal = nullptr;
    BestTargetActor = nullptr;
    BestTargetActorType = EType::MAX;
    BestTargetLocation = { 0, 0, 0 };
    BestTargetRotation = { 0, 0, 0 };

    AimOffset = { -1.5, -1.35, 0 };
}