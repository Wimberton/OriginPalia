#pragma once
#include <imgui.h>
#include <map>
#include <string>
#include <type_traits>

#include "Core/Configuration.h"
#include "Core/HotkeysManager.h"
#include "Core/DetourManager.h"

#include "ImGuiNotify.hpp"

#include <fonts/fonts.h>

namespace ImGui {

    inline bool isHotkeySelectorPopupOpen = false; // Hotkey selector popup flag

    IMGUI_API enum CheckboxHotkeyFlags {
        HOTKEY,
        DEFAULT
    };

    IMGUI_API void ShowHotkeyMenu(const std::string& actionName, const std::vector<std::pair<std::string, std::function<void()>>>& additionalMenuItems = {});

    // Function to get key name from key code
    IMGUI_API inline std::string GetHotkeyName(int key) {
        static std::map<int, std::string> keyNames = {
            {0x01, "Left Button"},
            {0x02, "Right Button"},
            {0x03, "Control-break"},
            {0x04, "Middle Button"},
            {0x05, "X1 Button"},
            {0x06, "X2 Button"},
            {0x08, "Backspace"},
            {0x09, "Tab"},
            {0x0C, "Clear"},
            {0x0D, "Enter"},
            {0x10, "Shift"},
            {0x11, "Ctrl"},
            {0x12, "Alt"},
            {0x13, "Pause"},
            {0x14, "Caps Lock"},
            {0x1B, "Esc"},
            {0x20, "Space"},
            {0x21, "Page Up"},
            {0x22, "Page Down"},
            {0x23, "End"},
            {0x24, "Home"},
            {0x25, "Left Arrow"},
            {0x26, "Up Arrow"},
            {0x27, "Right Arrow"},
            {0x28, "Down Arrow"},
            {0x29, "Select"},
            {0x2A, "Print"},
            {0x2B, "Execute"},
            {0x2C, "Print Screen"},
            {0x2D, "Insert"},
            {0x2E, "Delete"},
            {0x2F, "Help"},
            {0x30, "0"},
            {0x31, "1"},
            {0x32, "2"},
            {0x33, "3"},
            {0x34, "4"},
            {0x35, "5"},
            {0x36, "6"},
            {0x37, "7"},
            {0x38, "8"},
            {0x39, "9"},
            {0x41, "A"},
            {0x42, "B"},
            {0x43, "C"},
            {0x44, "D"},
            {0x45, "E"},
            {0x46, "F"},
            {0x47, "G"},
            {0x48, "H"},
            {0x49, "I"},
            {0x4A, "J"},
            {0x4B, "K"},
            {0x4C, "L"},
            {0x4D, "M"},
            {0x4E, "N"},
            {0x4F, "O"},
            {0x50, "P"},
            {0x51, "Q"},
            {0x52, "R"},
            {0x53, "S"},
            {0x54, "T"},
            {0x55, "U"},
            {0x56, "V"},
            {0x57, "W"},
            {0x58, "X"},
            {0x59, "Y"},
            {0x5A, "Z"},
            {0x60, "Num 0"},
            {0x61, "Num 1"},
            {0x62, "Num 2"},
            {0x63, "Num 3"},
            {0x64, "Num 4"},
            {0x65, "Num 5"},
            {0x66, "Num 6"},
            {0x67, "Num 7"},
            {0x68, "Num 8"},
            {0x69, "Num 9"},
            {0x6A, "Num *"},
            {0x6B, "Num +"},
            {0x6C, "Num Enter"},
            {0x6D, "Num -"},
            {0x6E, "Num ."},
            {0x6F, "Num /"},
            {0x70, "F1"},
            {0x71, "F2"},
            {0x72, "F3"},
            {0x73, "F4"},
            {0x74, "F5"},
            {0x75, "F6"},
            {0x76, "F7"},
            {0x77, "F8"},
            {0x78, "F9"},
            {0x79, "F10"},
            {0x7A, "F11"},
            {0x7B, "F12"},
            {0x7C, "F13"},
            {0x7D, "F14"},
            {0x7E, "F15"},
            {0x7F, "F16"},
            {0x80, "F17"},
            {0x81, "F18"},
            {0x82, "F19"},
            {0x83, "F20"},
            {0x84, "F21"},
            {0x85, "F22"},
            {0x86, "F23"},
            {0x87, "F24"},
            {0x90, "Num Lock"},
            {0x91, "Scroll Lock"},
            {0xA0, "Left Shift"},
            {0xA1, "Right Shift"},
            {0xA2, "Left Ctrl"},
            {0xA3, "Right Ctrl"},
            {0xA4, "Left Alt"},
            {0xA5, "Right Alt"},
            {0xA6, "Browser Back"},
            {0xA7, "Browser Forward"},
            {0xA8, "Browser Refresh"},
            {0xA9, "Browser Stop"},
            {0xAA, "Browser Search"},
            {0xAB, "Browser Favorites"},
            {0xAC, "Browser Home"},
            {0xAD, "Volume Mute"},
            {0xAE, "Volume Down"},
            {0xAF, "Volume Up"},
            {0xB0, "Next Track"},
            {0xB1, "Previous Track"},
            {0xB2, "Stop Media"},
            {0xB3, "Play/Pause Media"},
            {0xB4, "Start Mail"},
            {0xB5, "Select Media"},
            {0xB6, "Start Application 1"},
            {0xB7, "Start Application 2"},
            {0xBA, "OEM 1"},
            {0xBB, "OEM Plus"},
            {0xBC, "OEM Comma"},
            {0xBD, "OEM Minus"},
            {0xBE, "OEM Period"},
            {0xBF, "OEM 2"},
            {0xC0, "OEM 3"},
            {0xDB, "OEM 4"},
            {0xDC, "OEM 5"},
            {0xDD, "OEM 6"},
            {0xDE, "OEM 7"},
            {0xDF, "OEM 8"},
            {0xE2, "OEM 102"},
            {0xE5, "Process"},
            {0xE7, "Packet"},
            {0xF6, "Attn"},
            {0xF7, "CrSel"},
            {0xF8, "ExSel"},
            {0xF9, "Erase EOF"},
            {0xFA, "Play"},
            {0xFB, "Zoom"},
            {0xFE, "PA1"},
            {0xFF, "Clear"}
        };

        return keyNames.contains(key) ? keyNames[key] : "Unknown";
    }
    
    /**
     * 
     * @param label = name/id of checkbox
     * @param v = ptr bool
     * @param additionalMenuItems = optional additional menu items
     * @return button fired.
     */
    IMGUI_API bool Checkbox2(const char* label, bool* v, const std::vector<std::pair<std::string, std::function<void()>>>& additionalMenuItems = {});
    IMGUI_API bool Button2(const char* label, const ImVec2& size_arg = ImVec2(0,0), ImGuiButtonFlags flags = 0);

    void BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(-1.0f, -1.0f));
    void EndGroupPanel();
    void ColorPicker(const char* name, ImU32* color);
    void AddText(ImDrawList* drawList, const char* text, ImU32 textColor, ImVec2 screenPosition, ImU32 backgroundColor = 0x80000000);

    inline void CheckboxHelper(const char* label, bool* value, bool nextColumn, const char* tooltip, CheckboxHotkeyFlags hotkey_or_default = DEFAULT) {
        if (hotkey_or_default == CheckboxHotkeyFlags::DEFAULT) {
            if (Checkbox(label, value)) {
                Configuration::Save();
            }
        }
        else {
            if (Checkbox2(label, value)) {
                Configuration::Save();
            }
        }
        if (tooltip && IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            SetTooltip("%s", tooltip);
        }
        if (nextColumn) {
            TableNextColumn();
        }
    }

    inline void CheckboxHelper(const char* label, bool* value, const char* tooltip, CheckboxHotkeyFlags hotkey_or_default) {
        CheckboxHelper(label, value, false, tooltip, hotkey_or_default);
    }

    inline void CheckboxHelper(const char* label, bool* value, const char* tooltip) {
        CheckboxHelper(label, value, false, tooltip, CheckboxHotkeyFlags::DEFAULT);
    }

    inline void CheckboxHelper(const char* label, bool* value, bool nextColumn, CheckboxHotkeyFlags hotkey_or_default) {
        CheckboxHelper(label, value, nextColumn, nullptr, hotkey_or_default);
    }

    inline void CheckboxHelper(const char* label, bool* value, bool nextColumn) {
        CheckboxHelper(label, value, nextColumn, nullptr, CheckboxHotkeyFlags::DEFAULT);
    }

    inline void CheckboxHelper(const char* label, bool* value, CheckboxHotkeyFlags hotkey_or_default) {
        CheckboxHelper(label, value, false, nullptr, hotkey_or_default);
    }

    inline void CheckboxHelper(const char* label, bool* value) {
        CheckboxHelper(label, value, false, nullptr, CheckboxHotkeyFlags::DEFAULT);
    }

    template <typename... Args>
    void CheckboxDispatcher(const char* label, bool* value, Args&&... args) {
        CheckboxHelper(label, value, std::forward<Args>(args)...);
    }

    template<typename T>
    void SliderHelper(const char* label, T* value, T v_min, T v_max, const char* format, bool nextColumn, const char* tooltip) {
        bool valueChanged = false;
        if constexpr (std::is_same_v<T, float>) {
            valueChanged = ImGui::SliderFloat(label, value, v_min, v_max, format);
        }
        else if constexpr (std::is_same_v<T, int>) {
            valueChanged = ImGui::SliderInt(label, value, v_min, v_max);
        }
        else if constexpr (std::is_same_v<T, double>) {
            valueChanged = ImGui::SliderScalar(label, ImGuiDataType_Double, value, &v_min, &v_max, format);
        }
        if (tooltip && IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            SetTooltip("%s", tooltip);
        }
        if (valueChanged) {
            Configuration::Save();
        }
        if (nextColumn) {
            TableNextColumn();
        }
    }

    template<typename T>
    void SliderHelper(const char* label, T* value, T v_min, T v_max, const char* format, const char* tooltip) {
        SliderHelper(label, value, v_min, v_max, format, false, tooltip);
    }

    template<typename T>
    void SliderHelper(const char* label, T* value, T v_min, T v_max, const char* format, bool nextColumn) {
        SliderHelper(label, value, v_min, v_max, format, nextColumn, nullptr);
    }

    template<typename T>
    void SliderHelper(const char* label, T* value, T v_min, T v_max, const char* format) {
        SliderHelper(label, value, v_min, v_max, format, false, nullptr);
    }

    template<typename T, typename... Args>
    void SliderDispatcher(const char* label, T* value, T v_min, T v_max, const char* format, Args&&... args) {
        SliderHelper(label, value, v_min, v_max, format, std::forward<Args>(args)...);
    }

    /*
    * @brief Renders a feature as a checkbox, slider, button, or supporter-only lock icon with support link.
    *
    * If supporter features are enabled, it shows the specified widget with the given feature name.
    * Otherwise, it shows a lock icon, the feature name, a supporter-only message, and a clickable link to support.
    *
    * @param featureName Name of the feature to be displayed.
    * @param featureFlag Pointer to the boolean flag controlling the feature.
    * @param tooltip Tooltip text to be displayed when the widget or lock icon is hovered.
    * @param type The type of widget to render: "checkbox", "slider", or "button".
    * @param sliderMin Minimum value for the slider (if type is "slider").
    * @param sliderMax Maximum value for the slider (if type is "slider").
    * @param sliderFormat Format string for the slider value (if type is "slider").
    */
    inline void RenderSupporterFeature(const char* featureName, void* featureFlag, const char* tooltip, const char* type, float sliderMin = 0.0f, float sliderMax = 1.0f, const char* sliderFormat = "%.3f", CheckboxHotkeyFlags hotkey_or_default = CheckboxHotkeyFlags::DEFAULT) {
#ifdef ENABLE_SUPPORTER_FEATURES
        if (strcmp(type, "checkbox") == 0) {
            CheckboxDispatcher(featureName, static_cast<bool*>(featureFlag), tooltip, hotkey_or_default);
        } else if (strcmp(type, "slider") == 0) {
            SetNextItemWidth(200.0f);
            SliderDispatcher(featureName, static_cast<float*>(featureFlag), sliderMin, sliderMax, sliderFormat, tooltip);
        } else if (strcmp(type, "button") == 0) {
            if (Button(featureName)) {
                *static_cast<bool*>(featureFlag) = true;
                Configuration::Save();
            }
            if (tooltip && IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                SetTooltip("%s", tooltip);
            }
        }
#else
        if (strcmp(type, "checkbox") == 0 || strcmp(type, "button") == 0) {
            *((bool*)featureFlag) = false;
        }
        ImGui::BeginGroup();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::Text(ICON_FA_LOCK " %s", featureName);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("Unavailable in this build of OriginPalia");
        }
        ImGui::PopStyleColor();
        ImGui::EndGroup();
        ImGui::Spacing();
#endif
    }
}

#define IMGUI_SUPPORTER_FEATURE(featureName, featureFlag, tooltip, type, ...) \
    ImGui::RenderSupporterFeature(featureName, featureFlag, tooltip, type, ##__VA_ARGS__);

#define IMGUI_SETUP_TABLE(table_name, num_columns)                       \
    ImGui::BeginTable(table_name, num_columns);

#define IMGUI_TABLE_SETUP_COLUMN(name, flags, width)                     \
    ImGui::TableSetupColumn(name, flags, width);

/*
* @brief Will automatically add column header(s). Simply add an empty string to add a blank column as a spacer.
*/
#define IMGUI_ADD_HEADERS(...)                                           \
    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);                     \
    {                                                                    \
        const char* headers[] = {__VA_ARGS__};                           \
        for (int i = 0; i < sizeof(headers) / sizeof(headers[0]); ++i) { \
            ImGui::TableNextColumn();                                    \
            ImGui::Text(headers[i]);                                     \
        }                                                                \
    }

/*
* @brief Adds a row & includes TableNextRow, TableNextColumn with proper separation between checkboxes & colorpicker.
* If your ColorPicker isn't in the right column, make sure you've separated it in the arguments.
*
* @param item_name = name of row
* @param checkbox_expr = your CheckBox expression.
* @param color_picker_expr = your ColorPicker expression.
*/
#define IMGUI_ADD_ROW(item_name, checkbox_expr, color_picker_expr)       \
    ImGui::TableNextRow();                                               \
    ImGui::TableNextColumn();                                            \
    ImGui::Text(item_name);                                              \
    ImGui::TableNextColumn();                                            \
    checkbox_expr;                                                       \
    ImGui::TableNextColumn();                                            \
    color_picker_expr;

#define IMGUI_ADD_BLANK_ROW(...) \
    ImGui::TableNextRow(); \
    ImGui::TableNextColumn(); \
    __VA_ARGS__

#define IMGUI_BUTTON(label, ...)                                         \
    if (ImGui::Button2(label)) {                                         \
        __VA_ARGS__                                                      \
    }

/*
* Creates a checkbox and saves config. Note: `nextColumn` isn't required if you want to add a tooltip.
*
* @param label = checkbox name
* @param *value = boolean to modify
* @param (optional) nextColumn = add a TableNextColumn after the checkbox.
* @param (optional) tooltip = add a tooltip for the checkbox.
*/
#define IMGUI_CHECKBOX(...) \
    ImGui::CheckboxDispatcher(__VA_ARGS__);

/*
* Creates a slider and saves config. Detects type to use the appropriate ImGui function.
*
* @param label = slider name
* @param *value = float or int to modify
* @param v_min = minimum value
* @param v_max = maximum value
* @param format = format string for the slider value
* @param (optional) nextColumn = add a TableNextColumn after the slider.
* @param (optional) tooltip = add a tooltip for the slider.
*/
#define IMGUI_SLIDER(...) \
    ImGui::SliderDispatcher(__VA_ARGS__);

#define IMGUI_COLORPICKER(label, color)                                  \
    ImGui::ColorPicker(label, color)

#define IMGUI_TABLENEXTCOLUMN()                                          \
    ImGui::TableNextColumn();

#define IMGUI_END_TABLE()                                                \
    ImGui::EndTable();

inline void GetWidgetForPreview(int SelectedColor) {
    static bool checkboxState = false;
    static float sliderValue = 0.5f;
    switch (SelectedColor) {
        // TEXT
    case ImGuiCol_Text:
        ImGui::Text("Lorem ipsum dolor sit amet, consectetur adipiscing.");
        break;
    case ImGuiCol_TextDisabled:
        ImGui::Text("Lorem ipsum dolor sit amet, consectetur adipiscing.");
        break;
    case ImGuiCol_ChildBg:
        ImGui::BeginChild("Child", ImVec2(100, 100), true);
        ImGui::Text("Child window");
        ImGui::EndChild();
        break;
    case ImGuiCol_PopupBg:
        if (ImGui::BeginPopup("Popup##SamplePopup")) {
            ImGui::Text("Popup content.");
            ImGui::EndPopup();
        }
        if (ImGui::Button("Open Popup"))
            ImGui::OpenPopup("Popup##SamplePopup");
        break;
    case ImGuiCol_Button:
        ImGui::Button("Button");
        break;
    case ImGuiCol_ButtonActive:
        ImGui::Button("Button");
        break;
    case ImGuiCol_ButtonHovered:
        ImGui::Button("Button");
        break;
    case ImGuiCol_CheckMark:
        ImGui::Checkbox("Checkbox", &checkboxState);
        break;
    case ImGuiCol_SliderGrab:
        ImGui::SliderFloat("Slider", &sliderValue, 0.0f, 1.0f);
        break;
    case ImGuiCol_SliderGrabActive:
        ImGui::SliderFloat("Slider", &sliderValue, 0.0f, 1.0f);
        break;
    case ImGuiCol_Separator:
        ImGui::Separator();
        break;
    case ImGuiCol_SeparatorActive:
        ImGui::Separator();
        break;
    case ImGuiCol_SeparatorHovered:
        ImGui::Separator();
        break;
    default:
        ImGui::Text("No preview available for this color.");
        break;
    }
}

inline std::map<int, ImVec4> previousColors = {};

inline std::map<int, std::string> GuiColors = {
    {ImGuiCol_Text, "Text"},
    {ImGuiCol_TextDisabled, "TextDisabled"}, // TEXT = 0,1

    {ImGuiCol_WindowBg, "WindowBg"},
    {ImGuiCol_ChildBg, "ChildBg"},
    {ImGuiCol_PopupBg, "PopupBg"}, // 2,3,4

    {ImGuiCol_Border, "Border"},
    {ImGuiCol_BorderShadow, "BorderShadow"}, // Borders = 5,6

    {ImGuiCol_FrameBg, "FrameBg"},
    {ImGuiCol_FrameBgHovered, "FrameBgHovered"},
    {ImGuiCol_FrameBgActive, "FrameBgActive"}, // FrameBg = 7,8,9

    {ImGuiCol_TitleBg, "TitleBg"},
    {ImGuiCol_TitleBgActive, "TitleBgActive"},
    {ImGuiCol_TitleBgCollapsed, "TitleBgCollapsed"}, // TitleBg = 10,11,12

    {ImGuiCol_MenuBarBg, "MenuBarBg"}, // MenuBarBg = 13

    {ImGuiCol_ScrollbarBg, "ScrollbarBg"},
    {ImGuiCol_ScrollbarGrab, "ScrollbarGrab"},
    {ImGuiCol_ScrollbarGrabHovered, "ScrollbarGrabHovered"},
    {ImGuiCol_ScrollbarGrabActive, "ScrollbarGrabActive"}, // Scrollbars = 14,15,16,17

    {ImGuiCol_CheckMark, "CheckMark"}, // Checkmark = 18

    {ImGuiCol_SliderGrab, "SliderGrab"},
    {ImGuiCol_SliderGrabActive, "SliderGrabActive"}, // Sliders = 19,20

    {ImGuiCol_Button, "Button"},
    {ImGuiCol_ButtonHovered, "ButtonHovered"},
    {ImGuiCol_ButtonActive, "ButtonActive"}, // Buttons = 21,22,23

    {ImGuiCol_Header, "Header"},
    {ImGuiCol_HeaderHovered, "HeaderHovered"},
    {ImGuiCol_HeaderActive, "HeaderActive"}, // Headers = 24,25,26

    {ImGuiCol_Separator, "Separator"},
    {ImGuiCol_SeparatorHovered, "SeparatorHovered"},
    {ImGuiCol_SeparatorActive, "SeparatorActive"}, // Separator = 27,28,29

    {ImGuiCol_ResizeGrip, "ResizeGrip"},
    {ImGuiCol_ResizeGripHovered, "ResizeGripHovered"},
    {ImGuiCol_ResizeGripActive, "ResizeGripActive"}, // ResizeGrip = 30,31,32

    {ImGuiCol_Tab, "Tab"},
    {ImGuiCol_TabHovered, "TabHovered"},
    {ImGuiCol_TabActive, "TabActive"},
    {ImGuiCol_TabUnfocused, "TabUnfocused"},
    {ImGuiCol_TabUnfocusedActive, "TabUnfocusedActive"}, // Tabs = 34,33,35,37,38

    {ImGuiCol_PlotLines, "PlotLines"},
    {ImGuiCol_PlotLinesHovered, "PlotLinesHovered"},
    {ImGuiCol_PlotHistogram, "PlotHistogram"},
    {ImGuiCol_PlotHistogramHovered, "PlotHistogramHovered"}, // PlotLines = 40,41,42,43

    {ImGuiCol_TableHeaderBg, "TableHeaderBg"},
    {ImGuiCol_TableBorderStrong, "TableBorderStrong"},
    {ImGuiCol_TableBorderLight, "TableBorderLight"},
    {ImGuiCol_TableRowBg, "TableRowBg"},
    {ImGuiCol_TableRowBgAlt, "TableRowBgAlt"}, // Tables = 44,45,46,47,48

    {ImGuiCol_TextSelectedBg, "TextSelectedBg"}, // 50

    {ImGuiCol_DragDropTarget, "DragDropTarget"}, // 51

    {ImGuiCol_NavHighlight, "NavHighlight"},
    {ImGuiCol_NavWindowingHighlight, "NavWindowingHighlight"},
    {ImGuiCol_NavWindowingDimBg, "NavWindowingDimBg"}, // 52,53,54

    {ImGuiCol_ModalWindowDimBg, "ModalWindowDimBg"} // 55
};