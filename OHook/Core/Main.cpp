#include "Main.h"
#include "Configuration.h"
#include "HotkeysManager.h"
#include "Misc/Utils.h"

#include "Handlers/ActorHandler.h"
#include "Handlers/ToolHandler.h"
#include "Handlers/WidgetHandler.h"
#include "HotkeysManager.h"

#include "Detours/Main/AimDetours.h"
#include "Detours/Main/BlackMarketDetours.h"
#include "Detours/Main/CookingDetours.h"
#include "Detours/Main/FishingDetours.h"
#include "Detours/Main/HousingDetours.h"
#include "Detours/Main/HUDDetours.h"
#include "Detours/Main/MovementDetours.h"
#include "Detours/Main/PlayerTweaksDetours.h"
#include "Detours/Main/StoreDetours.h"
#include "Detours/Main/TeleportDetours.h"
#include "Detours/Main/ToolBrickerDetours.h"
#include "Detours/Main/VillagerDetours.h"
#include "Detours/Main/WidgetDetours.h"

#ifdef ENABLE_SUPPORTER_FEATURES
#include "Detours/Supporters/AutoTools/AnimalHunting.h"
#include "Detours/Supporters/AutoTools/BugCatching.h"
#include "Detours/Supporters/AutoTools/ToolSwinging.h"
#include "Detours/Supporters/Entitlements/LandscapeDetours.h"
#include "Detours/Supporters/Entitlements/OutfitsDetours.h"
#endif



void Main::Start() {
    // Load Settings
    Configuration::Load();
    HotkeysManager::InitHotkeys();
    ActorHandler::Start();
    SetupDetours();
}

void Main::Stop() {
    DetourManager::RemoveInstances();
    DetourManager::RemoveFunctionDetours();
    ActorHandler::Stop();
}

void Main::SetupDetours() {
    auto VC = GetValeriaCharacter();
    auto VPC = GetValeriaController();
    if (!VPC || !VC) {
        // Small delay per attempt (Needed)
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        SetupDetours();
        return;
    }

    ActorHandler::SetupDetours(VC,VPC);
    ToolHandler::SetupDetours(VC,VPC);
    WidgetHandler::SetupDetours(VC,VPC);
    HotkeysManager::SetupDetours(VC,VPC);

    AimDetours::SetupDetours(VC,VPC);
    BlackMarketDetours::SetupDetours(VC,VPC);
    CookingDetours::SetupDetours(VC,VPC);
    FishingDetours::SetupDetours(VC,VPC);
    HousingDetours::SetupDetours(VC,VPC);
    HUDDetours::SetupDetours(VC,VPC);
    MovementDetours::SetupDetours(VC,VPC);
    PlayerTweaksDetours::SetupDetours(VC,VPC);
    StoreDetours::SetupDetours(VC,VPC);
    TeleportDetours::SetupDetours(VC,VPC);
    ToolBrickerDetours::SetupDetours(VC,VPC);
    VillagerDetours::SetupDetours(VC,VPC);
    WidgetDetours::SetupDetours(VC,VPC);

    #ifdef ENABLE_SUPPORTER_FEATURES
    AnimalHunting::SetupDetours(VC,VPC);
    BugCatching::SetupDetours(VC,VPC);
    // Disabled for now as is seems to be patched
    // LandscapeDetours::SetupDetours(VC,VPC);
    OutfitsDetours::SetupDetours(VC,VPC);
    ToolSwinging::SetupDetours(VC,VPC);
    #endif
}