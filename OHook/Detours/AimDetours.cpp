#include "AimDetours.h"
#include "Configuration.h"
#include "PaliaOverlay.h"
#include "ActorHandler.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"

#include "functional"
#include <sstream>
#include <fstream>

#include <SDK/Palia_parameters.hpp>

#include "HUDDetours.h"
#include "MovementDetours.h"

using namespace SDK;

void AimDetours::Func_DoInteliAim() {
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
    std::string EquippedName = ValeriaCharacter->GetEquippedItem().ItemType->Name.ToString();

    bool isEquippedWithBow = EquippedName.find("Tool_Bow_") != std::string::npos;
    bool isEquippedWithInsectLauncher = EquippedName.find("Tool_InsectBallLauncher_") != std::string::npos;

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

    for (auto& [Actor, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, Distance, Texture] : ActorHandler::GetActors()) {
        if (!Actor || !IsActorValid(Actor) || WorldPosition.IsZero()) {
            continue;
        }

        bool bShouldConsider = false;

        if (isPlayerAiming) {
            switch (ActorType) {
            case EType::Animal:
                bShouldConsider = HUDDetours::Animals[Type][Variant] && isEquippedWithBow && CharacterLocation.GetDistanceToInMeters(WorldPosition) <= 22.0f;
                break;
            case EType::Bug:
                bShouldConsider = HUDDetours::Bugs[Type][Variant][Quality] && isEquippedWithInsectLauncher;
                break;
            default:
                continue;
            }
        }
        else {
            switch (ActorType) {
            case EType::Animal:
                bShouldConsider = HUDDetours::Animals[Type][Variant];
                break;
            case EType::Ore:
                bShouldConsider = HUDDetours::Ores[Type][Variant];
                break;
            case EType::Bug:
                bShouldConsider = HUDDetours::Bugs[Type][Variant][Quality];
                break;
            case EType::Forage:
                bShouldConsider = HUDDetours::Forageables[Type][Quality];
                break;
            case EType::Players:
                if (!Configuration::bAvoidTeleportingToPlayers) {
                    bShouldConsider = HUDDetours::Singles[static_cast<int>(EOneOffs::Player)];
                }
                break;
            case EType::NPCs:
                bShouldConsider = HUDDetours::Singles[static_cast<int>(EOneOffs::NPC)];
                break;
            case EType::Quest:
                bShouldConsider = HUDDetours::Singles[static_cast<int>(EOneOffs::Quest)];
                break;
            case EType::Loot:
                bShouldConsider = HUDDetours::Singles[static_cast<int>(EOneOffs::Loot)];
                break;
            case EType::Treasure:
                bShouldConsider = HUDDetours::Singles[static_cast<int>(EOneOffs::Treasure)];
                break;
            case EType::RummagePiles:
                if (HUDDetours::Singles[static_cast<int>(EOneOffs::RummagePiles)]) {
                    auto Pile = static_cast<ATimedLootPile*>(Actor);
                    if (!Pile || !IsActorValid(Pile))
                        break;

                    if (Configuration::bEnableOthers || (Pile->CanGather(ValeriaCharacter) && Pile->bActivated)) {
                        bShouldConsider = true;
                    }
                }
                break;
            case EType::Stables:
                bShouldConsider = HUDDetours::Singles[static_cast<int>(EOneOffs::Stables)];
                break;
            case EType::Tree:
                bShouldConsider = HUDDetours::Trees[Type][Variant];
                break;
            case EType::Fish:
                bShouldConsider = HUDDetours::Fish[Type];
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
        if (double Score = AngleWeight * Angle + DistanceWeight * Distance + MovementWeight * RelativeDirection.Magnitude(); Angle <= Configuration::FOVRadius / 2.0 && Score < 50.0f) {
            if (Score < currentBestScore) {
                currentBestScore = Score;
                BestScore = Score;
                BestTargetActor = Actor;
                BestTargetActorType = ActorType;
                BestTargetLocation = ActorLocation;
                BestTargetRotation = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, ActorLocation);
            }
        }
    }
}

void AimDetours::Func_DoSilentAim(void* Params) {
    if (!Configuration::bEnableSilentAimbot || !BestTargetActor || !IsActorValid(BestTargetActor))
        return;

    if (!(BestTargetActorType == EType::Animal || BestTargetActorType == EType::Bug))
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

void AimDetours::Func_DoLegacyAim() {
    if (!Configuration::bEnableAimbot || !BestTargetActor || Menu::bShowMenu)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    bool isKeyHeld = IsKeyHeld(VK_LBUTTON);
    bool isScoreValid = std::abs(BestScore - FLT_MAX) > 0.0001f;

    // Avoid teleporting if the Hotkey / Score is not valid
    if (!isKeyHeld || !isScoreValid)
        return;

    // Only aimbot when a bow is equipped
    if (ValeriaCharacter->GetEquippedItem().ItemType->Name.ToString().find("Tool_Bow_") == std::string::npos)
        return;

    bool IsAnimal = false;
    for (auto& [Actor, WorldPosition, DisplayName, ActorType, Type, Quality, Variant, Distance, Texture] : ActorHandler::GetActors()) {
        if (ActorType != EType::Animal || !IsActorValid(Actor))
            continue;

        if (Actor == BestTargetActor) {
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
        FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, BestTargetLocation);
        TargetRotation.Pitch += AimOffset.X;
        TargetRotation.Yaw += AimOffset.Y;

        UWorld* World = GetWorld();
        if (!World)
            return;

        // Smooth rotation adjustment
        FRotator NewRotation = CustomMath::RInterpTo(CharacterRotation, TargetRotation, UGameplayStatics::GetTimeSeconds(World), MovementDetours::SmoothingFactor);
        PlayerController->SetControlRotation(NewRotation);
    }
}