#include "HotkeyManager.h"
#include <windows.h>

std::vector<HotkeyControl> HotkeyControls = {
    {"ESP", std::string(128, '\0'), std::string(128, '\0'), false, false, 0},
    {"InteliAim", std::string(128, '\0'), std::string(128, '\0'), false, false, 0},
    {"SilentAimbot", std::string(128, '\0'), std::string(128, '\0'), false, false, 0},
    {"LegacyAimbot", std::string(128, '\0'), std::string(128, '\0'), false, false, 0},
    {"NoClip", std::string(128, '\0'), std::string(128, '\0'), false, false, 0},
    {"TeleportTargetted", std::string(128, '\0'), std::string(128, '\0'), false, false, 0},
    // Add more controls as needed
};

std::unordered_map<std::string, size_t> ControlIndex = {
    {"ESP", 0},
    {"InteliAim", 1},
    {"SilentAimbot", 2},
    {"LegacyAimbot", 3},
    {"NoClip", 4},
    {"TeleportTargetted", 5},
    // Add more controls as needed
};

bool TextChanged(HotkeyControl& control) {
    if (control.buffer != control.prevBuffer) {
        control.prevBuffer = control.buffer;
        return true;
    }
    return false;
}

void DetectSpecialKeys(HotkeyControl& control) {
    for (const auto& [vkKey, name] : keyMap) {
        if (GetAsyncKeyState(vkKey) & 0x8000) { // Check if the key is currently pressed
            control.buffer = name;
            control.vkCode = vkKey;
            break;
        }
    }
}

// TODO: This needs to be sanitized, remove conflicting ones, add missing ones. (Insert removed since overlay hardcoded for now)
std::unordered_map<int, std::string> keyMap = {
    {VK_CANCEL, "CANCEL"},
    {VK_MBUTTON, "MBUTTON"},
    {VK_XBUTTON1, "XBUTTON1"},
    {VK_XBUTTON2, "XBUTTON2"},
    {VK_BACK, "BACK"},
    {VK_TAB, "TAB"},
    {VK_CLEAR, "CLEAR"},
    {VK_RETURN, "RETURN"},
    {VK_SHIFT, "SHIFT"},
    {VK_CONTROL, "CONTROL"},
    {VK_MENU, "MENU"},
    {VK_PAUSE, "PAUSE"},
    {VK_CAPITAL, "CAPITAL"},
    {VK_ESCAPE, "ESCAPE"},
    {VK_SPACE, "SPACE"},
    {VK_PRIOR, "PAGE UP"},
    {VK_NEXT, "PAGE DOWN"},
    {VK_END, "END"},
    {VK_HOME, "HOME"},
    {VK_LEFT, "LEFT ARROW"},
    {VK_UP, "UP ARROW"},
    {VK_RIGHT, "RIGHT ARROW"},
    {VK_DOWN, "DOWN ARROW"},
    {VK_SELECT, "SELECT"},
    {VK_PRINT, "PRINT"},
    {VK_EXECUTE, "EXECUTE"},
    {VK_SNAPSHOT, "PRINT SCREEN"},
    {VK_DELETE, "DELETE"},
    {VK_HELP, "HELP"},
    {VK_APPS, "APPS"},
    {VK_SLEEP, "SLEEP"},
    {VK_NUMPAD0, "NUMPAD 0"},
    {VK_NUMPAD1, "NUMPAD 1"},
    {VK_NUMPAD2, "NUMPAD 2"},
    {VK_NUMPAD3, "NUMPAD 3"},
    {VK_NUMPAD4, "NUMPAD 4"},
    {VK_NUMPAD5, "NUMPAD 5"},
    {VK_NUMPAD6, "NUMPAD 6"},
    {VK_NUMPAD7, "NUMPAD 7"},
    {VK_NUMPAD8, "NUMPAD 8"},
    {VK_NUMPAD9, "NUMPAD 9"},
    {VK_MULTIPLY, "MULTIPLY"},
    {VK_ADD, "ADD"},
    {VK_SEPARATOR, "SEPARATOR"},
    {VK_SUBTRACT, "SUBTRACT"},
    {VK_DECIMAL, "DECIMAL"},
    {VK_DIVIDE, "DIVIDE"},
    {VK_F1, "F1"},
    {VK_F2, "F2"},
    {VK_F3, "F3"},
    {VK_F4, "F4"},
    {VK_F5, "F5"},
    {VK_F6, "F6"},
    {VK_F7, "F7"},
    {VK_F8, "F8"},
    {VK_F9, "F9"},
    {VK_F10, "F10"},
    {VK_F11, "F11"},
    {VK_F12, "F12"},
    {VK_F13, "F13"},
    {VK_F14, "F14"},
    {VK_F15, "F15"},
    {VK_F16, "F16"},
    {VK_F17, "F17"},
    {VK_F18, "F18"},
    {VK_F19, "F19"},
    {VK_F20, "F20"},
    {VK_F21, "F21"},
    {VK_F22, "F22"},
    {VK_F23, "F23"},
    {VK_F24, "F24"},
    {VK_NUMLOCK, "NUM LOCK"},
    {VK_SCROLL, "SCROLL LOCK"},
    {VK_LSHIFT, "LEFT SHIFT"},
    {VK_RSHIFT, "RIGHT SHIFT"},
    {VK_LCONTROL, "LEFT CONTROL"},
    {VK_RCONTROL, "RIGHT CONTROL"},
    {VK_LMENU, "LEFT ALT"},
    {VK_RMENU, "RIGHT ALT"},
    {VK_BROWSER_BACK, "BROWSER BACK"},
    {VK_BROWSER_FORWARD, "BROWSER FORWARD"},
    {VK_BROWSER_REFRESH, "BROWSER REFRESH"},
    {VK_BROWSER_STOP, "BROWSER STOP"},
    {VK_BROWSER_SEARCH, "BROWSER SEARCH"},
    {VK_BROWSER_FAVORITES, "BROWSER FAVORITES"},
    {VK_BROWSER_HOME, "BROWSER HOME"},
    {VK_VOLUME_MUTE, "VOLUME MUTE"},
    {VK_VOLUME_DOWN, "VOLUME DOWN"},
    {VK_VOLUME_UP, "VOLUME UP"},
    {VK_MEDIA_NEXT_TRACK, "MEDIA NEXT TRACK"},
    {VK_MEDIA_PREV_TRACK, "MEDIA PREV TRACK"},
    {VK_MEDIA_STOP, "MEDIA STOP"},
    {VK_MEDIA_PLAY_PAUSE, "MEDIA PLAY PAUSE"},
    {VK_LAUNCH_MAIL, "LAUNCH MAIL"},
    {VK_LAUNCH_MEDIA_SELECT, "LAUNCH MEDIA SELECT"},
    {VK_LAUNCH_APP1, "LAUNCH APP1"},
    {VK_LAUNCH_APP2, "LAUNCH APP2"},
    {VK_OEM_1, "OEM 1"},
    {VK_OEM_PLUS, "OEM PLUS"},
    {VK_OEM_COMMA, "OEM COMMA"},
    {VK_OEM_MINUS, "OEM MINUS"},
    {VK_OEM_PERIOD, "OEM PERIOD"},
    {VK_OEM_2, "OEM 2"},
    {VK_OEM_3, "OEM 3"},
    {VK_OEM_4, "OEM 4"},
    {VK_OEM_5, "OEM 5"},
    {VK_OEM_6, "OEM 6"},
    {VK_OEM_7, "OEM 7"},
    {VK_OEM_8, "OEM 8"},
    {VK_OEM_102, "OEM 102"},
    {VK_PROCESSKEY, "PROCESS KEY"},
    {VK_PACKET, "PACKET"},
    {VK_ATTN, "ATTN"},
    {VK_CRSEL, "CRSEL"},
    {VK_EXSEL, "EXSEL"},
    {VK_EREOF, "EREOF"},
    {VK_PLAY, "PLAY"},
    {VK_ZOOM, "ZOOM"},
    {VK_NONAME, "NONAME"},
    {VK_PA1, "PA1"},
    {VK_OEM_CLEAR, "OEM CLEAR"}
    // Add more VK keys as needed
};