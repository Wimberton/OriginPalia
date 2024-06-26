#include "RendererDetector.h"
#include "BaseHook.h"
#include "HookManager.h"
#include "Macros.h"

#include "Windows/DX12Hook.h"
#include "Windows/DX11Hook.h"
#include "Windows/DX10Hook.h"
#include "Windows/DX9Hook.h"
#include "Windows/OpenGLHook.h"
#include "Windows/WindowsHook.h"

#include <assert.h>

constexpr int MaxHookRetries = 500;
static decltype(&IDXGISwapChain::Present) _IDXGISwapChain_Present = nullptr;
static decltype(&IDirect3DDevice9::Present) _IDirect3DDevice9_Present = nullptr;
static decltype(&IDirect3DDevice9Ex::PresentEx) _IDirect3DDevice9Ex_PresentEx = nullptr;
static decltype(wglMakeCurrent)* _wglMakeCurrent = nullptr;

static constexpr auto windowClassName = "___overlay_window_class___";

RendererDetector& RendererDetector::Instance()
{
	static RendererDetector Instance;
	return Instance;
}

void RendererDetector::FindRenderer()
{
	if (HookThread == nullptr)
	{
		HookThread = new std::thread(&RendererDetector::FindRendererProc, this);
		HookThread->detach();
	}
}

void RendererDetector::RendererFound(BaseHook* InHook)
{
	std::lock_guard<std::mutex> lock(FoundMutex);
	HookManager& Manager = HookManager::Instance();

	bRendererFound = true;
	GameRenderer = InHook;

	if (InHook == nullptr)
		PRINT_DEBUG("Renderer found, hooking failed\n");
	else
		PRINT_DEBUG("Hooked renderer in %d/%d tries\n", HookRetries, MaxHookRetries);

	Manager.RemoveHook(RenderDetectHook);
}

BaseHook* RendererDetector::GetRenderer() const
{
	return GameRenderer;
}

void RendererDetector::CreateHWnd()
{
	if (DummyHWnd == nullptr) {
		HINSTANCE hInst = GetModuleHandle(nullptr);
		if (Atom == 0)
		{
			// Register a window class for creating our render window with.
			WNDCLASSEX windowClass = {};

			windowClass.cbSize = sizeof(WNDCLASSEX);
			windowClass.style = CS_HREDRAW | CS_VREDRAW;
			windowClass.lpfnWndProc = DefWindowProc;
			windowClass.cbClsExtra = 0;
			windowClass.cbWndExtra = 0;
			windowClass.hInstance = hInst;
			windowClass.hIcon = NULL;
			windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
			windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			windowClass.lpszMenuName = NULL;
			windowClass.lpszClassName = windowClassName;
			windowClass.hIconSm = NULL;

			Atom = ::RegisterClassEx(&windowClass);
		}

		if (Atom > 0)
		{
			DummyHWnd = ::CreateWindowEx(
				NULL,
				windowClassName,
				"",
				WS_OVERLAPPEDWINDOW,
				0,
				0,
				1,
				1,
				NULL,
				NULL,
				hInst,
				nullptr
			);

			assert(DummyHWnd && "Failed to create window");
		}
	}
}

void RendererDetector::DestroyHWnd()
{
	if (DummyHWnd != nullptr)
	{
		DestroyWindow(DummyHWnd);
		UnregisterClass(windowClassName, GetModuleHandle(nullptr));
	}
}

HRESULT WINAPI RendererDetector::MyIDXGISwapChain_Present(IDXGISwapChain* _this, UINT SyncInterval, UINT Flags)
{
	RendererDetector& inst = RendererDetector::Instance();
	HookManager& hm = HookManager::Instance();

	auto res = (_this->*_IDXGISwapChain_Present)(SyncInterval, Flags);
	if (!inst.StopRetry())
	{
		IUnknown* pDevice = nullptr;
		if (inst.bDX12Hooked)
		{
			_this->GetDevice(IID_PPV_ARGS(reinterpret_cast<ID3D12Device**>(&pDevice)));
		}
		if (pDevice)
		{
			DX12Hook::Instance()->StartHook();
		}
		else
		{
			if (inst.bDX11Hooked)
			{
				_this->GetDevice(IID_PPV_ARGS(reinterpret_cast<ID3D11Device**>(&pDevice)));
			}
			if (pDevice)
			{
				DX11Hook::Instance()->StartHook();
			}
			else
			{
				if (inst.bDX10Hooked)
				{
					_this->GetDevice(IID_PPV_ARGS(reinterpret_cast<ID3D10Device**>(&pDevice)));
				}
				if (pDevice)
				{
					DX10Hook::Instance()->StartHook();
				}
			}
		}
		if (pDevice) pDevice->Release();
	}

	return res;
}

HRESULT WINAPI RendererDetector::MyPresent(IDirect3DDevice9* _this, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
	RendererDetector& inst = RendererDetector::Instance();
	HookManager& hm = HookManager::Instance();
	auto res = (_this->*_IDirect3DDevice9_Present)(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	if (!inst.StopRetry())
	{
		DX9Hook::Instance()->StartHook();
	}
	return res;
}

HRESULT WINAPI RendererDetector::MyPresentEx(IDirect3DDevice9Ex* _this, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags)
{
	RendererDetector& inst = RendererDetector::Instance();
	HookManager& hm = HookManager::Instance();
	auto res = (_this->*_IDirect3DDevice9Ex_PresentEx)(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
	if (!inst.StopRetry())
	{
		DX9Hook::Instance()->StartHook();
	}
	return res;
}

BOOL WINAPI RendererDetector::MywglMakeCurrent(HDC hDC, HGLRC hGLRC)
{
	RendererDetector& inst = RendererDetector::Instance();
	HookManager& hm = HookManager::Instance();
	auto res = _wglMakeCurrent(hDC, hGLRC);
	if (!inst.StopRetry())
	{
		OpenGLHook::Instance()->StartHook();
	}
	return res;
}

void RendererDetector::HookDXGIPresent(IDXGISwapChain* pSwapChain)
{
	if (!bDXGIHooked) {
		bDXGIHooked = true;
		(void*&)_IDXGISwapChain_Present = (*reinterpret_cast<void***>(pSwapChain))[(int)IDXGISwapChainVTable::Present];

		RenderDetectHook->BeginHook();
		RenderDetectHook->HookFuncs(std::pair<void**, void*>((PVOID*)&_IDXGISwapChain_Present, &RendererDetector::MyIDXGISwapChain_Present));
		RenderDetectHook->EndHook();
	}
}

void RendererDetector::HookDX9Present(IDirect3DDevice9* pDevice, bool ex)
{
	(void*&)_IDirect3DDevice9_Present = (*reinterpret_cast<void***>(pDevice))[(int)IDirect3DDevice9VTable::Present];
	if (ex)
		(void*&)_IDirect3DDevice9Ex_PresentEx = (*reinterpret_cast<void***>(pDevice))[(int)IDirect3DDevice9VTable::PresentEx];

	RenderDetectHook->BeginHook();
	RenderDetectHook->HookFuncs(std::pair<void**, void*>((PVOID*)&_IDirect3DDevice9_Present, &RendererDetector::MyPresent));
	if (ex)
		RenderDetectHook->HookFuncs(std::pair<void**, void*>((PVOID*)&_IDirect3DDevice9Ex_PresentEx, &RendererDetector::MyPresentEx));
	RenderDetectHook->EndHook();
}

void RendererDetector::HookwglMakeCurrent(decltype(wglMakeCurrent)* wglMakeCurrent)
{
	_wglMakeCurrent = wglMakeCurrent;
	RenderDetectHook->BeginHook();
	RenderDetectHook->HookFuncs(std::pair<void**, void*>((PVOID*)&_wglMakeCurrent, &RendererDetector::MywglMakeCurrent));
	RenderDetectHook->EndHook();
}

void RendererDetector::HookDX9()
{
	if (!bDX9Hooked && !bRendererFound)
	{
		CreateHWnd();
		if (DummyHWnd == nullptr)
			return;

		IDirect3D9Ex* pD3D = nullptr;
		IUnknown* pDevice = nullptr;
		HMODULE library = GetModuleHandle(DX9_DLL);
		decltype(Direct3DCreate9Ex)* Direct3DCreate9Ex = nullptr;
		if (library != nullptr)
		{
			Direct3DCreate9Ex = (decltype(Direct3DCreate9Ex))GetProcAddress(library, "Direct3DCreate9Ex");
			D3DPRESENT_PARAMETERS params = {};
			params.BackBufferWidth = 1;
			params.BackBufferHeight = 1;
			params.hDeviceWindow = DummyHWnd;
			params.BackBufferCount = 1;
			params.Windowed = TRUE;
			params.SwapEffect = D3DSWAPEFFECT_DISCARD;

			if (Direct3DCreate9Ex != nullptr)
			{
				Direct3DCreate9Ex(D3D_SDK_VERSION, &pD3D);
				pD3D->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, NULL, reinterpret_cast<IDirect3DDevice9Ex**>(&pDevice));
			}
			else
			{
				decltype(Direct3DCreate9)* Direct3DCreate9 = (decltype(Direct3DCreate9))GetProcAddress(library, "Direct3DCreate9");
				if (Direct3DCreate9 != nullptr)
				{
					pD3D = reinterpret_cast<IDirect3D9Ex*>(Direct3DCreate9(D3D_SDK_VERSION));
					pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, reinterpret_cast<IDirect3DDevice9**>(&pDevice));
				}
			}
		}

		if (pDevice != nullptr)
		{
			PRINT_DEBUG("Hooked D3D9::Present to detect DX Version\n");

			bDX9Hooked = true;
			auto h = DX9Hook::Instance();
			h->LoadFunctions(reinterpret_cast<IDirect3DDevice9*>(pDevice), Direct3DCreate9Ex != nullptr);
			HookManager::Instance().AddHook(h);
			HookDX9Present(reinterpret_cast<IDirect3DDevice9*>(pDevice), Direct3DCreate9Ex != nullptr);
		}
		else
		{
			PRINT_DEBUG("Failed to hook D3D9::Present to detect DX Version\n");
		}

		if (pDevice) pDevice->Release();
		if (pD3D) pD3D->Release();
	}
}


void RendererDetector::HookDX10()
{
	if (!bDX10Hooked && !bRendererFound)
	{
		CreateHWnd();
		if (DummyHWnd == nullptr)
			return;

		IDXGISwapChain* pSwapChain = nullptr;
		ID3D10Device* pDevice = nullptr;
		HMODULE library = GetModuleHandle(DX10_DLL);
		if (library != nullptr)
		{
			decltype(D3D10CreateDeviceAndSwapChain)* D3D10CreateDeviceAndSwapChain =
				(decltype(D3D10CreateDeviceAndSwapChain))GetProcAddress(library, "D3D10CreateDeviceAndSwapChain");
			if (D3D10CreateDeviceAndSwapChain != nullptr)
			{
				DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};

				SwapChainDesc.BufferCount = 1;
				SwapChainDesc.BufferDesc.Width = 1;
				SwapChainDesc.BufferDesc.Height = 1;
				SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
				SwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
				SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				SwapChainDesc.OutputWindow = DummyHWnd;
				SwapChainDesc.SampleDesc.Count = 1;
				SwapChainDesc.SampleDesc.Quality = 0;
				SwapChainDesc.Windowed = TRUE;

				D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_NULL, NULL, 0, D3D10_SDK_VERSION, &SwapChainDesc, &pSwapChain, &pDevice);
			}
		}
		if (pSwapChain != nullptr)
		{
			PRINT_DEBUG("Hooked IDXGISwapChain::Present to detect DX Version\n");

			bDX10Hooked = true;
			auto h = DX10Hook::Instance();
			h->LoadFunctions(pSwapChain);
			HookManager::Instance().AddHook(h);
			HookDXGIPresent(pSwapChain);
		}
		else
		{
			PRINT_DEBUG("Failed to Hook IDXGISwapChain::Present to detect DX Version\n");
		}
		if (pDevice)pDevice->Release();
		if (pSwapChain)pSwapChain->Release();
	}
}


void RendererDetector::HookDX11()
{
	if (!bDX11Hooked && !bRendererFound)
	{
		CreateHWnd();
		if (DummyHWnd == nullptr)
			return;

		IDXGISwapChain* pSwapChain = nullptr;
		ID3D11Device* pDevice = nullptr;
		HMODULE library = GetModuleHandle(DX11_DLL);
		if (library != nullptr)
		{
			decltype(D3D11CreateDeviceAndSwapChain)* D3D11CreateDeviceAndSwapChain =
				(decltype(D3D11CreateDeviceAndSwapChain))GetProcAddress(library, "D3D11CreateDeviceAndSwapChain");
			if (D3D11CreateDeviceAndSwapChain != nullptr)
			{
				DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};

				SwapChainDesc.BufferCount = 1;
				SwapChainDesc.BufferDesc.Width = 1;
				SwapChainDesc.BufferDesc.Height = 1;
				SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
				SwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
				SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				SwapChainDesc.OutputWindow = DummyHWnd;
				SwapChainDesc.SampleDesc.Count = 1;
				SwapChainDesc.SampleDesc.Quality = 0;
				SwapChainDesc.Windowed = TRUE;

				D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_NULL, NULL, 0, NULL, NULL, D3D11_SDK_VERSION, &SwapChainDesc, &pSwapChain, &pDevice, NULL, NULL);
			}
		}
		if (pSwapChain != nullptr)
		{
			PRINT_DEBUG("Hooked IDXGISwapChain::Present to detect DX Version\n");

			bDX11Hooked = true;
			auto h = DX11Hook::Instance();
			h->LoadFunctions(pSwapChain);
			HookManager::Instance().AddHook(h);
			HookDXGIPresent(pSwapChain);
		}
		else
		{
			PRINT_DEBUG("Failed to Hook IDXGISwapChain::Present to detect DX Version\n");
		}

		if (pDevice) pDevice->Release();
		if (pSwapChain) pSwapChain->Release();
	}
}

void RendererDetector::HookDX12()
{
	if (!bDX12Hooked && !bRendererFound)
	{
		CreateHWnd();
		if (DummyHWnd == nullptr)
			return;

		IDXGIFactory4* pDXGIFactory = nullptr;
		IDXGISwapChain1* pSwapChain = nullptr;
		ID3D12CommandQueue* pCommandQueue = nullptr;
		ID3D12Device* pDevice = nullptr;

		HMODULE library = GetModuleHandle(DX12_DLL);
		if (library != nullptr)
		{
			decltype(D3D12CreateDevice)* D3D12CreateDevice =
				(decltype(D3D12CreateDevice))GetProcAddress(library, "D3D12CreateDevice");

			if (D3D12CreateDevice != nullptr)
			{
				D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice));

				if (pDevice != nullptr)
				{
					DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
					SwapChainDesc.BufferCount = 2;
					SwapChainDesc.Width = 1;
					SwapChainDesc.Height = 1;
					SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					SwapChainDesc.Stereo = FALSE;
					SwapChainDesc.SampleDesc = { 1, 0 };
					SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
					SwapChainDesc.Scaling = DXGI_SCALING_NONE;
					SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
					SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

					D3D12_COMMAND_QUEUE_DESC queueDesc = {};
					queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
					queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
					pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pCommandQueue));

					if (pCommandQueue != nullptr)
					{
						HMODULE dxgi = GetModuleHandle("dxgi.dll");
						if (dxgi != nullptr)
						{
							decltype(CreateDXGIFactory1)* CreateDXGIFactory1 = (decltype(CreateDXGIFactory1))GetProcAddress(dxgi, "CreateDXGIFactory1");
							if (CreateDXGIFactory1 != nullptr)
							{
								CreateDXGIFactory1(IID_PPV_ARGS(&pDXGIFactory));
								if (pDXGIFactory != nullptr)
								{
									pDXGIFactory->CreateSwapChainForHwnd(pCommandQueue, DummyHWnd, &SwapChainDesc, NULL, NULL, &pSwapChain);
								}
							}
						}
					}
				}//if (pDevice != nullptr)
			}//if (D3D12CreateDevice != nullptr)
		}//if (library != nullptr)
		if (pCommandQueue != nullptr && pSwapChain != nullptr)
		{
			PRINT_DEBUG("Hooked IDXGISwapChain::Present to detect DX Version\n");

			bDX12Hooked = true;
			auto h = DX12Hook::Instance();
			h->LoadFunctions(pCommandQueue, pSwapChain);
			HookManager::Instance().AddHook(h);
			HookDXGIPresent(pSwapChain);
		}
		else
		{
			PRINT_DEBUG("Failed to Hook IDXGISwapChain::Present to detect DX Version\n");
		}

		if (pSwapChain) pSwapChain->Release();
		if (pDXGIFactory) pDXGIFactory->Release();
		if (pCommandQueue) pCommandQueue->Release();
		if (pDevice) pDevice->Release();
	}
}

void RendererDetector::HookOGL()
{
	if (!bOGLHooked && !bRendererFound)
	{
		HMODULE library = GetModuleHandle(OPENGL_DLL);
		decltype(wglMakeCurrent)* wglMakeCurrent = nullptr;
		OpenGLHook::wglSwapBuffers_t wglSwapBuffers = nullptr;
		if (library != nullptr)
		{
			wglMakeCurrent = (decltype(wglMakeCurrent))GetProcAddress(library, "wglMakeCurrent");
			wglSwapBuffers = (decltype(wglSwapBuffers))GetProcAddress(library, "wglSwapBuffers");
		}
		if (wglMakeCurrent != nullptr && wglSwapBuffers != nullptr)
		{
			PRINT_DEBUG("Hooked wglMakeCurrent to detect OpenGL\n");

			bOGLHooked = true;
			auto h = OpenGLHook::Instance();
			h->LoadFunctions(wglSwapBuffers);
			HookManager::Instance().AddHook(h);
			HookwglMakeCurrent(wglMakeCurrent);
		}
		else
		{
			PRINT_DEBUG("Failed to Hook wglMakeCurrent to detect OpenGL\n");
		}
	}
}

void RendererDetector::CreateHook(const char* libname)
{
	if (!_stricmp(libname, DX9_DLL))
		HookDX9();
	else if (!_stricmp(libname, DX10_DLL))
		HookDX10();
	else if (!_stricmp(libname, DX11_DLL))
		HookDX11();
	else if (!_stricmp(libname, DX12_DLL))
		HookDX12();
	else if (!_stricmp(libname, OPENGL_DLL))
		HookOGL();
}

bool RendererDetector::StopRetry()
{
	bool bStop = ++HookRetries >= MaxHookRetries;
	if (bStop)
		RendererFound(nullptr);

	return bStop;
}


void RendererDetector::FindRendererProc(RendererDetector* _this)
{
	_this->RenderDetectHook = new BaseHook();
	HookManager& hm = HookManager::Instance();
	hm.AddHook(_this->RenderDetectHook);

	std::vector<std::string> const libraries = {
		OPENGL_DLL,
		DX12_DLL,
		DX11_DLL,
		DX10_DLL,
		DX9_DLL
	};

	while (!_this->bRendererFound && !_this->StopRetry())
	{
		std::vector<std::string>::const_iterator it = libraries.begin();
		while (it != libraries.end())
		{
			{
				std::lock_guard<std::mutex> lock(_this->FoundMutex);
				if (_this->bRendererFound)
					break;

				it = std::find_if(it, libraries.end(), [](std::string const& name) {
					auto x = GetModuleHandle(name.c_str());
				if (x != NULL)
					return true;
				return false;
					});

				if (it == libraries.end())
					break;

				_this->CreateHook(it->c_str());
				++it;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	_this->DestroyHWnd();

	if (_this->GameRenderer == nullptr) // Couldn't hook renderer
	{
		hm.RemoveHook(WindowsHook::Instance());
	}
	else
	{
		hm.AddHook(WindowsHook::Instance());
	}
	if (_this->bOGLHooked)
	{
		auto h = OpenGLHook::Instance();
		if (h != _this->GameRenderer)
		{
			_this->bOGLHooked = false;
			hm.RemoveHook(h);
		}
	}
	if (_this->bDX9Hooked)
	{
		auto h = DX9Hook::Instance();
		if (h != _this->GameRenderer)
		{
			_this->bDX9Hooked = false;
			hm.RemoveHook(h);
		}
	}
	if (_this->bDX10Hooked)
	{
		auto h = DX10Hook::Instance();
		if (h != _this->GameRenderer)
		{
			_this->bDX10Hooked = false;
			hm.RemoveHook(h);
		}
	}
	if (_this->bDX11Hooked)
	{
		auto h = DX11Hook::Instance();
		if (h != _this->GameRenderer)
		{
			_this->bDX11Hooked = false;
			hm.RemoveHook(h);
		}
	}
	if (_this->bDX12Hooked)
	{
		auto h = DX12Hook::Instance();
		if (h != _this->GameRenderer)
		{
			_this->bDX12Hooked = false;
			hm.RemoveHook(h);
		}
	}

	delete _this->HookThread;
	_this->HookThread = nullptr;
}

RendererDetector::RendererDetector() :
	HookThread(nullptr),
	HookRetries(0),
	bRendererFound(false),
	bDX9Hooked(false),
	bDX10Hooked(false),
	bDX11Hooked(false),
	bDX12Hooked(false),
	bDXGIHooked(false),
	bOGLHooked(false),
	RenderDetectHook(nullptr),
	GameRenderer(nullptr),
	Atom(0),
	DummyHWnd(nullptr)
{}

RendererDetector::~RendererDetector()
{
	if (HookThread != nullptr)
		HookRetries = MaxHookRetries;
}