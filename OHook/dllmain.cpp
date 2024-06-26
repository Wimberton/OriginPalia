#include <Windows.h>
#include <iostream>

#include "../PaliaSDK/SDK.hpp"
#include "PaliaOverlay.h"

// Forward declaration
DWORD WINAPI MainThread(LPVOID lpReserved);

BOOL APIENTRY DllMain(const HMODULE hModule, const DWORD ul_reason_for_call) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
    }

    return TRUE;
}

DWORD WINAPI MainThread(const LPVOID lpReserved) {
    // Steam Support
    bool IsSteamVersion = false;

    if (reinterpret_cast<uintptr_t>(GetModuleHandle("steam_api64.dll")) || reinterpret_cast<uintptr_t>(GetModuleHandle("steamclient64.dll"))) {
        IsSteamVersion = true;
    } else {
        char fileName[MAX_PATH];
        GetModuleFileName(nullptr, fileName, MAX_PATH);
        if (const std::string filePath(fileName); filePath.find("PaliaClientSteam-Win64-Shipping.exe") != std::string::npos) {
            IsSteamVersion = true;
        }
    }

    if (IsSteamVersion) {
        Offsets::GObjects = 0x08AAC680;             // 1.181.0
        Offsets::AppendString = 0x00CFFDD0;         // 1.181.0
        Offsets::GNames = 0x08A05CC0;               // 1.181.0
        Offsets::GWorld = 0x08C1D0B8;               // 1.181.0
        Offsets::ProcessEvent = 0x00ED9950;         // 1.181.0
        Offsets::ProcessEventIdx = 0x0000004D;      // 1.181.0
    }

    // Initialize SDK and create overlay
    //InitGObjects();

    const auto Overlay = new PaliaOverlay();
    OverlayBase::Instance = Overlay;
    Overlay->SetupColors();
    Overlay->SetupOverlay();

    // Wait for the thread to finish
    WaitForSingleObject(lpReserved, INFINITE);

    return true;
}
