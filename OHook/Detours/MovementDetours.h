#pragma once

#include "PaliaOverlay.h"

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class MovementDetours final {
public:
	MovementDetours() = default;

	static void Func_DoNoClip(PaliaOverlay* Overlay);
	static void Func_DoPersistentMovement();
};