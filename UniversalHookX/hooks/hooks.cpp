#include <cstdio>
#include <mutex>
#include <thread>

#include "hooks.hpp"

#include "backend/dx11/hook_directx11.hpp"
#include "backend/dx12/hook_directx12.hpp"

#include "console/console.hpp"
#include "menu/menu.hpp"
#include "utils/utils.hpp"

#include "MinHook.h"

#include "input/hook_mouse.hpp"
#include "input/hook_dinput8.hpp"

static HWND g_hWindow = NULL;
static std::mutex g_mReinitHooksGuard;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && Menu::bShowMenu) {
        return 1;
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

static DWORD WINAPI ReinitializeGraphicalHooks( LPVOID lpParam ) {
    std::lock_guard<std::mutex> guard{ g_mReinitHooksGuard };

    LOG( "[!] Hooks will reinitialize!\n" );

    HWND hNewWindow = U::GetProcessWindow();
    while ( hNewWindow == reinterpret_cast<HWND>( lpParam ) ) {
        hNewWindow = U::GetProcessWindow();
    }

    H::bShuttingDown = true;

    H::Free();
    H::Init();

    H::bShuttingDown = false;
    Menu::bShowMenu = true;

    return 0;
}

#define TILDE_KEY VK_OEM_3
//
#define KEY_TOGGLE_MENU VK_INSERT

void forceHideCursor() {
    while (ShowCursor(false) >= 0);
}
void forceShowCursor() {
    while (ShowCursor(true) < 0);
}

static WNDPROC oWndProc;
static LRESULT WINAPI WndProc( const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
    Mouse::enableApis = true;

    if (uMsg == WM_KEYDOWN) {
        if (wParam == KEY_TOGGLE_MENU) {
            Menu::bShowMenu = !Menu::bShowMenu;
            if (Menu::bShowMenu) {
                SetCursor(LoadCursor(nullptr, IDC_ARROW)); // Moving this here. Using it below will override ImGui's `io.mouse` cursor settings.
                
                // OLD METHOD - MOUSE TO MIDDLE WHEN TOGGLING MENU (Sometimes will throw the mouse to the middle-left based on calculations)
                //SetCursorPos(static_cast<int>(ImGui::GetIO().DisplaySize.x / 2), static_cast<int>(ImGui::GetIO().DisplaySize.y / 2));

                // NEW METHOD - Use this to correctly and accurately convert client view to screen coord position
                //POINT centerPos;
                //centerPos.x = static_cast<int>(ImGui::GetIO().DisplaySize.x / 2);
                //centerPos.y = static_cast<int>(ImGui::GetIO().DisplaySize.y / 2);
                //ClientToScreen(hWnd, &centerPos);
                //SetCursorPos(centerPos.x, centerPos.y);
            }
            return 0;
        }
    }

    bool callOriginal = true;

    LRESULT result = NULL;

    LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

    static RECT old_rect;
    
    if (Menu::bShowMenu) {
        switch (uMsg) {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_CHAR:
        case WM_SYSCHAR:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_XBUTTONDBLCLK:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_INPUT:
            ShowCursor(true);
            callOriginal = false;
            break;
        default:
            break;
        }
        
        ShowCursor(true);

        result = ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    }
    
    Mouse::enableApis = false;

    if (callOriginal)
        result = CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);

    return result;
}

enum DetectionState {
    DetectingDX12,
    DetectingDX11,
    DetectionComplete
};

namespace Hooks {
    void FindRenderer() {
        constexpr int max_retries = 10;
        static int retries = 0;
        static DetectionState state = DetectingDX12; // Set default to DX12.

        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        while (!bFoundRenderer && retries < max_retries + 1) {
            std::lock_guard<std::mutex> lock(FoundMutex);
            const char* type = (state == DetectingDX12) ? "DX12" : "DX11";

            uintptr_t d3d12 = reinterpret_cast<uintptr_t>(GetModuleHandle("D3D12.dll"));
            uintptr_t d3d12core = reinterpret_cast<uintptr_t>(GetModuleHandle("D3D12Core.dll"));
            uintptr_t d3dscache = reinterpret_cast<uintptr_t>(GetModuleHandle("D3DSCache.dll"));
            uintptr_t d3d11 = reinterpret_cast<uintptr_t>(GetModuleHandle("d3d11.dll"));

            if (state == DetectingDX12) {
                if (d3dscache && (d3d12 || d3d12core)) {
                    LOG("[+] Renderer Detection logs: Type: %s, D3D12.dll: %s, D3D12Core.dll: %s, D3DSCache.dll: %s, d3d11.dll: %s\n",
                        type,
                        d3d12 ? "[X]" : "[ ]",
                        d3d12core ? "[X]" : "[ ]",
                        d3dscache ? "[X]" : "[ ]",
                        d3d11 ? "[X]" : "[ ]");

                    LOG("[+] DX12 detected. Setting rendering backend.\n");
                    U::SetRenderingBackend(DIRECTX12);
                    bFoundRenderer = true;
                    state = DetectionComplete;
                    break;
                }
            }
            else if (state == DetectingDX11) {
                if (d3d11 && (!d3dscache || !d3d12)) {
                    LOG("[+] Renderer Detection logs: Type: %s, D3D12.dll: %s, D3D12Core.dll: %s, D3DSCache.dll: %s, d3d11.dll: %s\n",
                        type,
                        d3d12 ? "[X]" : "[ ]",
                        d3d12core ? "[X]" : "[ ]",
                        d3dscache ? "[X]" : "[ ]",
                        d3d11 ? "[X]" : "[ ]");

                    LOG("[+] DX11 detected. Setting rendering backend.\n");
                    U::SetRenderingBackend(DIRECTX11);
                    bFoundRenderer = true;
                    state = DetectionComplete;
                    break;
                }
            }

            if (!bFoundRenderer) {
                LOG("[+] Renderer Detection logs: Type: %s, D3D12.dll: %s, D3D12Core.dll: %s, D3DSCache.dll: %s, d3d11.dll: %s\n",
                    type,
                    d3d12 ? "[X]" : "[ ]",
                    d3d12core ? "[X]" : "[ ]",
                    d3dscache ? "[X]" : "[ ]",
                    d3d11 ? "[X]" : "[ ]");

                retries++;
                std::this_thread::sleep_for(std::chrono::milliseconds(250));

                if (retries >= max_retries && state == DetectingDX12) {
                    state = DetectingDX11;
                    retries = 0;
                }
            }
        }
    }

    bool FoundRenderer() {
        return bFoundRenderer;
    }

    void Init() {
        if (!FoundRenderer()) return;

        if (FoundRenderer()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        g_hWindow = U::GetProcessWindow();

        #ifdef DISABLE_LOGGING_CONSOLE
        bool bNoConsole = GetConsoleWindow() == NULL;
        if (bNoConsole) {
            AllocConsole();
        }
        #endif

        RenderingBackend_t eRenderingBackend = U::GetRenderingBackend();
        switch (eRenderingBackend) {
        case DIRECTX11:
            DX11::Hook(g_hWindow);
            break;
        case DIRECTX12:
            DX12::Hook(g_hWindow);
            break;
        }

        #ifdef DISABLE_LOGGING_CONSOLE
        if (bNoConsole) {
            FreeConsole();
        }
        #endif

        oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));

        Mouse::Hook();
        DI8::Hook();
    }

    void Free() {
        DI8::Unhook();
        Mouse::Unhook();

        if (oWndProc) {
            SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));
        }

        MH_DisableHook( MH_ALL_HOOKS );
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        RenderingBackend_t eRenderingBackend = U::GetRenderingBackend();
        switch (eRenderingBackend) {
        case DIRECTX11:
            DX11::Unhook();
            break;
        case DIRECTX12:
            DX12::Unhook();
            break;
        }

        Console::Free();
    }
} // namespace Hooks
