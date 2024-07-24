#include "ImGuiExt.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace {
	ImVector<ImRect> s_GroupPanelLabelStack;
}

void ImGui::BeginGroupPanel(const char* name, const ImVec2& size) {
	BeginGroup();

	//auto cursorPos = GetCursorScreenPos();
	auto itemSpacing = GetStyle().ItemSpacing;
	PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	auto frameHeight = GetFrameHeight();
	BeginGroup();

	ImVec2 effectiveSize;
	if (size.x < 0.0f)
		effectiveSize.x = GetContentRegionAvail().x;
	else
		effectiveSize.x = size.x;

	Dummy(ImVec2(effectiveSize.x, 0.0f));

	Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	SameLine(0.0f, 0.0f);
	BeginGroup();
	Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	SameLine(0.0f, 0.0f);
	TextUnformatted(name);
	auto labelMin = GetItemRectMin();
	auto labelMax = GetItemRectMax();
	SameLine(0.0f, 0.0f);
	Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
	BeginGroup();

	//ImGui::GetWindowDrawList()->AddRect(labelMin, labelMax, IM_COL32(255, 0, 255, 255));

	PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
	GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
	GetCurrentWindow()->WorkRect.Max.x -= frameHeight * 0.5f;
	GetCurrentWindow()->InnerRect.Max.x -= frameHeight * 0.5f;
#else
	GetCurrentWindow()->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
#endif
	GetCurrentWindow()->Size.x -= frameHeight;

	auto itemWidth = CalcItemWidth();
	PushItemWidth(ImMax(0.0f, itemWidth - frameHeight));

	s_GroupPanelLabelStack.push_back(ImRect(labelMin, labelMax));
}

void ImGui::EndGroupPanel()
{
	PopItemWidth();

	auto itemSpacing = GetStyle().ItemSpacing;

	PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	auto frameHeight = GetFrameHeight();

	EndGroup();

	//ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

	EndGroup();

	SameLine(0.0f, 0.0f);
	Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

	EndGroup();

	auto itemMin = GetItemRectMin();
	auto itemMax = GetItemRectMax();
	//ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

	auto labelRect = s_GroupPanelLabelStack.back();
	s_GroupPanelLabelStack.pop_back();

	ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
	auto frameRect = ImRect(itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f));
	labelRect.Min.x -= itemSpacing.x;
	labelRect.Max.x += itemSpacing.x;
	for (int i = 0; i < 4; ++i) {
		switch (i) {
		// left half-plane
		case 0:
			PushClipRect(ImVec2(-FLT_MAX, -FLT_MAX), ImVec2(labelRect.Min.x, FLT_MAX), true);
			break;
		// right half-plane
		case 1:
			PushClipRect(ImVec2(labelRect.Max.x, -FLT_MAX), ImVec2(FLT_MAX, FLT_MAX), true);
			break;
		// top
		case 2:
			PushClipRect(ImVec2(labelRect.Min.x, -FLT_MAX), ImVec2(labelRect.Max.x, labelRect.Min.y), true);
			break;
		// bottom
		case 3:
			PushClipRect(ImVec2(labelRect.Min.x, labelRect.Max.y), ImVec2(labelRect.Max.x, FLT_MAX), true);
			break;
		default:
			break;
		}

		GetWindowDrawList()->AddRect(
			frameRect.Min, frameRect.Max,
			ImColor(GetStyleColorVec4(ImGuiCol_Border)),
			halfFrame.x);

		PopClipRect();
	}

	PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
	GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
	GetCurrentWindow()->WorkRect.Max.x += frameHeight * 0.5f;
	GetCurrentWindow()->InnerRect.Max.x += frameHeight * 0.5f;
#else
	ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x += frameHeight * 0.5f;
#endif
	GetCurrentWindow()->Size.x += frameHeight;

	Dummy(ImVec2(0.0f, 0.0f));

	EndGroup();
}

void ImGui::ColorPicker(const char* name, ImU32* color)
{
	ImVec4 c = ColorConvertU32ToFloat4(*color);
	auto flags = ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs;
	if (ColorEdit4(name, reinterpret_cast<float*>(&c), flags)) {
		*color = ColorConvertFloat4ToU32(c);
		Configuration::Save();
	}
}

void ImGui::AddText(ImDrawList* drawList, const char* text, ImU32 textColor, ImVec2 screenPosition, ImU32 backgroundColor) {
	auto size = CalcTextSize(text);

	auto adjustedTextPosition = screenPosition - size * 0.5f;

	size += GetStyle().FramePadding * 2.0f;

	drawList->AddRectFilled(screenPosition - size * 0.5f, screenPosition + size * 0.5f, backgroundColor);
	drawList->AddText(adjustedTextPosition, textColor, text);
}