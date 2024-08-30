#include "menu.hpp"

#include "imgui.h"
#include "impls/windows/imgui_impl_win32.h"

#include "fonts/fonts.h"

#include "menu.hpp"

#include "overlay.hpp"
#include "hooks/hooks.hpp"
#include "console/console.hpp"

#include <cmath>
#include <iostream>

namespace ig = ImGui;

MenuBase* MenuBase::Instance = nullptr;

void MenuBase::OverlayProc() {
	DrawGuiESP();
	DrawGuiFOVCircle();

	DrawHUD();
	if (Menu::bShowMenu) DrawOverlay();
}

namespace Menu {

    void InitializeContext(const HWND hwnd) {
        if (ig::GetCurrentContext( ))
            return;

        ImGui::CreateContext( );
        ImGui_ImplWin32_Init(hwnd);

        gameHwnd = hwnd;

        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = io.LogFilename = nullptr;
    }

    void CreateFonts() {
		ImGuiIO& io = ImGui::GetIO();

		//define icon ranges
		static constexpr ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

		// Inter Medium (Default)
		ImFontConfig inter_medium_config;
		inter_medium_config.OversampleH = inter_medium_config.OversampleV = 2;
		inter_medium_config.PixelSnapH = true;
		inter_medium_config.GlyphRanges = io.Fonts->GetGlyphRangesDefault();
		if (inter_medium_config.SizePixels <= 0.0f)
			inter_medium_config.SizePixels = 15.0f * 1.0f;

		FontDefault = io.Fonts->AddFontFromMemoryCompressedBase85TTF(inter_medium_ttf, inter_medium_config.SizePixels, &inter_medium_config, inter_medium_config.GlyphRanges);
		{
			// Merge FontAwesome with Default
			ImFontConfig icons_config;
			icons_config.MergeMode = true;
			icons_config.PixelSnapH = true;
			icons_config.FontDataOwnedByAtlas = false;
			icons_config.GlyphOffset.y = 2.5f;
			icons_config.GlyphOffset.x = -2.2f;
			icons_config.OversampleH = icons_config.OversampleV = 2;
			if (icons_config.SizePixels <= 0.0f)
				icons_config.SizePixels = 18.0f;
			io.Fonts->AddFontFromMemoryCompressedBase85TTF(fontAwesome, icons_config.SizePixels, &icons_config, icons_ranges);
		}

		// Inter Light
		ImFontConfig inter_light_config;
		inter_light_config.OversampleH = inter_light_config.OversampleV = 2;
		inter_light_config.PixelSnapH = true;
		inter_light_config.GlyphRanges = io.Fonts->GetGlyphRangesDefault();
		if (inter_light_config.SizePixels <= 0.0f)
			inter_light_config.SizePixels = 15.0f * 1.0f;

		InterLight = io.Fonts->AddFontFromMemoryCompressedBase85TTF(inter_light_ttf, inter_light_config.SizePixels, &inter_light_config, inter_light_config.GlyphRanges);
		{
			// Merge FontAwesome with Inter Light
			ImFontConfig icons_config_light;
			icons_config_light.MergeMode = true;
			icons_config_light.PixelSnapH = true;
			icons_config_light.FontDataOwnedByAtlas = false;
			icons_config_light.GlyphOffset.y = 2.4f;
			icons_config_light.GlyphOffset.x = -2.0f;
			icons_config_light.OversampleH = icons_config_light.OversampleV = 2;
			if (icons_config_light.SizePixels <= 0.0f)
				icons_config_light.SizePixels = 18.0f;
			io.Fonts->AddFontFromMemoryCompressedBase85TTF(fontAwesome, icons_config_light.SizePixels, &icons_config_light, icons_ranges);
		}

		// Inter Black
		ImFontConfig inter_black_config;
		inter_black_config.OversampleH = inter_black_config.OversampleV = 2;
		inter_black_config.PixelSnapH = true;
		inter_black_config.GlyphRanges = io.Fonts->GetGlyphRangesDefault();
		if (inter_black_config.SizePixels <= 0.0f)
			inter_black_config.SizePixels = 15.0f;

		InterBlack = io.Fonts->AddFontFromMemoryCompressedBase85TTF(inter_black_ttf, inter_black_config.SizePixels, &inter_black_config, inter_black_config.GlyphRanges);
		{
			// Merge FontAwesome with Inter Black
			ImFontConfig icons_config_black;
			icons_config_black.MergeMode = true;
			icons_config_black.PixelSnapH = true;
			icons_config_black.FontDataOwnedByAtlas = false;
			icons_config_black.GlyphOffset.y = 2.4f;
			icons_config_black.GlyphOffset.x = -2.5f;
			icons_config_black.OversampleH = icons_config_black.OversampleV = 2;
			if (icons_config_black.SizePixels <= 0.0f)
				icons_config_black.SizePixels = 18.0f;
			io.Fonts->AddFontFromMemoryCompressedBase85TTF(fontAwesome, icons_config_black.SizePixels, &icons_config_black, icons_ranges);
		}

		// Inter Semi-Bold
		ImFontConfig inter_semibold_config;
		inter_semibold_config.OversampleH = inter_semibold_config.OversampleV = 2;
		inter_semibold_config.PixelSnapH = true;
		inter_semibold_config.GlyphRanges = io.Fonts->GetGlyphRangesDefault();
		if (inter_semibold_config.SizePixels <= 0.0f)
			inter_semibold_config.SizePixels = 14.5f;

		ESPFont = io.Fonts->AddFontFromMemoryCompressedBase85TTF(inter_semibold_ttf, inter_semibold_config.SizePixels, &inter_semibold_config, inter_semibold_config.GlyphRanges);
		{
			// Merge FontAwesome with ESPFont (Inter-Semibold
			ImFontConfig icons_config_semibold;
			icons_config_semibold.MergeMode = true;
			icons_config_semibold.PixelSnapH = true;
			icons_config_semibold.FontDataOwnedByAtlas = false;
			icons_config_semibold.GlyphOffset.y = 2.4f;
			icons_config_semibold.GlyphOffset.x = -2.5f;
			icons_config_semibold.OversampleH = icons_config_semibold.OversampleV = 2;
			if (icons_config_semibold.SizePixels <= 0.0f)
				icons_config_semibold.SizePixels = 16.5f;
			io.Fonts->AddFontFromMemoryCompressedBase85TTF(fontAwesome, icons_config_semibold.SizePixels, &icons_config_semibold, icons_ranges);
		}

		// Add FontHUD
		ImFontConfig fontcfg;
		fontcfg.OversampleH = fontcfg.OversampleV = 2;
		fontcfg.PixelSnapH = true;
		fontcfg.GlyphRanges = io.Fonts->GetGlyphRangesDefault();
		fontcfg.SizePixels = std::round(io.DisplaySize.y / 60);
		FontHUD = io.Fonts->AddFontDefault(&fontcfg);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0; // Disable round window
    }

    void Render( ) {
        ImGui::Begin("Overlay", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs);
        {
            // Set Window position to top left corner
            ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);

            // Set Window size to full screen
            ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

            Overlay::Draw();
        }
        ImGui::End();
    }
} // namespace Menu
