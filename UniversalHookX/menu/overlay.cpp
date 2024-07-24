#include "overlay.hpp"
#include "overlay_pub.h"
#include "math.hpp"

#include "console/console.hpp"
#include "menu.hpp"
#include "utils/utils.hpp"

#include "imgui.h"
#include "impls/windows/imgui_impl_win32.h"

#include <Windows.h>

#include <mutex>
#include <map>

using namespace Math;

///////////////////////

std::mutex apiMutex;

///////////////////////

namespace Overlay {

    #define COL_RED IM_COL32(255, 0, 0, 255)
    #define COL_GREEN IM_COL32(0, 255, 0, 255)
    #define COL_BLUE IM_COL32(0, 0, 255, 255)
    #define COL_WHITE IM_COL32(255, 255, 255, 255)
    #define COL_BLACK IM_COL32(0, 0, 0, 255)
    #define COL_TRANSPARENT IM_COL32(0, 0, 0, 0)
    #define COL_YELLOW IM_COL32(255, 255, 0, 255)
    //
    #define COL_NEUTRAL IM_COL32(0, 255, 0, 128)
    #define COL_HIGHLIGHT COL_GREEN

    void Draw() {
        const std::lock_guard<std::mutex> lock(apiMutex);

        ImVec2 winSize = ImGui::GetWindowSize();
        auto drawList = ImGui::GetWindowDrawList();

        ImGuiIO io = ImGui::GetIO();

        // Overlay menus.
        MenuBase::Instance->OverlayProc();
    }

}
