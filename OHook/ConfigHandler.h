#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H

#include <string>
#include <chrono>
#include <fstream>
#include <filesystem>
#include "json.hpp"
#include "PaliaOverlay.h"

namespace fs = std::filesystem;

class ConfigHandler {
public:
    ConfigHandler(const std::string& configDirectory, const std::string& configFilePath);
    void SaveConfiguration(PaliaOverlay* Overlay);
    bool LoadConfiguration(PaliaOverlay* Overlay);
    void UpdateConfiguration(PaliaOverlay* Overlay);

private:
    std::string configDirectory;
    std::string configFilePath;
    std::chrono::steady_clock::time_point lastSaveTime;
    std::chrono::seconds saveInterval;

    void EnsureDirectoryExists();
};

#endif // CONFIG_HANDLER_H
