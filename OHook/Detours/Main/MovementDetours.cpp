#include "Detours/Main/MovementDetours.h"
#include "Core/Configuration.h"
#include "Overlay/PaliaOverlay.h"
#include "Misc/SDKExt.h"
#include "Misc/Utils.h"

#include "functional"
#include <fstream>

#include <SDK/Palia_parameters.hpp>

using namespace SDK;

void MovementDetours::SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC) {
    DetourManager::AddInstance(VC);
	DetourManager::AddFunctionListener("Function Engine.Actor.ReceiveTick", &Func_DoNoClip, nullptr);
	DetourManager::AddFunctionListener("Function Engine.Actor.ReceiveTick", &Func_DoPersistentMovement, nullptr);
    DetourManager::AddInstance(VC->GetValeriaCharacterMovementComponent());
	DetourManager::AddFunctionListener("Function Palia.ValeriaClientPriMovementComponent.RpcServer_SendMovement", &Func_SendMovement, nullptr);
}

void MovementDetours::Func_DoNoClip(const UObject* Context, UFunction* Function, void* Parms) {
    if (!bEnableNoclip && bEnableNoclip == bPreviousNoclipState)
        return;

    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    UValeriaCharacterMoveComponent* ValeriaMovementComponent = ValeriaCharacter->GetValeriaCharacterMovementComponent();
    if (!IsComponentValid(ValeriaMovementComponent))
        return;

    FVector ZeroVec(0, 0, 0);

    if (bEnableNoclip != bPreviousNoclipState) {
        if (bEnableNoclip) {
            ValeriaMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying, 5);
            ValeriaCharacter->CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        }
        else {
            ValeriaMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking, 1);
            ValeriaCharacter->CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
            ValeriaCharacter->CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
        }

        bPreviousNoclipState = bEnableNoclip;
    }

    // Logic for Noclip Camera
    if (bEnableNoclip) {
        if (Menu::bShowMenu || !IsGameWindowActive())
            return;

        const auto PlayerController = GetPlayerController();
        if (!PlayerController)
            return;

        const FRotator& CameraRot = PlayerController->PlayerCameraManager->GetCameraRotation();

        FVector CameraForward = UKismetMathLibrary::GetForwardVector(CameraRot);
        FVector CameraRight = UKismetMathLibrary::GetRightVector(CameraRot);
        constexpr FVector CameraUp = { 0.f, 0.f, 1.f };

        CameraForward.Normalize();
        CameraRight.Normalize();

        FVector MovementDirection = { 0.f, 0.f, 0.f };
        float FlySpeed = 800.0f;
        bool bIsMoving = false;

        if (IsKeyHeld('W')) {
            MovementDirection += CameraForward;
            bIsMoving = true;
        }
        if (IsKeyHeld('S')) {
            MovementDirection -= CameraForward;
            bIsMoving = true;
        }
        if (IsKeyHeld('D')) {
            MovementDirection += CameraRight;
            bIsMoving = true;
        }
        if (IsKeyHeld('A')) {
            MovementDirection -= CameraRight;
            bIsMoving = true;
        }
        if (IsKeyHeld(VK_SPACE)) {
            MovementDirection += CameraUp;
            bIsMoving = true;
        }
        if (IsKeyHeld(VK_CONTROL)) {
            MovementDirection -= CameraUp;
            bIsMoving = true;
        }
        if (IsKeyHeld(VK_SHIFT)) {
            FlySpeed *= 2.0f;
        }

        if (bIsMoving) {
            MovementDirection.Normalize();
            MovementDirection *= FlySpeed;
        }
        else {
            MovementDirection = ZeroVec;
            ValeriaMovementComponent->Velocity = ZeroVec;
        }

        constexpr float DeltaTime = 1.0f / 60.0f; // 60 FPS
        const FVector MovementDelta = MovementDirection * DeltaTime;

        FHitResult HitResult;
        ValeriaCharacter->K2_SetActorLocation(ValeriaCharacter->K2_GetActorLocation() + MovementDelta, false, &HitResult, false);
    }
}

void MovementDetours::Func_DoPersistentMovement(const UObject* Context, UFunction* Function, void* Parms) {
    const auto ValeriaCharacter = GetValeriaCharacter();
    if (!ValeriaCharacter)
        return;

    UValeriaCharacterMoveComponent* ValeriaMovementComponent = ValeriaCharacter->GetValeriaCharacterMovementComponent();
    if (!IsComponentValid(ValeriaMovementComponent))
        return;

    ValeriaMovementComponent->MaxWalkSpeed = Configuration::CustomWalkSpeed;
    ValeriaMovementComponent->SprintSpeedMultiplier = Configuration::CustomSprintSpeed;
    ValeriaMovementComponent->ClimbingSpeed = Configuration::CustomClimbingSpeed;
    ValeriaMovementComponent->GlidingMaxSpeed = Configuration::CustomGlidingSpeed;
    ValeriaMovementComponent->GlidingFallSpeed = Configuration::CustomGlidingFallSpeed;
    ValeriaMovementComponent->JumpZVelocity = Configuration::CustomJumpVelocity;
    ValeriaMovementComponent->MaxStepHeight = Configuration::CustomMaxStepHeight;
}

void MovementDetours::Func_SendMovement(const UObject* Context, UFunction* Function, void* Parms) {
    auto sendMovement = static_cast<Params::ValeriaClientPriMovementComponent_RpcServer_SendMovement*>(Parms);
    sendMovement->MoveInfo.TargetVelocity.X = 0;
    sendMovement->MoveInfo.TargetVelocity.Y = 0;
}