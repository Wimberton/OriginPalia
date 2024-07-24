#pragma once

#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class MovementDetours final {
public:
	MovementDetours() = default;

	static void Func_DoNoClip();
	static void Func_DoPersistentMovement();

	static float SmoothingFactor;
	
	// Movement Booleans
	static bool bEnableNoclip;
	static bool bPreviousNoclipState;

	// Movement Numericals
	static int currentMovementModeIndex;
	static int globalGameSpeedIndex;

	// Movement Floats
	static float velocitySpeedMultiplier;
	static float GlobalGameSpeed;
	static float NoClipFlySpeed;
	static float WalkSpeed;
	static float GameSpeed;
	static float SprintSpeedMultiplier;
	static float ClimbingSpeed;
	static float GlidingSpeed;
	static float GlidingFallSpeed;
	static float JumpVelocity;
	static float MaxStepHeight;
	static float CustomGameSpeed;
};

inline float MovementDetours::SmoothingFactor = 90.0f;

// Movement Booleans
inline bool MovementDetours::bEnableNoclip = false;
inline bool MovementDetours::bPreviousNoclipState = false;

// Movement Numericals
inline int MovementDetours::currentMovementModeIndex = 0;
inline int MovementDetours::globalGameSpeedIndex = 0;

// Movement Floats
inline float MovementDetours::velocitySpeedMultiplier = 0.0f;
inline float MovementDetours::GlobalGameSpeed = 1.0f;
inline float MovementDetours::NoClipFlySpeed = 600.0f;
inline float MovementDetours::WalkSpeed = 565.0f;
inline float MovementDetours::GameSpeed = 1.0f;
inline float MovementDetours::SprintSpeedMultiplier = 1.65f;
inline float MovementDetours::ClimbingSpeed = 80.0f;
inline float MovementDetours::GlidingSpeed = 900.0f;
inline float MovementDetours::GlidingFallSpeed = 250.0f;
inline float MovementDetours::JumpVelocity = 700.0f;
inline float MovementDetours::MaxStepHeight = 45.0f;
inline float MovementDetours::CustomGameSpeed = 1.0f;
