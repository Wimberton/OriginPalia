#include <Windows.h>
#include <iostream>
#include "../PaliaSDK/SDK.hpp"
#include "PaliaOverlay.h"

// Forward declaration
DWORD WINAPI MainThread(LPVOID lpReserved);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
    // Added code for steamAPI recognition (thanks VoidPollo)
    bool IsSteamVersion = false;
    if ((uintptr_t)GetModuleHandle("steam_api64.dll")) {
        IsSteamVersion = true;
    }
    else if ((uintptr_t)GetModuleHandle("steamclient64.dll")) {
        IsSteamVersion = true;
    }
    else {
        char fileName[MAX_PATH];
        GetModuleFileName(NULL, fileName, MAX_PATH);
        if (strstr(fileName, "PaliaClientSteam-Win64-Shipping.exe")) {
            IsSteamVersion = true;
        }
    }

    if (IsSteamVersion) {
        Offsets::GObjects = 0x089C9C00;
        Offsets::AppendString = 0x00CF9A50;
        Offsets::GWorld = 0x08B3A638;
        Offsets::ProcessEvent = 0x00ED35D0;
    }

    // Initialize SDK and create overlay
    InitGObjects();

    auto Overlay = new PaliaOverlay();
    OverlayBase::Instance = Overlay;
    Overlay->SetupColors();
    Overlay->SetupOverlay();

    // Wait for the thread to finish
    WaitForSingleObject(reinterpret_cast<HANDLE>(lpReserved), INFINITE);

    return TRUE;
}