#pragma once

#include "PaliaOverlay.h"

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class HUDDetours final {
public:
	HUDDetours() = default;

	static void Func_DoFOVCircle(const PaliaOverlay* Overlay, const AHUD* HUD);
	static void Func_DoESP(PaliaOverlay* Overlay, const AHUD* HUD);
};