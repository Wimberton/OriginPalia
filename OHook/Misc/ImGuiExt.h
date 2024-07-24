#pragma once
#include <imgui.h>
#include <map>
#include <string>
#include <type_traits>

#include "Configuration.h"

namespace ImGui {
    void BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(-1.0f, -1.0f));
    void EndGroupPanel();
    void ColorPicker(const char* name, ImU32* color);
    void AddText(ImDrawList* drawList, const char* text, ImU32 textColor, ImVec2 screenPosition, ImU32 backgroundColor = 0x80000000);
}

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

/*
* Creates a checkbox and saves config. Note: `nextColumn` isn't required if you want to add a tooltip.
*
* @param label = checkbox name
* @param *value = boolean to modify
* @param (optional) nextColumn = add a TableNextColumn after the checkbox.
* @param (optional) tooltip = add a tooltip for the checkbox.
*/
#define IMGUI_CHECKBOX(...) \
    CheckboxDispatcher(__VA_ARGS__);

inline void CheckboxHelper(const char* label, bool* value, bool nextColumn, const char* tooltip) {
    if (ImGui::Checkbox(label, value)) {
        Configuration::Save();
    }
    if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("%s", tooltip);
    }
    if (nextColumn) {
        ImGui::TableNextColumn();
    }
}

inline void CheckboxHelper(const char* label, bool* value, const char* tooltip) {
    CheckboxHelper(label, value, false, tooltip);
}

inline void CheckboxHelper(const char* label, bool* value, bool nextColumn) {
    CheckboxHelper(label, value, nextColumn, nullptr);
}

inline void CheckboxHelper(const char* label, bool* value) {
    CheckboxHelper(label, value, false, nullptr);
}

template <typename... Args>
void CheckboxDispatcher(const char* label, bool* value, Args&&... args) {
    CheckboxHelper(label, value, std::forward<Args>(args)...);
}

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
    SliderDispatcher(__VA_ARGS__);

template<typename T>
inline void SliderHelper(const char* label, T* value, T v_min, T v_max, const char* format, bool nextColumn, const char* tooltip) {
    bool valueChanged = false;
    if constexpr (std::is_same_v<T, float>) {
        valueChanged = ImGui::SliderFloat(label, value, v_min, v_max, format);
    }
    else if constexpr (std::is_same_v<T, int>) {
        valueChanged = ImGui::SliderInt(label, value, v_min, v_max);
    }

    if (valueChanged) {
        Configuration::Save();
    }
    if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("%s", tooltip);
    }
    if (nextColumn) {
        ImGui::TableNextColumn();
    }
}

template<typename T>
inline void SliderHelper(const char* label, T* value, T v_min, T v_max, const char* format, const char* tooltip) {
    SliderHelper(label, value, v_min, v_max, format, false, tooltip);
}

template<typename T>
inline void SliderHelper(const char* label, T* value, T v_min, T v_max, const char* format, bool nextColumn) {
    SliderHelper(label, value, v_min, v_max, format, nextColumn, nullptr);
}

template<typename T>
inline void SliderHelper(const char* label, T* value, T v_min, T v_max, const char* format) {
    SliderHelper(label, value, v_min, v_max, format, false, nullptr);
}

template<typename T, typename... Args>
void SliderDispatcher(const char* label, T* value, T v_min, T v_max, const char* format, Args&&... args) {
    SliderHelper(label, value, v_min, v_max, format, std::forward<Args>(args)...);
}

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