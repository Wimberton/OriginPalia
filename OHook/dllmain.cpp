#include "Core/Main.h"
#include "Core/HotkeysManager.h"
#include "Overlay/PaliaOverlay.h"

#include "console/console.hpp"
#include "hooks/hooks.hpp"

#include <Windows.h>
#include <thread>
#include <bit>

#include "MinHook.h"

#include "../PaliaSDK/SDK.hpp"

// Forward declaration
DWORD WINAPI OnProcessAttach(LPVOID lpParam);
DWORD WINAPI OnProcessDetach(LPVOID lpParam);

BOOL WINAPI DllMain(const HMODULE hModule, const DWORD fdwReason, const LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        HANDLE hHandle = CreateThread(nullptr, 0, OnProcessAttach, hModule, 0, nullptr);
        if (hHandle != nullptr) {
            CloseHandle(hHandle);
        }
    } else if (fdwReason == DLL_PROCESS_DETACH && !lpReserved) {
        OnProcessDetach(nullptr);
    }

    return TRUE;
}

DWORD WINAPI OnProcessAttach(const LPVOID lpParam) {
    auto hModule = static_cast<HMODULE>(lpParam);
	int32_t GMallocOffset = 0x089F16E0; // 0.183.0

	// Steam Support
	char fileName[MAX_PATH];
	GetModuleFileName(nullptr, fileName, MAX_PATH);
	if (const std::string filePath(fileName); filePath.find("PaliaClientSteam-Win64-Shipping.exe") != std::string::npos) {
		Offsets::GObjects			= 0x08B419C0; // 0.183.0
		Offsets::AppendString		= 0x00D03FC0; // 0.183.0
		Offsets::GNames				= 0x08A9B000; // 0.183.0
		Offsets::GWorld				= 0x08CB2408; // 0.183.0
		Offsets::ProcessEvent		= 0x00EDDD10; // 0.183.0
		Offsets::ProcessEventIdx	= 0x0000004D; // 0.183.0
		GMallocOffset				= 0x08A60960; // 0.183.0
	}
    
    const auto Overlay = new PaliaOverlay();
    MenuBase::Instance = Overlay;

    // Opens a console window (if enabled in console.hpp)
    Console::Alloc();
    
    // Initialize MinHook
    if (MH_Initialize() != MH_OK) {
        LOG("[!] MinHook initialization failed.\n");
        FreeLibraryAndExitThread(hModule, 0);
    }

#ifdef FORCE_BACKEND
	static HMODULE d3d12 = GetModuleHandle("D3D12.dll");
	static HMODULE d3d12core = GetModuleHandle("D3D12Core.dll");
	static HMODULE d3dscache = GetModuleHandle("D3DSCache.dll");
	static HMODULE d3d11 = GetModuleHandle("d3d11.dll");

	LOG("[+] Forced Renderer logs: D3D12.dll: %s, D3D12Core.dll: %s, D3DSCache.dll: %s, d3d11.dll: %s\n",
		d3d12 ? "[X]" : "[ ]",
		d3d12core ? "[X]" : "[ ]",
		d3dscache ? "[X]" : "[ ]",
		d3d11 ? "[X]" : "[ ]");

	LOG("[!] Manually forcing DX12.\n");
	U::SetRenderingBackend(DIRECTX12);
#else
    std::thread findRenderer(H::FindRenderer);
    findRenderer.join();

    if (!H::FoundRenderer()) {
        LOG("[!] Failed to find backend renderer. Exiting.\n");
        FreeLibraryAndExitThread(hModule, 1);
    }
#endif

    // Initialize FMemory
    FMemory::FMalloc::UnrealStaticGMalloc = std::bit_cast<FMemory::FMalloc**>(InSDKUtils::GetImageBase() + GMallocOffset);
    FMemory::GMalloc = *FMemory::FMalloc::UnrealStaticGMalloc;

    H::Init();

    Main::Start();
    
    return 0;
}

DWORD WINAPI OnProcessDetach(const LPVOID lpParam) {
    Main::Stop();
    H::Free();
    MH_Uninitialize();

    Console::Free();
    return 0;
}
