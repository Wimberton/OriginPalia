#pragma once
#include "../BaseHook.h"
#include <d3d9.h>
#include "DX_VTables.h"

class DX9Hook : public BaseHook
{
public:
#define DX9_DLL "d3d9.dll"
    virtual ~DX9Hook();

    bool StartHook();
    static DX9Hook* Instance();
    virtual const char* GetLibName() const;

    void LoadFunctions(IDirect3DDevice9* pDevice, bool ex);

private:
    DX9Hook();

    void ResetRenderState();
    void PrepareForOverlay(IDirect3DDevice9* pDevice);

    // Hook to render functions
    decltype(&IDirect3DDevice9::Reset)       Reset;
    decltype(&IDirect3DDevice9::EndScene)    EndScene;
    decltype(&IDirect3DDevice9::Present)     Present;
    decltype(&IDirect3DDevice9Ex::PresentEx) PresentEx;

    static HRESULT STDMETHODCALLTYPE MyReset(IDirect3DDevice9* _this, D3DPRESENT_PARAMETERS* pPresentationParameters);
    static HRESULT STDMETHODCALLTYPE MyEndScene(IDirect3DDevice9* _this);
    static HRESULT STDMETHODCALLTYPE MyPresent(IDirect3DDevice9* _this, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
    static HRESULT STDMETHODCALLTYPE MyPresentEx(IDirect3DDevice9Ex* _this, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);


private:
    static DX9Hook* _inst;
    
    bool hooked;
    bool initialized;
    bool uses_present;
};

