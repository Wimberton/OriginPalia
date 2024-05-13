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
    // Initialize SDK and create overlay
    SDK::InitGObjects();
    auto Overlay = new PaliaOverlay();
    OverlayBase::Instance = Overlay;
    Overlay->SetupColors();
    Overlay->SetupOverlay();

    // Wait for the thread to finish
    WaitForSingleObject(reinterpret_cast<HANDLE>(lpReserved), INFINITE);

    return TRUE;
}