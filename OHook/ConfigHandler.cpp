#include <fstream>
#include <filesystem>
#include "ConfigHandler.h"
#include "PaliaOverlay.h"

using json = nlohmann::json;

ConfigHandler::ConfigHandler(std::string configDirectory, std::string configFilePath) : configDirectory(std::move(configDirectory)), configFilePath(std::move(configFilePath)), saveInterval(3), Overlay(nullptr) {
    lastSaveTime = std::chrono::steady_clock::now();
}

void ConfigHandler::EnsureDirectoryExists() const {
    std::filesystem::path dir(configDirectory);
    if (!exists(dir)) {
        create_directories(dir);
    }
}

void ConfigHandler::SaveConfiguration() {
    EnsureDirectoryExists();

    json j = Overlay->settings;

    std::ofstream configFile(configDirectory + configFilePath);
    configFile << j.dump(4);

    // Update the currentConfig with the new values
    currentConfig = j;
}

bool ConfigHandler::LoadConfiguration(PaliaOverlay* overlay) {
    if (!std::filesystem::exists(configDirectory + configFilePath)) {
        return false;
    }

    std::ifstream configFile(configDirectory + configFilePath);
    json j;
    configFile >> j;

    Overlay = overlay;
    Overlay->settings = j.get<PaliaOverlay::Settings>(); // Load Settings

    // Load the configuration into currentConfig
    currentConfig = j;

    return true;
}

bool ConfigHandler::HasConfigurationChanged(PaliaOverlay* overlay) const {
    if (overlay) {
        json j = overlay->settings;
        return j != currentConfig;
    }
    return false;
}

void ConfigHandler::UpdateConfiguration(PaliaOverlay* overlay) {
    auto now = std::chrono::steady_clock::now();
    if (now - lastSaveTime >= saveInterval && HasConfigurationChanged(overlay)) {
        SaveConfiguration();
        lastSaveTime = now;

        // After saving, update currentConfig
        if (overlay) {
            json j = overlay->settings;
            currentConfig = j;
        }
    }
}
