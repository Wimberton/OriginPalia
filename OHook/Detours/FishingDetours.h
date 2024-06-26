#pragma once

#include "PaliaOverlay.h"

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class FishingDetours final {
public:
	FishingDetours() = default;

	static void Func_DoFastAutoFishing(const PaliaOverlay* Overlay);
	static void Func_DoFishingCaptureOverride(PaliaOverlay* Overlay, void* Params);
	static void Func_DoInstantCatch();
	static Params::FishingComponent_RpcServer_EndFishing* EndFishingDetoured(const PaliaOverlay* Overlay, void* Params);
	static void Func_DoFishingCleanup();
};