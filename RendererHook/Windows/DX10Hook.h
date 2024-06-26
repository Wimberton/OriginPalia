#pragma once
#include "../BaseHook.h"
#include <d3d10.h>
#include "DX_VTables.h"

class DX10Hook :
	public BaseHook
{
public:
#define DX10_DLL "d3d10.dll"

	virtual ~DX10Hook();

	bool StartHook();
	static DX10Hook* Instance();
	virtual const char* GetLibName() const;

	void LoadFunctions(IDXGISwapChain* pSwapChain);
private:
    // Functions
    DX10Hook();

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
    static DX10Hook* _inst;

    bool hooked;
    bool initialized;
    ID3D10Device* pDevice;
    ID3D10RenderTargetView* mainRenderTargetView;

};

