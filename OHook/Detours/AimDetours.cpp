#include "AimDetours.h"
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

void AimDetours::Func_DoInteliAim(PaliaOverlay* Overlay) {
    if (!Configuration::bDrawFOVCircle)
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

    std::string EquippedName;
    EquippedName = ValeriaCharacter->GetEquippedItem().ItemType->Name.ToString();

    bool isEquippedWithBow = EquippedName.find("Tool_Bow_") != std::string::npos;
    bool isEquippedWithInsectLauncher = EquippedName.find("Tool_InsectBallLauncher_") != std::string::npos;

    FVector CharacterLocation = ValeriaCharacter->K2_GetActorLocation();
    FRotator CharacterRotation = PlayerController->GetControlRotation();
    FVector ForwardVector = UKismetMathLibrary::GetForwardVector(CharacterRotation);

    FVector ZeroV(0, 0, 0);
    FRotator ZeroR(0, 0, 0);

    Overlay->BestTargetActor = nullptr;
    Overlay->BestTargetActorType = EType::MAX;
    Overlay->BestTargetLocation = ZeroV;
    Overlay->BestTargetRotation = ZeroR;
    double BestScore = DBL_MAX;

    for (auto& [Actor, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, Distance, Texture] : Overlay->CachedActors) {
        if (!Actor || !IsActorValid(Actor) || WorldPosition.IsZero())
            continue;

        bool bShouldConsider = false;

        if (isPlayerAiming) {
            switch (ActorType) {
            case EType::Animal:
                bShouldConsider = Overlay->Animals[Type][Variant] && isEquippedWithBow && CharacterLocation.GetDistanceToInMeters(WorldPosition) <= 22.0f;
                break;
            case EType::Bug:
                bShouldConsider = Overlay->Bugs[Type][Variant][Quality] && isEquippedWithInsectLauncher;
                break;
            default:
                continue;
            }
        }
        else {
            switch (ActorType) {
            case EType::Animal:
                bShouldConsider = Overlay->Animals[Type][Variant];
                break;
            case EType::Ore:
                bShouldConsider = Overlay->Ores[Type][Variant];
                break;
            case EType::Bug:
                bShouldConsider = Overlay->Bugs[Type][Variant][Quality];
                break;
            case EType::Forage:
                bShouldConsider = Overlay->Forageables[Type][Quality];
                break;
            case EType::Players:
                if (!Configuration::bAvoidTeleportingToPlayers) {
                    bShouldConsider = Overlay->Singles[static_cast<int>(EOneOffs::Player)];
                }
                break;
            case EType::NPCs:
                bShouldConsider = Overlay->Singles[static_cast<int>(EOneOffs::NPC)];
                break;
            case EType::Quest:
                bShouldConsider = Overlay->Singles[static_cast<int>(EOneOffs::Quest)];
                break;
            case EType::Loot:
                bShouldConsider = Overlay->Singles[static_cast<int>(EOneOffs::Loot)];
                break;
            case EType::Treasure:
                bShouldConsider = Overlay->Singles[static_cast<int>(EOneOffs::Treasure)];
                break;
            case EType::RummagePiles:
                if (Overlay->Singles[static_cast<int>(EOneOffs::RummagePiles)]) {
                    auto Pile = static_cast<ATimedLootPile*>(Actor);
                    if (!Pile || !IsActorValid(Pile))
                        break;

                    if (Configuration::bEnableOthers || (Pile->CanGather(ValeriaCharacter) && Pile->bActivated)) {
                        bShouldConsider = true;
                    }
                }
                break;
            case EType::Stables:
                bShouldConsider = Overlay->Singles[static_cast<int>(EOneOffs::Stables)];
                break;
            case EType::Tree:
                bShouldConsider = Overlay->Trees[Type][Variant];
                break;
            case EType::Fish:
                bShouldConsider = Overlay->Fish[Type];
                break;
            default:
                break;
            }
        }

        if (!bShouldConsider)
            continue;

        FVector ActorLocation = Actor->K2_GetActorLocation();
        FVector DirectionToActor = (ActorLocation - CharacterLocation).GetNormalized();
        FVector TargetVelocity = Actor->GetVelocity();

        FVector RelativeVelocity = TargetVelocity - ValeriaCharacter->GetVelocity();
        FVector RelativeDirection = RelativeVelocity.GetNormalized();

        Distance = CharacterLocation.GetDistanceToInMeters(ActorLocation);
        float Angle = CustomMath::RadiansToDegrees(acosf(static_cast<float>(ForwardVector.Dot(DirectionToActor))));

        if (ActorLocation.IsZero())
            continue;

        if (Actor == ValeriaCharacter)
            continue;

        if (Configuration::bEnableESPCulling && Distance > Configuration::CullDistance)
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
        if (double Score = AngleWeight * Angle + DistanceWeight * Distance + MovementWeight * RelativeDirection.Magnitude(); Angle <= Configuration::FOVRadius / 2.0 && Score < Overlay->SelectionThreshold) {
            if (Score < BestScore) {
                BestScore = Score;
                Overlay->BestScore = Score;
                Overlay->BestTargetActor = Actor;
                Overlay->BestTargetActorType = ActorType;
                Overlay->BestTargetLocation = ActorLocation;
                Overlay->BestTargetRotation = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, ActorLocation);
            }
        }
    }
}

void AimDetours::Func_DoSilentAim(const PaliaOverlay* Overlay, void* Params) {
    if (!Configuration::bEnableSilentAimbot || !Overlay->BestTargetActor || !IsActorValid(Overlay->BestTargetActor))
        return;

    if (!(Overlay->BestTargetActorType == EType::Animal || Overlay->BestTargetActorType == EType::Bug))
        return;

    auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    auto FiringComponent = ValeriaCharacter->GetFiringComponent();
    if (!FiringComponent || !FiringComponent->IsValidLowLevel() || FiringComponent->IsDefaultObject())
        return;

    auto FireProjectile = static_cast<Params::ProjectileFiringComponent_RpcServer_FireProjectile*>(Params);

    for (auto& [ProjectileId, Pad_22C8, ProjectileActor, HasHit, Pad_22C9] : FiringComponent->FiredProjectiles) {
        if (ProjectileActor && IsActorValid(ProjectileActor) && ProjectileId == FireProjectile->ProjectileId) {
            
            AValeriaProjectile* ValeriaProjectile = static_cast<AValeriaProjectile*>(ProjectileActor);
            if (!ValeriaProjectile)
                continue;

            FVector TargetLocation = Overlay->BestTargetActor->K2_GetActorLocation();
            float DistanceToTarget = ValeriaCharacter->K2_GetActorLocation().GetDistanceToInMeters(TargetLocation);

            if (DistanceToTarget <= 22.0f && Overlay->BestTargetActorType == EType::Animal) {

                FVector DirectionToTarget = (TargetLocation - ValeriaProjectile->K2_GetActorLocation()).GetNormalized();
                FVector NewProjectileLocation = TargetLocation - (DirectionToTarget * 50.0f);

                FHitResult HitResult;

                ValeriaProjectile->K2_SetActorLocation(NewProjectileLocation, false, &HitResult, false);
                ValeriaProjectile->HitActorAccordingToServer = Overlay->BestTargetActor;
                HasHit = true;

                // FiringComponent->RpcServer_NotifyProjectileHit(FireProjectile->ProjectileId, Overlay->BestTargetActor, TargetLocation);
            }
            else if (Overlay->BestTargetActorType == EType::Bug) {
                ValeriaProjectile->bInstantCatchTrappedInsects = true;

                FVector DirectionToTarget = (TargetLocation - ValeriaProjectile->K2_GetActorLocation()).GetNormalized();
                FVector NewProjectileLocation = TargetLocation - (DirectionToTarget * 50.0f);

                FHitResult HitResult;

                ValeriaProjectile->K2_SetActorLocation(NewProjectileLocation, false, &HitResult, false);
                ValeriaProjectile->HitActorAccordingToServer = Overlay->BestTargetActor;
                HasHit = true;
            }
        }
    }
}

void AimDetours::Func_DoLegacyAim(const PaliaOverlay* Overlay) {
    if (!Configuration::bEnableAimbot || !Overlay->BestTargetActor || Overlay->ShowOverlay())
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    bool isKeyHeld = IsKeyHeld(VK_LBUTTON);
    bool isScoreValid = std::abs(Overlay->BestScore - FLT_MAX) > 0.0001f;

    // Avoid teleporting if the Hotkey / Score is not valid
    if (!isKeyHeld || !isScoreValid)
        return;

    // Only aimbot when a bow is equipped
    if (ValeriaCharacter->GetEquippedItem().ItemType->Name.ToString().find("Tool_Bow_") == std::string::npos)
        return;

    bool IsAnimal = false;
    for (auto& [Actor, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, Distance, Texture] : Overlay->CachedActors) {
        if (ActorType != EType::Animal || !IsActorValid(Actor))
            continue;

        if (Actor == Overlay->BestTargetActor) {
            IsAnimal = true;
            break;
        }
    }

    if (IsAnimal) {
        const auto PlayerController = GetPlayerController();
        if (!PlayerController)
            return;

        FVector CharacterLocation = ValeriaCharacter->K2_GetActorLocation();
        FRotator CharacterRotation = PlayerController->GetControlRotation();

        // Apply offset to pitch and yaw directly
        FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, Overlay->BestTargetLocation);
        TargetRotation.Pitch += Overlay->AimOffset.X;
        TargetRotation.Yaw += Overlay->AimOffset.Y;

        UWorld* World = GetWorld();
        if (!World)
            return;

        // Smooth rotation adjustment
        FRotator NewRotation = CustomMath::RInterpTo(CharacterRotation, TargetRotation, UGameplayStatics::GetTimeSeconds(World), Overlay->SmoothingFactor);
        PlayerController->SetControlRotation(NewRotation);
    }
}