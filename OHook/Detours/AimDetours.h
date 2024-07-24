#pragma once

#include "PaliaOverlay.h"

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class AimDetours final {
public:
	AimDetours() = default;

	static void Func_DoInteliAim();
	static void Func_DoSilentAim(void* Params);
	static void Func_DoLegacyAim();
	
	static double BestScore;
	static AActor* BestTargetActorAnimal;
	static AActor* BestTargetActor;
	static EType BestTargetActorType;
	static FVector BestTargetLocation;
	static FRotator BestTargetRotation;

	static FVector AimOffset;
};

inline double AimDetours::BestScore = 0.0f;
inline AActor* AimDetours::BestTargetActorAnimal = nullptr;
inline AActor* AimDetours::BestTargetActor = nullptr;
inline EType AimDetours::BestTargetActorType = EType::MAX;
inline FVector AimDetours::BestTargetLocation = { 0, 0, 0 };
inline FRotator AimDetours::BestTargetRotation = { 0, 0, 0 };

inline FVector AimDetours::AimOffset = { -1.5, -1.35, 0 };