#include "DX11Hook.h"
#include "WindowsHook.h"
#include "../RendererDetector.h"
#include "../ImGui/imgui.h"
#include "../ImGui/impls/windows/imgui_impl_dx11.h"
#include "../OverlayBase.h"
#include "../Macros.h"

DX11Hook *DX11Hook::_inst = nullptr;

HRESULT GetDeviceAndCtxFromSwapchain(IDXGISwapChain *pSwapChain, ID3D11Device **ppDevice,
                                     ID3D11DeviceContext **ppContext) {
    HRESULT ret = pSwapChain->GetDevice(IID_PPV_ARGS(ppDevice));

    if (SUCCEEDED(ret))
        (*ppDevice)->GetImmediateContext(ppContext);

    return ret;
}

bool DX11Hook::StartHook() {
    bool res = true;
    if (!hooked) {
        if (!WindowsHook::Instance()->StartHook())
            return false;

        PRINT_DEBUG("Hooked DirectX 11\n");
        hooked = true;

        RendererDetector::Instance().RendererFound(this);

        BeginHook();
        HookFuncs(std::make_pair<void **, void *>(&(PVOID &)DX11Hook::Present, &DX11Hook::MyPresent),
                  std::make_pair<void **, void *>(&(PVOID &)DX11Hook::ResizeTarget, &DX11Hook::MyResizeTarget),
                  std::make_pair<void **, void *>(&(PVOID &)DX11Hook::ResizeBuffers, &DX11Hook::MyResizeBuffers));
        EndHook();

        OverlayBase::Instance->HookReady();
    }
    return res;
}

void DX11Hook::ResetRenderState() {
    if (initialized) {
        if (mainRenderTargetView) {
            mainRenderTargetView->Release();
            mainRenderTargetView = NULL;
        }

        pContext->Release();

        ImGui_ImplDX11_Shutdown();
        WindowsHook::Instance()->ResetRenderState();
        ImGui::DestroyContext();

        initialized = false;
    }
}

// Try to make this function and overlay's proc as short as possible or it might
// affect game's fps.
void DX11Hook::PrepareForOverlay(IDXGISwapChain *pSwapChain) {
    DXGI_SWAP_CHAIN_DESC desc;
    pSwapChain->GetDesc(&desc);

    if (!initialized) {
        ID3D11Device *pDevice = nullptr;
        if (FAILED(GetDeviceAndCtxFromSwapchain(pSwapChain, &pDevice, &pContext)))
            return;

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.IniFilename = NULL;

        ID3D11Texture2D *pBackBuffer;
        pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

        ID3D11RenderTargetView *get_targets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
        pContext->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, get_targets, NULL);
        bool bind_target = true;

        for (unsigned i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
            if (get_targets[i]) {
                ID3D11Resource *res = NULL;
                get_targets[i]->GetResource(&res);
                if (res) {
                    if (res == (ID3D11Resource *)pBackBuffer) {
                        bind_target = false;
                    }

                    res->Release();
                }

                get_targets[i]->Release();
            } else {
                break;
            }
        }

        if (bind_target) {
            pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
        }

        pBackBuffer->Release();

        ImGui_ImplDX11_Init(pDevice, pContext);

        pDevice->Release();

        OverlayBase::Instance->CreateFonts();

        initialized = true;
    }

    if (ImGui_ImplDX11_NewFrame()) {
        WindowsHook::Instance()->PrepareForOverlay(desc.OutputWindow);

        ImGui::NewFrame();

        OverlayBase::Instance->OverlayProc();

        ImGui::Render();

        if (mainRenderTargetView) {
            pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
        }

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
}

HRESULT STDMETHODCALLTYPE DX11Hook::MyPresent(IDXGISwapChain *_this, UINT SyncInterval, UINT Flags) {
    DX11Hook::Instance()->PrepareForOverlay(_this);
    return (_this->*DX11Hook::Instance()->Present)(SyncInterval, Flags);
}

HRESULT STDMETHODCALLTYPE DX11Hook::MyResizeTarget(IDXGISwapChain *_this, const DXGI_MODE_DESC *pNewTargetParameters) {
    DX11Hook::Instance()->ResetRenderState();
    return (_this->*DX11Hook::Instance()->ResizeTarget)(pNewTargetParameters);
}

HRESULT STDMETHODCALLTYPE DX11Hook::MyResizeBuffers(IDXGISwapChain *_this, UINT BufferCount, UINT Width, UINT Height,
                                                    DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
    DX11Hook::Instance()->ResetRenderState();
    return (_this->*DX11Hook::Instance()->ResizeBuffers)(BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

DX11Hook::DX11Hook()
    : initialized(false), hooked(false), pContext(nullptr), mainRenderTargetView(nullptr), Present(nullptr),
      ResizeBuffers(nullptr), ResizeTarget(nullptr) {
    _library = LoadLibrary(DX11_DLL);
}

DX11Hook::~DX11Hook() {
    PRINT_DEBUG("DX11 Hook removed\n");

    if (initialized) {
        if (mainRenderTargetView) {
            mainRenderTargetView->Release();
            mainRenderTargetView = NULL;
        }

        pContext->Release();

        ImGui_ImplDX11_InvalidateDeviceObjects();
        ImGui::DestroyContext();

        initialized = false;
    }

    FreeLibrary(reinterpret_cast<HMODULE>(_library));

    _inst = nullptr;
}

DX11Hook *DX11Hook::Instance() {
    if (_inst == nullptr)
        _inst = new DX11Hook;

    return _inst;
}

const char *DX11Hook::GetLibName() const { return DX11_DLL; }

void DX11Hook::LoadFunctions(IDXGISwapChain *pSwapChain) {
    void **vTable;

    vTable = *reinterpret_cast<void ***>(pSwapChain);
#define LOAD_FUNC(X) (void *&)X = vTable[(int)IDXGISwapChainVTable::X]
    LOAD_FUNC(Present);
    LOAD_FUNC(ResizeBuffers);
    LOAD_FUNC(ResizeTarget);
#undef LOAD_FUNC
}
