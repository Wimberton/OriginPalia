#pragma once
#include <Windows.h>
#include "../BaseHook.h"
class WindowsHook : public BaseHook
{
public:
#define WINDOWS_DLL "user32.dll"
public:
    virtual ~WindowsHook();

    void ResetRenderState();
    void PrepareForOverlay(HWND);

    HWND GetGameHwnd() const;
    WNDPROC GetGameWndProc() const;

    bool StartHook();
    static WindowsHook* Instance();
    virtual const char* GetLibName() const;
private:
    // Functions
    WindowsHook();

    // Hook to Windows window messages
    decltype(GetRawInputBuffer)* GetRawInputBuffer;
    decltype(GetRawInputData)* GetRawInputData;
    decltype(SetCursorPos)* SetCursorPos;

    static LRESULT CALLBACK HookWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static UINT WINAPI MyGetRawInputBuffer(PRAWINPUT pData, PUINT pcbSize, UINT cbSizeHeader);
    static UINT WINAPI MyGetRawInputData(HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader);

    static BOOL WINAPI MySetCursorPos(int x, int y);
private:
    static WindowsHook* _inst;

    // Variables
    bool hooked;
    bool initialized;
    HWND _game_hwnd;
    WNDPROC _game_wndproc;
};

