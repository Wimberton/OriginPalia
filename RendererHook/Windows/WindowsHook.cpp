#include "WindowsHook.h"

#include "../ImGui/imgui.h"
#include "../ImGui/impls/windows/imgui_impl_win32.h"
#include "../OverlayBase.h"
#include "../Macros.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam);

WindowsHook *WindowsHook::_inst = nullptr;

bool WindowsHook::StartHook() {
  bool res = true;
  if (!hooked) {
    GetRawInputBuffer = ::GetRawInputBuffer;
    GetRawInputData = ::GetRawInputData;
    SetCursorPos = ::SetCursorPos;

    PRINT_DEBUG("Hooked Windows\n");

    BeginHook();
    HookFuncs(
        std::make_pair<void **, void *>(&(PVOID &)GetRawInputBuffer,
                                        &WindowsHook::MyGetRawInputBuffer),
        std::make_pair<void **, void *>(&(PVOID &)GetRawInputData,
                                        &WindowsHook::MyGetRawInputData),
        std::make_pair<void **, void *>(&(PVOID &)SetCursorPos,
                                        &WindowsHook::MySetCursorPos));
    EndHook();

    hooked = true;
  }
  return res;
}

void WindowsHook::ResetRenderState() {
  if (initialized) {
    // Restore the original window procedure
    SetWindowLongPtr(_game_hwnd, GWLP_WNDPROC, (LONG_PTR)_game_wndproc);
    _game_hwnd = nullptr;
    _game_wndproc = nullptr;
    ImGui_ImplWin32_Shutdown();
    initialized = false;
  }
}

void WindowsHook::PrepareForOverlay(HWND hWnd) {
  if (!initialized) {
    ImGui_ImplWin32_Init(hWnd);

    _game_hwnd = hWnd;
    _game_wndproc = (WNDPROC)SetWindowLongPtr(
        hWnd, GWLP_WNDPROC, (LONG_PTR)&WindowsHook::HookWndProc);

    initialized = true;
  }

  ImGui_ImplWin32_NewFrame();
}

HWND WindowsHook::GetGameHwnd() const { return _game_hwnd; }

WNDPROC WindowsHook::GetGameWndProc() const { return _game_wndproc; }

/////////////////////////////////////////////////////////////////////////////////////
// Windows window hooks
bool IgnoreMsg(UINT uMsg) {
  switch (uMsg) {
    // Mouse Events
  case WM_MOUSEMOVE:
  case WM_MOUSEWHEEL:
  case WM_MOUSEHWHEEL:
  case WM_LBUTTONUP:
  case WM_LBUTTONDOWN:
  case WM_LBUTTONDBLCLK:
  case WM_RBUTTONUP:
  case WM_RBUTTONDOWN:
  case WM_RBUTTONDBLCLK:
  case WM_MBUTTONUP:
  case WM_MBUTTONDOWN:
  case WM_MBUTTONDBLCLK:
  case WM_XBUTTONUP:
  case WM_XBUTTONDOWN:
  case WM_XBUTTONDBLCLK:
  case WM_MOUSEACTIVATE:
  case WM_MOUSEHOVER:
  case WM_MOUSELEAVE:
    // Keyboard Events
  case WM_KEYDOWN:
  case WM_KEYUP:
  case WM_SYSKEYDOWN:
  case WM_SYSKEYUP:
  case WM_SYSDEADCHAR:
  case WM_CHAR:
  case WM_UNICHAR:
  case WM_DEADCHAR:
    // Raw Input Events
  case WM_INPUT:
    return true;
  }
  return false;
}

void RawMouseEvent(RAWINPUT &raw) {
  if (raw.header.dwType == RIM_TYPEMOUSE) {
    if (raw.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
      ImGui_ImplWin32_WndProcHandler(WindowsHook::Instance()->GetGameHwnd(),
                                     WM_LBUTTONDOWN, 0, 0);
    else if (raw.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
      ImGui_ImplWin32_WndProcHandler(WindowsHook::Instance()->GetGameHwnd(),
                                     WM_LBUTTONUP, 0, 0);
    else if (raw.data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
      ImGui_ImplWin32_WndProcHandler(WindowsHook::Instance()->GetGameHwnd(),
                                     WM_MBUTTONDOWN, 0, 0);
    else if (raw.data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
      ImGui_ImplWin32_WndProcHandler(WindowsHook::Instance()->GetGameHwnd(),
                                     WM_MBUTTONUP, 0, 0);
    else if (raw.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
      ImGui_ImplWin32_WndProcHandler(WindowsHook::Instance()->GetGameHwnd(),
                                     WM_RBUTTONDOWN, 0, 0);
    else if (raw.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
      ImGui_ImplWin32_WndProcHandler(WindowsHook::Instance()->GetGameHwnd(),
                                     WM_RBUTTONUP, 0, 0);
  }
}

LRESULT CALLBACK WindowsHook::HookWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                          LPARAM lParam) {
  OverlayBase *overlay = OverlayBase::Instance;
  bool show = overlay->ShowOverlay();
  // Is the event is a key press
  if (uMsg == WM_KEYDOWN) {
    // INSERT is pressed and was not pressed before
    if (wParam == VK_INSERT && !(lParam & (1 << 30))) {
      show = !show;
      overlay->ShowOverlay(show);
    }
  }

  ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
  if (show) {
    if (IgnoreMsg(uMsg))
      return 0;
  }

  // Call the overlay window procedure
  return CallWindowProc(WindowsHook::Instance()->_game_wndproc, hWnd, uMsg,
                        wParam, lParam);
}

UINT WINAPI WindowsHook::MyGetRawInputBuffer(PRAWINPUT pData, PUINT pcbSize,
                                             UINT cbSizeHeader) {
  if (pData == nullptr || !OverlayBase::Instance->ShowOverlay())
    return WindowsHook::Instance()->GetRawInputBuffer(pData, pcbSize,
                                                      cbSizeHeader);

  int num =
      WindowsHook::Instance()->GetRawInputBuffer(pData, pcbSize, cbSizeHeader);
  for (int i = 0; i < num; ++i)
    RawMouseEvent(pData[i]);

  return 0;
}

UINT WINAPI WindowsHook::MyGetRawInputData(HRAWINPUT hRawInput, UINT uiCommand,
                                           LPVOID pData, PUINT pcbSize,
                                           UINT cbSizeHeader) {
  if (pData == nullptr || !OverlayBase::Instance->ShowOverlay())
    return WindowsHook::Instance()->GetRawInputData(hRawInput, uiCommand, pData,
                                                    pcbSize, cbSizeHeader);

  WindowsHook::Instance()->GetRawInputData(hRawInput, uiCommand, pData, pcbSize,
                                           cbSizeHeader);

  RawMouseEvent(*reinterpret_cast<RAWINPUT *>(pData));

  *pcbSize = 0;

  return 0;
}

BOOL WINAPI WindowsHook::MySetCursorPos(int x, int y) {
  if (OverlayBase::Instance->ShowOverlay()) {
    POINT p;
    GetCursorPos(&p);
    x = p.x;
    y = p.y;
  }

  return WindowsHook::Instance()->SetCursorPos(x, y);
}
/////////////////////////////////////////////////////////////////////////////////////

WindowsHook::WindowsHook()
    : initialized(false), hooked(false), _game_hwnd(nullptr),
      _game_wndproc(nullptr), GetRawInputBuffer(nullptr),
      GetRawInputData(nullptr) {
  //_library = LoadLibrary(DLL_NAME);
}

WindowsHook::~WindowsHook() {
  PRINT_DEBUG("Windows Hook removed\n");

  ResetRenderState();

  // FreeLibrary(reinterpret_cast<HMODULE>(_library));

  _inst = nullptr;
}

WindowsHook *WindowsHook::Instance() {
  if (_inst == nullptr)
    _inst = new WindowsHook;

  return _inst;
}

const char *WindowsHook::GetLibName() const { return WINDOWS_DLL; }