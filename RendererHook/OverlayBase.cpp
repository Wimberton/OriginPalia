#include "OverlayBase.h"
#include "RendererDetector.h"
#include <imgui.h>
#include "Windows/WindowsHook.h"
#include <cmath>

OverlayBase* OverlayBase::Instance = nullptr;

void OverlayBase::HookReady()
{
	if (!bIsReady)
	{
		//ImGuiIO &io = ImGui::GetIO();
		//io.WantSetMousePos = false;
		//io.MouseDrawCursor = false;
		//io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		bIsReady = true;
	}
}

void OverlayBase::CreateFonts()
{
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig fontcfg;

	fontcfg.OversampleH = fontcfg.OversampleV = 1;
	fontcfg.PixelSnapH = true;
	fontcfg.GlyphRanges = io.Fonts->GetGlyphRangesDefault();

	fontcfg.SizePixels = std::round(io.DisplaySize.y / 68);
	FontDefault = io.Fonts->AddFontDefault(&fontcfg);

	fontcfg.SizePixels = std::round(io.DisplaySize.y / 60);
	FontHUD = io.Fonts->AddFontDefault(&fontcfg);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0; // Disable round window
}

void OverlayBase::OverlayProc()
{
	if (!IsReady()) return;

	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushFont(FontHUD);
	DrawHUD();
	ImGui::PopFont();

	if (bShowOverlay)
	{
		io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;

		ImGui::PushFont(FontDefault);
		DrawOverlay();
		ImGui::PopFont();
	}
	else {
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	}

}

void OverlayBase::ShowOverlay(bool bShow)
{
	if (!IsReady() || bShowOverlay == bShow)
		return;

	ImGuiIO& io = ImGui::GetIO();

	io.MouseDrawCursor = bShow;

	static RECT old_clip;

	if (bShow)
	{
		HWND GameHWnd = WindowsHook::Instance()->GetGameHwnd();
		RECT cliRect, wndRect, clipRect;

		GetClipCursor(&old_clip);
		// The window rectangle has borders and menus counted in the size
		GetWindowRect(GameHWnd, &wndRect);
		// The client rectangle is the window without borders
		GetClientRect(GameHWnd, &cliRect);

		clipRect = wndRect; // Init clip rectangle

		// Get Window width with borders
		wndRect.right -= wndRect.left;
		// Get Window height with borders & menus
		wndRect.bottom -= wndRect.top;
		// Compute the border width
		int borderWidth = (wndRect.right - cliRect.right) / 2;
		// Client top clip is the menu bar width minus bottom border
		clipRect.top += wndRect.bottom - cliRect.bottom - borderWidth;
		// Client left clip is the left border minus border width
		clipRect.left += borderWidth;
		// Here goes the same for right and bottom
		clipRect.right -= borderWidth;
		clipRect.bottom -= borderWidth;

		ClipCursor(&clipRect);
	}
	else
	{
		ClipCursor(&old_clip);
	}

	bShowOverlay = bShow;
}

void OverlayBase::SetupOverlay()
{
	std::lock_guard<std::recursive_mutex> lock(OverlayMutex);
	if (!bSetupOverlayCalled)
	{
		bSetupOverlayCalled = true;
		RendererDetector::Instance().FindRenderer();
	}
}
