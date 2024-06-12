#pragma once
#include <imgui.h>

namespace ImGui {
void BeginGroupPanel(const char *name, const ImVec2 &size = ImVec2(-1.0f, -1.0f));
void EndGroupPanel();
void ColorPicker(const char *name, ImU32 *color);
void AddText(ImDrawList *drawList, char *text, ImU32 textColor, ImVec2 screenPosition,
             ImU32 backgroundColor = 0x80000000);
} // namespace ImGui