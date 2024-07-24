#include "../../backend.hpp"
#include "./hook_dinput8.hpp"

#ifdef ENABLE_BACKEND_DI8

#include "console/console.hpp"
#include "menu/menu.hpp"
#include "MinHook.h"
#include <memory>

#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

IDirectInput8* createDirectInput8( ) {
	HINSTANCE hinst = GetModuleHandle(NULL);
    IDirectInput8* pDirectInput;
    auto hr = DirectInput8Create(
		hinst, 
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&pDirectInput,
		NULL
	);
    if (FAILED(hr))
        return NULL;
	return pDirectInput;
}

IDirectInputDevice8* createDevice(GUID guid, IDirectInput8* dinput) {
	IDirectInputDevice8* pDevice;
    auto hr = dinput->CreateDevice(guid, &pDevice, NULL);
    if (FAILED(hr))
        return NULL;
    return pDevice;
}

static std::add_pointer_t<HRESULT WINAPI(IDirectInputDevice8* device, DWORD cbData, LPVOID lpvData)> oGetDeviceState_mouse;
HRESULT WINAPI hkGetDeviceState_mouse(IDirectInputDevice8* device, DWORD cbData, LPVOID lpvData) {
	LOG("[+] Hello from mouse hook!\n");
    if (Menu::bShowMenu) {
		memset(lpvData, 0, cbData);
		return S_OK;
    } else {
		return oGetDeviceState_mouse(device, cbData, lpvData);
    }
}

static std::add_pointer_t<HRESULT WINAPI(IDirectInputDevice8* device, DWORD cbData, LPVOID lpvData)> oGetDeviceState_keyboard;
HRESULT WINAPI hkGetDeviceState_keyboard(IDirectInputDevice8* device, DWORD cbData, LPVOID lpvData) {
	LOG("[+] Hello from keyboard hook!\n");
    if (Menu::bShowMenu) {
		return S_OK;
    } else {
		return oGetDeviceState_keyboard(device, cbData, lpvData);
    }
}

enum IDirectInputDevice8_Method {
	GetDeviceState = 9
};

namespace DI8 {

	void HookDevice(IDirectInputDevice8* device, size_t vtableIndex, LPVOID pDetour, LPVOID* ppOriginal) {

		LOG("[+] Hooking device: %llX\n", reinterpret_cast<UINT_PTR>(device));

		void** vtable = *reinterpret_cast<void***>(device);

		LOG("[+] Device vtable: %llX\n", reinterpret_cast<UINT_PTR>(vtable));

		void* functionToHook = vtable[vtableIndex];

		LOG("[+] Original function: %llX\n", reinterpret_cast<UINT_PTR>(functionToHook));

		static MH_STATUS hookStatus = MH_CreateHook(
			reinterpret_cast<void**>(functionToHook), 
			pDetour, ppOriginal
		);

		LOG("[+] Hook status: %llu\n", hookStatus);

		hookStatus = MH_EnableHook(functionToHook);

		LOG("[+] Hook status: %llu\n", hookStatus);

	}

	void Hook() {

		auto dinput = createDirectInput8( );
        if (dinput) {
            LOG("[+] Created DirectInput8.\n");

            auto mouse = createDevice(GUID_SysMouse, dinput);
            if (mouse) {
				LOG("[+] Hooking mouse.\n");
                HookDevice(
					mouse, 
					IDirectInputDevice8_Method::GetDeviceState, 
					&hkGetDeviceState_mouse,
					reinterpret_cast<void**>(&oGetDeviceState_mouse)
				);
				mouse->Release( );
            }

#ifdef ENABLE_D8_KEYBOARD_HOOK
			// We're not using this for now. MinHook is shitting the bed when using both these hooks.
			// Maybe we should mimick the usage pattern in DX9 hook closer:
			//   1. Pull function pointers out of the device.
			//   2. Release the device. 
			//   3. Create the hooks.
			//   4. Enable the hooks.
			auto keyboard = createDevice(GUID_SysKeyboard, dinput);
            if (keyboard) {
				LOG("[+] Hooking keyboard.\n");
                HookDevice(
					keyboard, 
					IDirectInputDevice8_Method::GetDeviceState,
					&hkGetDeviceState_keyboard,
					reinterpret_cast<void**>(&oGetDeviceState_keyboard)
				);
				keyboard->Release( );
            }
#endif

			dinput->Release( );
        }

	}

	void Unhook() {

	}

}

#else
namespace DI8 {
    void Hook( ) {
    }
    void Unhook( ) {
    }
}
#endif
