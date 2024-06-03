#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H

#include <string>
#include <chrono>
#include "json.hpp"

class PaliaOverlay; // Forward declaration

class ConfigHandler {
public:
    ConfigHandler(std::string configDirectory, std::string configFilePath);
    void SaveConfiguration();
    bool LoadConfiguration(PaliaOverlay* overlay);
    bool HasConfigurationChanged(PaliaOverlay* overlay) const;
    void UpdateConfiguration(PaliaOverlay* overlay);

private:
    std::string configDirectory;
    std::string configFilePath;
    std::chrono::steady_clock::time_point lastSaveTime;
    std::chrono::seconds saveInterval;
    nlohmann::json currentConfig;
    PaliaOverlay* Overlay;

    void EnsureDirectoryExists() const;
};

#endif
