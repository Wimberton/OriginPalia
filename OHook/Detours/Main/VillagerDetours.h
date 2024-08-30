#pragma once

#include <SDK.hpp>

using namespace SDK;

class VillagerDetours final {
public:
	static void SetupDetours(AValeriaCharacter* VC, AValeriaPlayerController* VPC);
	static void GiftVillagers();
	static void Tick(const UObject* Context, UFunction* Function, void* Parms);

	static inline std::vector<std::string> LastReport;

private:
	static void Func_IsAwareOfGiftingPreference(void* Context, void* TheStack, void* Result);

	static bool InGiftLoop;
	static bool WaitingForDialogue;
	static bool WaitingForClose;
};

inline bool VillagerDetours::InGiftLoop = false;
inline bool VillagerDetours::WaitingForDialogue = false;
inline bool VillagerDetours::WaitingForClose = false;