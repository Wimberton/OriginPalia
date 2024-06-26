#pragma once
#include "../BaseHook.h"
#include <d3d11.h>
#include "DX_VTables.h"

class DX11Hook :public BaseHook
{
public:
#define DX11_DLL "d3d11.dll"
public:
    virtual ~DX11Hook();

    bool StartHook();
    static DX11Hook* Instance();
    virtual const char* GetLibName() const;

    void LoadFunctions(IDXGISwapChain* pSwapChain);
private:

    DX11Hook();

    void ResetRenderState();
    void PrepareForOverlay(IDXGISwapChain* pSwapChain);

    // Hook to render functions
    static HRESULT STDMETHODCALLTYPE MyPresent(IDXGISwapChain* _this, UINT SyncInterval, UINT Flags);
    static HRESULT STDMETHODCALLTYPE MyResizeTarget(IDXGISwapChain* _this, const DXGI_MODE_DESC* pNewTargetParameters);
    static HRESULT STDMETHODCALLTYPE MyResizeBuffers(IDXGISwapChain* _this, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

    decltype(&IDXGISwapChain::Present)       Present;
    decltype(&IDXGISwapChain::ResizeBuffers) ResizeBuffers;
    decltype(&IDXGISwapChain::ResizeTarget)  ResizeTarget;

private:
    static DX11Hook* _inst;

    // Variables
    bool hooked;
    bool initialized;
    ID3D11DeviceContext* pContext;
    ID3D11RenderTargetView* mainRenderTargetView;


};

