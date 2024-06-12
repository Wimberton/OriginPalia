#include "DX10Hook.h"
#include "WindowsHook.h"
#include "../RendererDetector.h"
#include "../OverlayBase.h"
#include "../ImGui/imgui.h"
#include "../ImGui/impls/windows/imgui_impl_dx10.h"
#include "../Macros.h"

DX10Hook *DX10Hook::_inst = nullptr;

bool DX10Hook::StartHook() {
  bool res = true;
  if (!hooked) {
    if (!WindowsHook::Instance()->StartHook())
      return false;

    PRINT_DEBUG("Hooked DirectX 10\n");
    hooked = true;

    RendererDetector::Instance().RendererFound(this);

    BeginHook();
    HookFuncs(std::make_pair<void **, void *>(&(PVOID &)DX10Hook::Present,
                                              &DX10Hook::MyPresent),
              std::make_pair<void **, void *>(&(PVOID &)DX10Hook::ResizeTarget,
                                              &DX10Hook::MyResizeTarget),
              std::make_pair<void **, void *>(&(PVOID &)DX10Hook::ResizeBuffers,
                                              &DX10Hook::MyResizeBuffers));
    EndHook();

    OverlayBase::Instance->HookReady();
  }
  return res;
}

void DX10Hook::ResetRenderState() {
  if (initialized) {
    mainRenderTargetView->Release();

    ImGui_ImplDX10_Shutdown();
    WindowsHook::Instance()->ResetRenderState();
    ImGui::DestroyContext();

    initialized = false;
  }
}

// Try to make this function and overlay's proc as short as possible or it might
// affect game's fps.
void DX10Hook::PrepareForOverlay(IDXGISwapChain *pSwapChain) {
  DXGI_SWAP_CHAIN_DESC desc;
  pSwapChain->GetDesc(&desc);

  if (!initialized) {
    if (FAILED(pSwapChain->GetDevice(IID_PPV_ARGS(&pDevice))))
      return;

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = NULL;

    ID3D10Texture2D *pBackBuffer;

    pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
    pBackBuffer->Release();

    ImGui_ImplDX10_Init(pDevice);

    pDevice->Release();

    OverlayBase::Instance->CreateFonts();

    initialized = true;
  }

  if (ImGui_ImplDX10_NewFrame()) {
    WindowsHook::Instance()->PrepareForOverlay(desc.OutputWindow);

    ImGui::NewFrame();

    OverlayBase::Instance->OverlayProc();

    ImGui::Render();

    pDevice->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());
  }
}

HRESULT STDMETHODCALLTYPE DX10Hook::MyPresent(IDXGISwapChain *_this,
                                              UINT SyncInterval, UINT Flags) {
  DX10Hook::Instance()->PrepareForOverlay(_this);
  return (_this->*DX10Hook::Instance()->Present)(SyncInterval, Flags);
}

HRESULT STDMETHODCALLTYPE DX10Hook::MyResizeTarget(
    IDXGISwapChain *_this, const DXGI_MODE_DESC *pNewTargetParameters) {
  DX10Hook::Instance()->ResetRenderState();
  return (_this->*DX10Hook::Instance()->ResizeTarget)(pNewTargetParameters);
}

HRESULT STDMETHODCALLTYPE DX10Hook::MyResizeBuffers(IDXGISwapChain *_this,
                                                    UINT BufferCount,
                                                    UINT Width, UINT Height,
                                                    DXGI_FORMAT NewFormat,
                                                    UINT SwapChainFlags) {
  DX10Hook::Instance()->ResetRenderState();
  return (_this->*DX10Hook::Instance()->ResizeBuffers)(
      BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

DX10Hook::DX10Hook()
    : initialized(false), hooked(false), pDevice(nullptr),
      mainRenderTargetView(nullptr), Present(nullptr), ResizeBuffers(nullptr),
      ResizeTarget(nullptr) {
  _library = LoadLibrary(DX10_DLL);
}

DX10Hook::~DX10Hook() {
  PRINT_DEBUG("DX10 Hook removed\n");

  if (initialized) {
    mainRenderTargetView->Release();

    ImGui_ImplDX10_InvalidateDeviceObjects();
    ImGui::DestroyContext();

    initialized = false;
  }

  FreeLibrary(reinterpret_cast<HMODULE>(_library));

  _inst = nullptr;
}

DX10Hook *DX10Hook::Instance() {
  if (_inst == nullptr)
    _inst = new DX10Hook;

  return _inst;
}

const char *DX10Hook::GetLibName() const { return DX10_DLL; }

void DX10Hook::LoadFunctions(IDXGISwapChain *pSwapChain) {
  void **vTable;

  vTable = *reinterpret_cast<void ***>(pSwapChain);
#define LOAD_FUNC(X) (void *&)X = vTable[(int)IDXGISwapChainVTable::X]
  LOAD_FUNC(Present);
  LOAD_FUNC(ResizeBuffers);
  LOAD_FUNC(ResizeTarget);
#undef LOAD_FUNC
}
