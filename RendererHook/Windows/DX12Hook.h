#pragma once

#include "../BaseHook.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include "DX_VTables.h"

class DX12Hook : public BaseHook {
public:
#define DX12_DLL "d3d12.dll"

public:
  virtual ~DX12Hook();

  bool StartHook();
  static DX12Hook *Instance();
  virtual const char *GetLibName() const;

  void LoadFunctions(ID3D12CommandQueue *pCommandQueue,
                     IDXGISwapChain *pSwapChain);

private:
  // Functions
  DX12Hook();

  void ResetRenderState();
  void PrepareForOverlay(IDXGISwapChain *pSwapChain,
                         ID3D12CommandQueue *pCommandQueue);
  ID3D12CommandQueue *FindCommandQueueFromSwapChain(IDXGISwapChain *pSwapChain);

  // Hook to render functions
  static HRESULT STDMETHODCALLTYPE MyPresent(IDXGISwapChain *_this,
                                             UINT SyncInterval, UINT Flags);
  static HRESULT STDMETHODCALLTYPE MyResizeTarget(
      IDXGISwapChain *_this, const DXGI_MODE_DESC *pNewTargetParameters);
  static HRESULT STDMETHODCALLTYPE MyResizeBuffers(IDXGISwapChain *_this,
                                                   UINT BufferCount, UINT Width,
                                                   UINT Height,
                                                   DXGI_FORMAT NewFormat,
                                                   UINT SwapChainFlags);
  static void STDMETHODCALLTYPE
  MyExecuteCommandLists(ID3D12CommandQueue *_this, UINT NumCommandLists,
                        ID3D12CommandList *const *ppCommandLists);
  static HRESULT STDMETHODCALLTYPE
  MyPresent1(IDXGISwapChain1 *_this, UINT SyncInterval, UINT Flags,
             const DXGI_PRESENT_PARAMETERS *pPresentParameters);

  decltype(&IDXGISwapChain::Present) Present;
  decltype(&IDXGISwapChain::ResizeBuffers) ResizeBuffers;
  decltype(&IDXGISwapChain::ResizeTarget) ResizeTarget;
  decltype(&ID3D12CommandQueue::ExecuteCommandLists) ExecuteCommandLists;
  decltype(&IDXGISwapChain1::Present1) Present1;

private:
  static DX12Hook *_inst;

  struct DX12Frame_t {
    D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget = {};
    ID3D12CommandAllocator *pCmdAlloc = nullptr;
    ID3D12Resource *pBackBuffer = nullptr;

    inline void Reset() {
      pCmdAlloc = nullptr;
      pBackBuffer = nullptr;
    }

    DX12Frame_t(DX12Frame_t const &) = delete;
    DX12Frame_t &operator=(DX12Frame_t const &) = delete;

    DX12Frame_t(D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget,
                ID3D12CommandAllocator *pCmdAlloc, ID3D12Resource *pBackBuffer)
        : RenderTarget(RenderTarget), pCmdAlloc(pCmdAlloc),
          pBackBuffer(pBackBuffer) {}

    DX12Frame_t(DX12Frame_t &&other) noexcept
        : RenderTarget(other.RenderTarget), pCmdAlloc(other.pCmdAlloc),
          pBackBuffer(other.pBackBuffer) {
      other.Reset();
    }

    DX12Frame_t &operator=(DX12Frame_t &&other) noexcept {
      DX12Frame_t tmp(std::move(other));
      RenderTarget = tmp.RenderTarget;
      pCmdAlloc = tmp.pCmdAlloc;
      pBackBuffer = tmp.pBackBuffer;
      tmp.Reset();

      return *this;
    }

    ~DX12Frame_t() {
      if (pCmdAlloc != nullptr)
        pCmdAlloc->Release();
      if (pBackBuffer != nullptr)
        pBackBuffer->Release();
    }
  };

  // Variables
  bool hooked;
  bool initialized;

  size_t CommandQueueOffset;
  ID3D12CommandQueue *pCmdQueue;
  ID3D12Device *pDevice;
  std::vector<DX12Frame_t> OverlayFrames;
  // std::vector<bool> srvDescHeapBitmap;
  ID3D12DescriptorHeap *pSrvDescHeap;
  ID3D12GraphicsCommandList *pCmdList;
  ID3D12DescriptorHeap *pRtvDescHeap;
};
