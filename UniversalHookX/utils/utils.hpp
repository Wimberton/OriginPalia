#pragma once

#include <Windows.h>

enum RenderingBackend_t {
	NONE = 0,

	DIRECTX9,
	DIRECTX10,
	DIRECTX11,
	DIRECTX12,

	OPENGL,
	VULKAN,
};

namespace Utils {
	void SetRenderingBackend(RenderingBackend_t eRenderingBackend);
	RenderingBackend_t GetRenderingBackend( );
	const char* RenderingBackendToStr( );

	HWND GetProcessWindow( );
	void UnloadDLL( );
	
	HMODULE GetCurrentImageBase( );

	int GetCorrectDXGIFormat(int eCurrentFormat);

	// Safe dereference
	template <typename T>
	BOOL safeDeref(T* ptr, T& result) {
		if (!ptr)
			return false;
		DWORD pid = GetCurrentProcessId();
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if (!hProcess)
			return false;
		BOOL success = ReadProcessMemory(hProcess, ptr, &result, sizeof(T), nullptr);
		CloseHandle(hProcess);
		return success;
	}

	// Safe read array
	template <typename T>
	BOOL safeReadArray(T* ptr, T* result, size_t size) {
		if (!ptr)
			return false;
		if (!result)
			return false;
		DWORD pid = GetCurrentProcessId();
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if (!hProcess)
			return false;
		BOOL success = ReadProcessMemory(hProcess, ptr, result, size * sizeof(T), nullptr);
		CloseHandle(hProcess);
		return success;
	}

}

namespace U = Utils;
