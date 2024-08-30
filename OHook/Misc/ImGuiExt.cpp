#include "ImGuiExt.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace {
	ImVector<ImRect> s_GroupPanelLabelStack;
}

void ImGui::ShowHotkeyMenu(const std::string& actionName, const std::vector<std::pair<std::string, std::function<void()>>>& additionalMenuItems) {
	static std::unordered_map<std::string, bool> waitingForKeyPress;
	static std::unordered_map<std::string, int> selectedKey;
	static std::set<int> usedKeys;

	isHotkeySelectorPopupOpen = IsPopupOpen(("Select Hotkey##" + actionName).c_str());

	// Populate usedKeys set
	usedKeys.clear();
	const auto& hotkeys = HotkeysManager::GetHotkeys();
	for (const auto& [otherActionName, keyCallbackPair] : hotkeys) {
		if (otherActionName != actionName) {
			usedKeys.insert(keyCallbackPair.first);
		}
	}

	std::string popupName = "Select Hotkey##" + actionName;  // Ensure the popup name is unique
	if (BeginPopupModal(popupName.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		float windowWidth = ImGui::GetWindowSize().x;
		float buttonWidth = 120.0f * 2 + ImGui::GetStyle().ItemSpacing.x;
		ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

		Text("Press any key to assign it as a hotkey...");

		Separator();

		for (int key = 0x01; key <= 0xFE; ++key) {
			if (GetAsyncKeyState(key) & 0x8000) {
				if (HotkeysManager::GetDisallowedKeys().contains(key)) {
					// Skip showing the warning for the left mouse button
					if (key != 0x01) {
						Text("Key is disallowed!");
					}
				}
				else if (usedKeys.contains(key)) {
					Text("Key is already in use!");
				}
				else {
					ImGui::InsertNotification({ImGuiToastType::Success, 2000, "Changed hotkey of '%s' to '%s'.", actionName.c_str(), GetHotkeyName(key).c_str()});
					selectedKey[actionName] = key;
					waitingForKeyPress[actionName] = false;

					CloseCurrentPopup();
				}
			}
		}

		// Handle Escape key press
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
			HotkeysManager::SetHotkey(actionName, 0); // Set to none (0)
			Configuration::Save(ESaveFile::HotkeySettings);
			waitingForKeyPress[actionName] = false;
			CloseCurrentPopup();
		}

		// Add "Clear" button
		if (Button("Clear", ImVec2(120, 0))) {
			HotkeysManager::SetHotkey(actionName, 0); // Set to none (0)
			Configuration::Save(ESaveFile::HotkeySettings);
			waitingForKeyPress[actionName] = false;
			ImGui::InsertNotification({ImGuiToastType::Info, 2000, "Cleared current hotkey."});
			CloseCurrentPopup();
		}

		SameLine();

		// Add "Cancel" button
		if (Button("Cancel", ImVec2(120, 0))) {
			waitingForKeyPress[actionName] = false;
			CloseCurrentPopup();
		}
		EndPopup();
	}

	int currentKey = HotkeysManager::GetHotkey(actionName);
	std::string buttonText = "Change Hotkey [" + (currentKey ? GetHotkeyName(currentKey) + "]" : "None]");

	std::string menuName = "Options Menu##" + actionName;
	if (BeginPopup(menuName.c_str(), ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
		// Default menu item to change the hotkey of current selected checkbox/button
		if (MenuItem(buttonText.c_str())) {
			waitingForKeyPress[actionName] = true;
			CloseCurrentPopup();
		}

		// Optional additional menu items
		for (const auto& [itemLabel, itemCallback] : additionalMenuItems) {
			static std::string optionalActionId = itemLabel;
			optionalActionId.erase(std::remove(optionalActionId.begin(), optionalActionId.end(), ' '), optionalActionId.end());
			static std::string optionalActionName = itemLabel + "##" + optionalActionId;
			if (MenuItem(optionalActionName.c_str())) {
				itemCallback();
				CloseCurrentPopup();
			}
		}
		
		EndPopup();
	}

	if (waitingForKeyPress[actionName]) {
		OpenPopup(popupName.c_str());
	}

	if (selectedKey[actionName] != 0) {
		HotkeysManager::SetHotkey(actionName, selectedKey[actionName]);
		selectedKey[actionName] = 0;
		Configuration::Save(ESaveFile::HotkeySettings);
	}

	isHotkeySelectorPopupOpen = IsPopupOpen(("Select Hotkey##" + actionName).c_str());
}

bool ImGui::Checkbox2(const char* label, bool* v, const std::vector<std::pair<std::string, std::function<void()>>>& additionalMenuItems) {
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	// ============================================
	// Standard checkbox area
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	const float square_sz = GetFrameHeight();
	const ImVec2 pos = window->DC.CursorPos;
	const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
	ItemSize(total_bb, style.FramePadding.y);
	const bool is_visible = ItemAdd(total_bb, id);
	if (!is_visible)
	{
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
		return false;
	}

	bool hovered, held;
	bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
	{
		*v = !(*v);
		MarkItemEdited(id);
	}

	const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
	const bool mixed_value = (g.LastItemData.InFlags & ImGuiItemFlags_MixedValue) != 0;
	if (is_visible)
	{
		RenderNavHighlight(total_bb, id);
		RenderFrame(check_bb.Min, check_bb.Max, GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
		ImU32 check_col = GetColorU32(ImGuiCol_CheckMark);
		if (mixed_value)
		{
			// Undocumented tristate/mixed/indeterminate checkbox (#2644)
			// This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue supported by all widgets (not just checkbox)
			ImVec2 pad(ImMax(1.0f, IM_TRUNC(square_sz / 3.6f)), ImMax(1.0f, IM_TRUNC(square_sz / 3.6f)));
			window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding);
		}
		else if (*v)
		{
			const float pad = ImMax(1.0f, IM_TRUNC(square_sz / 6.0f));
			RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f);
		}
	}
	const ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
	if (g.LogEnabled)
		LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");
	if (is_visible && label_size.x > 0.0f)
		RenderText(label_pos, label);

	// ============================================
	// Hotkey button area
	std::string actionName = label;
	actionName.erase(std::remove(actionName.begin(), actionName.end(), ' '), actionName.end());

	std::string menuName = "Options Menu##" + actionName;
	
	ShowHotkeyMenu(actionName);

	if (hovered && GetIO().MouseDown[1]) {
		OpenPopup(menuName.c_str());
	}

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
	return pressed;
}

bool ImGui::Button2(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags) {
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ItemSize(size, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	// Render
	const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	RenderNavHighlight(bb, id);
	RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

	if (g.LogEnabled)
		LogSetNextTextDecoration("[", "]");
	RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

	// ============================================
	// Hotkey button area
	std::string actionName = label;
	actionName.erase(std::remove(actionName.begin(), actionName.end(), ' '), actionName.end());

	std::string menuName = "Options Menu##" + actionName;

	ShowHotkeyMenu(actionName);

	if (hovered && GetIO().MouseDown[1]) {
		OpenPopup(menuName.c_str());
	}

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return pressed;
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