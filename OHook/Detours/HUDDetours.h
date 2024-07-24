#pragma once

#include "PaliaOverlay.h"

#include <SDK.hpp>
#include <SDK/Palia_parameters.hpp>

class PaliaOverlay;

using namespace SDK;

class HUDDetours final {
public:
	HUDDetours() = default;

	static void Func_DoESP(const AHUD* HUD);

	static bool Ores[static_cast<int>(EOreType::MAX)][static_cast<int>(EGatherableSize::MAX)];
	static bool Animals[static_cast<int>(ECreatureKind::MAX)][static_cast<int>(ECreatureQuality::MAX)];
	static bool Bugs[static_cast<int>(EBugKind::MAX)][static_cast<int>(EBugQuality::MAX)][2];
	static bool Trees[static_cast<int>(ETreeType::MAX)][static_cast<int>(EGatherableSize::MAX)];
	static bool Fish[static_cast<int>(EFishType::MAX)];
	static bool Singles[static_cast<int>(EOneOffs::MAX)];

	static bool Forageables[static_cast<int>(EForageableType::MAX)][2];
	static int ForageableCommon[4];
	static int ForageableUncommon[9];
	static int ForageableRare[3];
	static int ForageableEpic[2];
};

// Ores[Type][Size]
inline bool HUDDetours::Ores[static_cast<int>(EOreType::MAX)][static_cast<int>(EGatherableSize::MAX)] = {};
// Animals[Type][Size]
inline bool HUDDetours::Animals[static_cast<int>(ECreatureKind::MAX)][static_cast<int>(ECreatureQuality::MAX)] = {};
// Bugs[Type][Size][Starred]
inline bool HUDDetours::Bugs[static_cast<int>(EBugKind::MAX)][static_cast<int>(EBugQuality::MAX)][2] = {};
// Trees[Type][Size]
inline bool HUDDetours::Trees[static_cast<int>(ETreeType::MAX)][static_cast<int>(EGatherableSize::MAX)] = {};
// Fish[Type]
inline bool HUDDetours::Fish[static_cast<int>(EFishType::MAX)] = {};
// Singles[Type]
inline bool HUDDetours::Singles[static_cast<int>(EOneOffs::MAX)] = {};
// Forageables[Type][Starred]
inline bool HUDDetours::Forageables[static_cast<int>(EForageableType::MAX)][2] = {};

// Forageables[Type][]
inline int HUDDetours::ForageableCommon[4] = {
	static_cast<int>(EForageableType::Oyster),
	static_cast<int>(EForageableType::Shell),
	static_cast<int>(EForageableType::Sundrop),
	static_cast<int>(EForageableType::MushroomRed)
};
inline int HUDDetours::ForageableUncommon[9] = {
	static_cast<int>(EForageableType::Coral),
	static_cast<int>(EForageableType::PoisonFlower),
	static_cast<int>(EForageableType::WaterFlower),
	static_cast<int>(EForageableType::EmeraldCarpet),
	static_cast<int>(EForageableType::SpicedSprouts),
	static_cast<int>(EForageableType::SweetLeaves),
	static_cast<int>(EForageableType::Garlic),
	static_cast<int>(EForageableType::Ginger),
	static_cast<int>(EForageableType::GreenOnion)
};
inline int HUDDetours::ForageableRare[3] = {
	static_cast<int>(EForageableType::DragonsBeard),
	static_cast<int>(EForageableType::MushroomBlue),
	static_cast<int>(EForageableType::HeatRoot)
};
inline int HUDDetours::ForageableEpic[2] = {
	static_cast<int>(EForageableType::Heartdrop),
	static_cast<int>(EForageableType::DariCloves)
};