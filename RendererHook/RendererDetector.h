#pragma once
#include <thread>
#include <mutex>
#include <Windows.h>

class BaseHook;

struct IDXGISwapChain;
struct IDirect3DDevice9;
struct IDirect3DDevice9Ex;

class RendererDetector
{
public:
	static RendererDetector& Instance();

	void FindRenderer();
	void RendererFound(BaseHook* InHook);
	BaseHook* GetRenderer() const;

private:
	RendererDetector();
	~RendererDetector();

	static HRESULT STDMETHODCALLTYPE MyIDXGISwapChain_Present(IDXGISwapChain* _this, UINT SyncInterval, UINT Flags);
	static HRESULT STDMETHODCALLTYPE MyPresent(IDirect3DDevice9* _this, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
	static HRESULT STDMETHODCALLTYPE MyPresentEx(IDirect3DDevice9Ex* _this, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
	static BOOL WINAPI MywglMakeCurrent(HDC hDC, HGLRC hGLRC);

	void HookDXGIPresent(IDXGISwapChain* pSwapChain);
	void HookDX9Present(IDirect3DDevice9* pDevice, bool ex);
	void HookwglMakeCurrent(decltype(wglMakeCurrent)* wglMakeCurrent);

	void HookDX9();
	void HookDX10();
	void HookDX11();
	void HookDX12();
	void HookOGL();

	void CreateHWnd();
	void DestroyHWnd();
	void CreateHook(const char* libname);
	bool StopRetry();

	static void FindRendererProc(RendererDetector* _this);

private:
	std::thread* HookThread;
	std::mutex FoundMutex;
	unsigned int HookRetries;
	bool bRendererFound;
	bool bDX9Hooked;
	bool bDX10Hooked;
	bool bDX11Hooked;
	bool bDX12Hooked;
	bool bDXGIHooked;
	bool bOGLHooked;
	BaseHook* RenderDetectHook;
	BaseHook* GameRenderer;

	ATOM Atom;
	HWND DummyHWnd;
};