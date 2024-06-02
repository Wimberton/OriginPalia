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
        Offsets::GObjects = 0x08A68340; //0.179.0 0x089C9C00;
        Offsets::AppendString = 0x00CFEBB0; //0.179.0 0x00CF9A50;
        Offsets::GWorld = 0x08BD8D78; //0.179.0 0x08B3A638;
        Offsets::ProcessEvent = 0x00ED8730; //0.179.0 0x00ED35D0;
    }

    // Initialize SDK and create overlay
    InitGObjects();

    const auto Overlay = new PaliaOverlay();
    OverlayBase::Instance = Overlay;
    Overlay->SetupColors();
    Overlay->SetupOverlay();

    // Wait for the thread to finish
    WaitForSingleObject(lpReserved, INFINITE);

    return true;
}
