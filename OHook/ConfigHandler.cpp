#include "ConfigHandler.h"

using json = nlohmann::json;

ConfigHandler::ConfigHandler(const std::string& configDir, const std::string& configFile)
    : configDirectory(configDir), configFilePath(configFile), saveInterval(3) {
    lastSaveTime = std::chrono::steady_clock::now();
}

void ConfigHandler::EnsureDirectoryExists() {
    fs::path dir(configDirectory);
    if (!fs::exists(dir)) {
        fs::create_directories(dir);
    }
}

void ConfigHandler::SaveConfiguration(PaliaOverlay* Overlay) {
    EnsureDirectoryExists();

    json j;
    j["Enable Silent Aimbot"] = Overlay->bEnableSilentAimbot;
    j["Enable Legacy Aimbot"] = Overlay->bEnableAimbot;
    j["Teleport to Targeted"] = Overlay->bTeleportToTargeted;
    j["Teleport to Waypoint"] = Overlay->bEnableWaypointTeleport;
    j["Avoid Teleporting To Targeted Players"] = Overlay->bAvoidTeleportingToPlayers;
    j["Teleport Dropped Loot to Player"] = Overlay->bEnableLootbagTeleportation;
    j["Enable ESP"] = Overlay->bEnableESP;
    j["ESP Text Scale"] = Overlay->ESPTextScale;
    j["Limit Distance"] = Overlay->bEnableESPCulling;
    j["Distance"] = Overlay->CullDistance;
    j["Enable InteliAim Circle"] = Overlay->bDrawFOVCircle;
    j["InteliAim Radius"] = Overlay->FOVRadius;
    j["Capture fishing spot"] = Overlay->bCaptureFishingSpot;
    j["Override fishing spot"] = Overlay->bOverrideFishingSpot;
    j["Custom Walk Speed"] = Overlay->CustomWalkSpeed;
    j["Custom Sprint Speed Multiplier"] = Overlay->CustomSprintSpeedMultiplier;
    j["Custom Climbing Speed"] = Overlay->CustomClimbingSpeed;
    j["Custom Gliding Speed"] = Overlay->CustomGlidingSpeed;
    j["Custom Gliding Fall Speed"] = Overlay->CustomGlidingFallSpeed;
    j["Custom Jump Velocity"] = Overlay->CustomJumpVelocity;
    j["Custom Max Step Height"] = Overlay->CustomMaxStepHeight;
    j["Place Items Anywhere"] = Overlay->bPlaceAnywhere;
    j["ESP_Players"] = Overlay->Singles[static_cast<int>(EOneOffs::Player)];

    std::ofstream configFile(configDirectory + configFilePath);
    configFile << j.dump(4);
}

bool ConfigHandler::LoadConfiguration(PaliaOverlay* Overlay) {
    if (!fs::exists(configDirectory + configFilePath)) {
        return false;
    }

    std::ifstream configFile(configDirectory + configFilePath);
    json j;
    configFile >> j;

    Overlay->bEnableSilentAimbot = j["Enable Silent Aimbot"].get<bool>();
    Overlay->bEnableAimbot = j["Enable Legacy Aimbot"].get<bool>();
    Overlay->bTeleportToTargeted = j["Teleport to Targeted"].get<bool>();
    Overlay->bEnableWaypointTeleport = j["Teleport to Waypoint"].get<bool>();
    Overlay->bAvoidTeleportingToPlayers = j["Avoid Teleporting To Targeted Players"].get<bool>();
    Overlay->bEnableLootbagTeleportation = j["Teleport Dropped Loot to Player"].get<bool>();
    Overlay->bEnableESP = j["Enable ESP"].get<bool>();
    Overlay->ESPTextScale = j["ESP Text Scale"].get<float>();
    Overlay->bEnableESPCulling = j["Limit Distance"].get<bool>();
    Overlay->CullDistance = j["Distance"].get<int>();
    Overlay->bDrawFOVCircle = j["Enable InteliAim Circle"].get<bool>();
    Overlay->FOVRadius = j["InteliAim Radius"].get<float>();
    Overlay->bCaptureFishingSpot = j["Capture fishing spot"].get<bool>();
    Overlay->bOverrideFishingSpot = j["Override fishing spot"].get<bool>();
    Overlay->CustomWalkSpeed = j["Custom Walk Speed"].get<float>();
    Overlay->CustomSprintSpeedMultiplier = j["Custom Sprint Speed Multiplier"].get<float>();
    Overlay->CustomClimbingSpeed = j["Custom Climbing Speed"].get<float>();
    Overlay->CustomGlidingSpeed = j["Custom Gliding Speed"].get<float>();
    Overlay->CustomGlidingFallSpeed = j["Custom Gliding Fall Speed"].get<float>();
    Overlay->CustomJumpVelocity = j["Custom Jump Velocity"].get<float>();
    Overlay->CustomMaxStepHeight = j["Custom Max Step Height"].get<float>();
    Overlay->bPlaceAnywhere = j["Place Items Anywhere"].get<bool>();
    Overlay->Singles[static_cast<int>(EOneOffs::Player)] = j["ESP_Players"].get<bool>();

    return true;
}

void ConfigHandler::UpdateConfiguration(PaliaOverlay* Overlay) {
    auto now = std::chrono::steady_clock::now();
    if (now - lastSaveTime >= saveInterval) {
        SaveConfiguration(Overlay);
        lastSaveTime = now;
    }
}

