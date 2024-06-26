#pragma once

#include "PaliaOverlay.h"

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class AimDetours final {
public:
	AimDetours() = default;

	static void Func_DoInteliAim(PaliaOverlay* Overlay);
	static void Func_DoSilentAim(const PaliaOverlay* Overlay, void* Params);
	static void Func_DoLegacyAim(const PaliaOverlay* Overlay);
};