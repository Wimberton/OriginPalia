#include "DX9Hook.h"
#include "WindowsHook.h"
#include "../OverlayBase.h"
#include "../RendererDetector.h"
#include "../ImGui/imgui.h"
#include "../ImGui/impls/windows/imgui_impl_dx9.h"
#include "../Macros.h"

DX9Hook *DX9Hook::_inst = nullptr;

bool DX9Hook::StartHook() {
    if (!hooked) {
        if (!WindowsHook::Instance()->StartHook())
            return false;

        PRINT_DEBUG("Hooked DirectX 9\n");
        hooked = true;

        RendererDetector::Instance().RendererFound(this);

        BeginHook();
        HookFuncs(std::make_pair<void **, void *>(&(PVOID &)Reset, &DX9Hook::MyReset),
                  std::make_pair<void **, void *>(&(PVOID &)Present, &DX9Hook::MyPresent));
        if (PresentEx != nullptr) {
            HookFuncs(std::make_pair<void **, void *>(&(PVOID &)PresentEx, &DX9Hook::MyPresentEx)
                      // std::make_pair<void**, void*>(&(PVOID&)EndScene,
                      // &DX9Hook::MyEndScene)
            );
        }
        EndHook();

        OverlayBase::Instance->HookReady();
    }
    return true;
}

void DX9Hook::ResetRenderState() {
    if (initialized) {
        initialized = false;
        ImGui_ImplDX9_Shutdown();
        WindowsHook::Instance()->ResetRenderState();
        ImGui::DestroyContext();
    }
}

// Try to make this function and overlay's proc as short as possible or it might
// affect game's fps.
void DX9Hook::PrepareForOverlay(IDirect3DDevice9 *pDevice) {
    D3DDEVICE_CREATION_PARAMETERS param;
    pDevice->GetCreationParameters(&param);

    // Workaround to detect if we changed window.
    if (param.hFocusWindow != WindowsHook::Instance()->GetGameHwnd())
        ResetRenderState();

    if (!initialized) {
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.IniFilename = NULL;

        ImGui_ImplDX9_Init(pDevice);

        OverlayBase::Instance->CreateFonts();

        initialized = true;
    }

    if (ImGui_ImplDX9_NewFrame()) {
        WindowsHook::Instance()->PrepareForOverlay(param.hFocusWindow);

        ImGui::NewFrame();

        OverlayBase::Instance->OverlayProc();

        ImGui::Render();

        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }
}

HRESULT STDMETHODCALLTYPE DX9Hook::MyReset(IDirect3DDevice9 *_this, D3DPRESENT_PARAMETERS *pPresentationParameters) {
    DX9Hook::Instance()->ResetRenderState();
    return (_this->*DX9Hook::Instance()->Reset)(pPresentationParameters);
}

HRESULT STDMETHODCALLTYPE DX9Hook::MyEndScene(IDirect3DDevice9 *_this) {
    if (!DX9Hook::Instance()->uses_present)
        DX9Hook::Instance()->PrepareForOverlay(_this);
    return (_this->*DX9Hook::Instance()->EndScene)();
}

HRESULT STDMETHODCALLTYPE DX9Hook::MyPresent(IDirect3DDevice9 *_this, CONST RECT *pSourceRect, CONST RECT *pDestRect,
                                             HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion) {
    DX9Hook::Instance()->uses_present = true;
    DX9Hook::Instance()->PrepareForOverlay(_this);
    return (_this->*DX9Hook::Instance()->Present)(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT STDMETHODCALLTYPE DX9Hook::MyPresentEx(IDirect3DDevice9Ex *_this, CONST RECT *pSourceRect,
                                               CONST RECT *pDestRect, HWND hDestWindowOverride,
                                               CONST RGNDATA *pDirtyRegion, DWORD dwFlags) {
    DX9Hook::Instance()->uses_present = true;
    DX9Hook::Instance()->PrepareForOverlay(_this);
    return (_this->*DX9Hook::Instance()->PresentEx)(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}

DX9Hook::DX9Hook()
    : initialized(false), hooked(false), uses_present(false), EndScene(nullptr), Present(nullptr), PresentEx(nullptr),
      Reset(nullptr) {
    _library = LoadLibrary(DX9_DLL);
}

DX9Hook::~DX9Hook() {
    PRINT_DEBUG("DX9 Hook removed\n");

    if (initialized) {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        ImGui::DestroyContext();
    }

    FreeLibrary(reinterpret_cast<HMODULE>(_library));

    _inst = nullptr;
}

DX9Hook *DX9Hook::Instance() {
    if (_inst == nullptr)
        _inst = new DX9Hook;

    return _inst;
}

const char *DX9Hook::GetLibName() const { return DX9_DLL; }

void DX9Hook::LoadFunctions(IDirect3DDevice9 *pDevice, bool ex) {
    void **vTable = *reinterpret_cast<void ***>(pDevice);

#define LOAD_FUNC(X) (void *&)X = vTable[(int)IDirect3DDevice9VTable::X]
    LOAD_FUNC(Reset);
    LOAD_FUNC(EndScene);
    LOAD_FUNC(Present);
    if (ex)
        LOAD_FUNC(PresentEx);
#undef LOAD_FUNC
}
