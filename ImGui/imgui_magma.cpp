#include "imgui_magma.h"

#include "imgui.h"

namespace ImGui {
    namespace Magma {

        void StyleColorsMagma() {
            ImGuiStyle* style = &ImGui::GetStyle();
            style->GrabRounding = 4.0f;

            ImVec4* colors = style->Colors;

            colors[ImGuiCol_Text] = ColorConvertU32ToFloat4(Magma::GRAY900); // text on hovered controls is gray900
            colors[ImGuiCol_TextDisabled] = ColorConvertU32ToFloat4(Magma::GRAY500);

            colors[ImGuiCol_WindowBg] = ColorConvertU32ToFloat4(0xFA252525);
            colors[ImGuiCol_ChildBg] = ColorConvertU32ToFloat4(Magma::GREEN700); // yet to see instance (set to green for noticeability when it occurs)
            colors[ImGuiCol_PopupBg] = ColorConvertU32ToFloat4(Magma::GRAY100); // not sure about this. Note: applies to tooltips too.

            colors[ImGuiCol_Border] = ColorConvertU32ToFloat4(Magma::GRAY200);
            colors[ImGuiCol_BorderShadow] = ColorConvertU32ToFloat4(Magma::Static::NONE); // We don't want shadows. Ever.

            colors[ImGuiCol_FrameBg] = ColorConvertU32ToFloat4(Magma::GRAY100); // this isnt right, spectrum does not do this, but it's a good fallback
            colors[ImGuiCol_FrameBgHovered] = ColorConvertU32ToFloat4(Magma::GRAY200);
            colors[ImGuiCol_FrameBgActive] = ColorConvertU32ToFloat4(Magma::GRAY100);

            colors[ImGuiCol_TitleBg] = ColorConvertU32ToFloat4(Magma::GRAY300); // those titlebar values are totally made up, spectrum does not have this.
            colors[ImGuiCol_TitleBgActive] = ColorConvertU32ToFloat4(Magma::GRAY200);
            colors[ImGuiCol_TitleBgCollapsed] = ColorConvertU32ToFloat4(Magma::GRAY400);

            colors[ImGuiCol_MenuBarBg] = ColorConvertU32ToFloat4(Magma::GRAY50);

            colors[ImGuiCol_ScrollbarBg] = ColorConvertU32ToFloat4(Magma::GRAY50); // same as regular background
            colors[ImGuiCol_ScrollbarGrab] = ColorConvertU32ToFloat4(Magma::GRAY200);
            colors[ImGuiCol_ScrollbarGrabHovered] = ColorConvertU32ToFloat4(Magma::GRAY300);
            colors[ImGuiCol_ScrollbarGrabActive] = ColorConvertU32ToFloat4(Magma::GRAY200);

            colors[ImGuiCol_CheckMark] = ColorConvertU32ToFloat4(Magma::RED400);

            colors[ImGuiCol_SliderGrab] = ColorConvertU32ToFloat4(Magma::RED400);
            colors[ImGuiCol_SliderGrabActive] = ColorConvertU32ToFloat4(Magma::RED500);

            colors[ImGuiCol_Button] = ColorConvertU32ToFloat4(Magma::RED200); // match default button to Spectrum's 'Action Button'.
            colors[ImGuiCol_ButtonHovered] = ColorConvertU32ToFloat4(Magma::RED300);
            colors[ImGuiCol_ButtonActive] = ColorConvertU32ToFloat4(Magma::RED200);

            colors[ImGuiCol_Header] = ColorConvertU32ToFloat4(Magma::RED75);
            colors[ImGuiCol_HeaderHovered] = ColorConvertU32ToFloat4(Magma::RED100);
            colors[ImGuiCol_HeaderActive] = ColorConvertU32ToFloat4(Magma::RED75);

            colors[ImGuiCol_Separator] = ColorConvertU32ToFloat4(Magma::GRAY300);
            colors[ImGuiCol_SeparatorHovered] = ColorConvertU32ToFloat4(Magma::GRAY400);
            colors[ImGuiCol_SeparatorActive] = ColorConvertU32ToFloat4(Magma::GRAY500);

            colors[ImGuiCol_ResizeGrip] = ColorConvertU32ToFloat4(Magma::RED400);
            colors[ImGuiCol_ResizeGripHovered] = ColorConvertU32ToFloat4(Magma::RED400);
            colors[ImGuiCol_ResizeGripActive] = ColorConvertU32ToFloat4(Magma::RED400);

            colors[ImGuiCol_Tab] = ColorConvertU32ToFloat4(Magma::GRAY300);
            colors[ImGuiCol_TabHovered] = ColorConvertU32ToFloat4(Magma::RED50);
            colors[ImGuiCol_TabActive] = ColorConvertU32ToFloat4(Magma::RED100);
            colors[ImGuiCol_TabUnfocused] = ColorConvertU32ToFloat4(Magma::GREEN400); //yet to see instance of this
            colors[ImGuiCol_TabUnfocusedActive] = ColorConvertU32ToFloat4(Magma::GREEN400); //yet to see instance of this

            colors[ImGuiCol_PlotLines] = ColorConvertU32ToFloat4(Magma::BLUE400);
            colors[ImGuiCol_PlotLinesHovered] = ColorConvertU32ToFloat4(Magma::BLUE600);
            colors[ImGuiCol_PlotHistogram] = ColorConvertU32ToFloat4(Magma::BLUE400);
            colors[ImGuiCol_PlotHistogramHovered] = ColorConvertU32ToFloat4(Magma::BLUE600);

            colors[ImGuiCol_TableHeaderBg] = ColorConvertU32ToFloat4(Magma::GRAY100);
            colors[ImGuiCol_TableBorderStrong] = ColorConvertU32ToFloat4(Magma::GRAY200);
            colors[ImGuiCol_TableBorderLight] = ColorConvertU32ToFloat4(Magma::GRAY200);
            colors[ImGuiCol_TableRowBg] = ColorConvertU32ToFloat4(Magma::GRAY300);
            colors[ImGuiCol_TableRowBgAlt] = ColorConvertU32ToFloat4(Magma::GRAY300);

            colors[ImGuiCol_TextSelectedBg] = ColorConvertU32ToFloat4((Magma::RED400 & 0x00FFFFFF) | 0x33000000);

            colors[ImGuiCol_DragDropTarget] = ColorConvertU32ToFloat4(Magma::GREEN700); //yet to see instance of this

            colors[ImGuiCol_NavHighlight] = ColorConvertU32ToFloat4((Magma::GRAY900 & 0x00FFFFFF) | 0x0A000000);
            colors[ImGuiCol_NavWindowingHighlight] = ColorConvertU32ToFloat4(Magma::GREEN700); //yet to see instance of this
            colors[ImGuiCol_NavWindowingDimBg] = ColorConvertU32ToFloat4(Magma::GREEN700); //yet to see instance of this

            colors[ImGuiCol_ModalWindowDimBg] = ColorConvertU32ToFloat4(0xBF2F2F2F); //yet to see instance of this
        }
    }
}