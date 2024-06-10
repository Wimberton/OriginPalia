#pragma once

#include <unordered_map>
#include <string>
#include <vector>

struct HotkeyControl {
    std::string label;
    std::string buffer;
    std::string prevBuffer;
    bool enabled;
    bool isFocused;
    int vkCode;
};

extern std::unordered_map<int, std::string> keyMap;
extern std::unordered_map<std::string, size_t> ControlIndex;
extern std::vector<HotkeyControl> HotkeyControls;

bool TextChanged(HotkeyControl& control);
void DetectSpecialKeys(HotkeyControl& control);
