#pragma once

#include "PaliaOverlay.h"

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class TeleportDetours final {
public:
	TeleportDetours() = default;

	static void Func_DoTeleportToTargeted(PaliaOverlay* Overlay);
	static void Func_DoTeleportToWaypoint(void* Params);
	static void Func_DoTeleportLoot(PaliaOverlay* Overlay);
};