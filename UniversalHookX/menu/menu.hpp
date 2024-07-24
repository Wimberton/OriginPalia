#pragma once

#include <Windows.h>
#include <mutex>
#include "imgui.h"

class MenuBase {
public:
    virtual ~MenuBase() = default;
    static MenuBase* Instance;

    void OverlayProc();

protected:
    // Called always - use to draw an HUD
    virtual void DrawHUD() = 0;
    // Called only when overlay is active
    virtual void DrawOverlay() = 0;

    virtual void DrawGuiESP() = 0;

    virtual void DrawGuiFOVCircle() = 0;
};

namespace Menu {
    void InitializeContext(HWND hwnd);
    void Render();
    void CreateFonts();

    inline HWND gameHwnd;

    inline bool bShowMenu = false;

    inline ImFont* FontDefault; // Inter-Medium
    inline ImFont* FontHUD;
    inline ImFont* InterBlack;
    inline ImFont* InterLight;
    inline ImFont* FontAwesome;
    inline ImFont* ESPFont;
} // namespace Menu
