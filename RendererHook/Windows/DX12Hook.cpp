#include "DX12Hook.h"
#include "../RendererDetector.h"
#include "../ImGui/imgui.h"
#include "../ImGui/impls/windows/imgui_impl_dx12.h"
#include <dxgi1_4.h>
#include "WindowsHook.h"
#include "../OverlayBase.h"
#include "../Macros.h"

DX12Hook* DX12Hook::_inst = nullptr;

template<typename T>
static inline void SafeRelease(T*& pUnk)
{
	if (pUnk != nullptr)
	{
		pUnk->Release();
		pUnk = nullptr;
	}
}

ID3D12CommandQueue* DX12Hook::FindCommandQueueFromSwapChain(IDXGISwapChain* pSwapChain)
{
	ID3D12CommandQueue* pCommandQueue = nullptr;

	if (CommandQueueOffset == 0 && pCmdQueue != nullptr)
	{
		for (size_t i = 0; i < 1024; ++i)
		{
			if (*reinterpret_cast<ID3D12CommandQueue**>(reinterpret_cast<uintptr_t>(pSwapChain) + i) == pCmdQueue)
			{
				PRINT_DEBUG("Found IDXGISwapChain::ppCommandQueue at offset %zu.\n", i);
				CommandQueueOffset = i;
				break;
			}
		}
	}

	if (CommandQueueOffset != 0)
		pCommandQueue = *reinterpret_cast<ID3D12CommandQueue**>(reinterpret_cast<uintptr_t>(pSwapChain) + CommandQueueOffset);

	return pCommandQueue;
}

bool DX12Hook::StartHook()
{
	bool res = true;
	if (!hooked)
	{
		if (!WindowsHook::Instance()->StartHook())
			return false;

		PRINT_DEBUG("Hooked DirectX 12\n");
		hooked = true;

		RendererDetector::Instance().RendererFound(this);

		BeginHook();
		HookFuncs(
			std::make_pair<void**, void*>(&(PVOID&)DX12Hook::Present, &DX12Hook::MyPresent),
			std::make_pair<void**, void*>(&(PVOID&)DX12Hook::ResizeTarget, &DX12Hook::MyResizeTarget),
			std::make_pair<void**, void*>(&(PVOID&)DX12Hook::ResizeBuffers, &DX12Hook::MyResizeBuffers),
			std::make_pair<void**, void*>(&(PVOID&)DX12Hook::ExecuteCommandLists, &DX12Hook::MyExecuteCommandLists)
		);
		if (Present1 != nullptr)
		{
			HookFuncs(
				std::make_pair<void**, void*>(&(PVOID&)Present1, &DX12Hook::MyPresent1)
			);
		}
		EndHook();

		OverlayBase::Instance->HookReady();
	}
	return res;
}

void DX12Hook::ResetRenderState()
{
	if (initialized)
	{
		ImGui_ImplDX12_Shutdown();
		WindowsHook::Instance()->ResetRenderState();
		ImGui::DestroyContext();

		OverlayFrames.clear();

		SafeRelease(pSrvDescHeap);
		SafeRelease(pRtvDescHeap);
		SafeRelease(pDevice);

		initialized = false;
	}
}

// Try to make this function and overlay's proc as short as possible or it might affect game's fps.
void DX12Hook::PrepareForOverlay(IDXGISwapChain* pSwapChain, ID3D12CommandQueue* pCommandQueue)
{
	if (pCommandQueue == nullptr)
		return;

	IDXGISwapChain3* pSwapChain3 = nullptr;
	DXGI_SWAP_CHAIN_DESC sc_desc;
	pSwapChain->QueryInterface(IID_PPV_ARGS(&pSwapChain3));
	if (pSwapChain3 == nullptr)
		return;

	pSwapChain3->GetDesc(&sc_desc);

	if (!initialized)
	{
		UINT bufferIndex = pSwapChain3->GetCurrentBackBufferIndex();
		pDevice = nullptr;
		if (pSwapChain3->GetDevice(IID_PPV_ARGS(&pDevice)) != S_OK)
			return;

		UINT bufferCount = sc_desc.BufferCount;

		//srvDescHeapBitmap.clear();

		//constexpr UINT descriptor_count = 1024;

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = 1;
			//desc.NumDescriptors = descriptor_count;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			if (pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pSrvDescHeap)) != S_OK)
			{
				pDevice->Release();
				pSwapChain3->Release();
				return;
			}
		}

		//srvDescHeapBitmap.resize(descriptor_count, false);

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NumDescriptors = bufferCount;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 1;
			if (pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pRtvDescHeap)) != S_OK)
			{
				pDevice->Release();
				pSwapChain3->Release();
				pSrvDescHeap->Release();
				return;
			}

			SIZE_T rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
			ID3D12CommandAllocator* pCmdAlloc;
			ID3D12Resource* pBackBuffer;

			for (UINT i = 0; i < bufferCount; ++i)
			{
				pCmdAlloc = nullptr;
				pBackBuffer = nullptr;

				if (pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCmdAlloc)) != S_OK || pCmdAlloc == nullptr)
				{
					OverlayFrames.clear();
					pSrvDescHeap->Release();
					pRtvDescHeap->Release();
					pDevice->Release();
					pSwapChain3->Release();
					return;
				}

				if (i == 0)
				{
					if (pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCmdAlloc, NULL, IID_PPV_ARGS(&pCmdList)) != S_OK ||
						pCmdList == nullptr || pCmdList->Close() != S_OK)
					{
						OverlayFrames.clear();
						SafeRelease(pCmdList);
						pCmdAlloc->Release();
						pSrvDescHeap->Release();
						pRtvDescHeap->Release();
						pDevice->Release();
						pSwapChain3->Release();
						return;
					}
				}

				if (pSwapChain3->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer)) != S_OK || pBackBuffer == nullptr)
				{
					OverlayFrames.clear();
					pCmdList->Release();
					pCmdAlloc->Release();
					pSrvDescHeap->Release();
					pRtvDescHeap->Release();
					pDevice->Release();
					pSwapChain3->Release();
					return;
				}

				pDevice->CreateRenderTargetView(pBackBuffer, NULL, rtvHandle);

				OverlayFrames.emplace_back(rtvHandle, pCmdAlloc, pBackBuffer);
				rtvHandle.ptr += rtvDescriptorSize;
			}
		}

		//auto heaps = std::move(get_free_texture_heap());


		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = NULL;

		ImGui_ImplDX12_Init(pDevice, bufferCount, DXGI_FORMAT_R8G8B8A8_UNORM, NULL,
			pSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			pSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

		OverlayBase::Instance->CreateFonts();

		initialized = true;
	}

	if (ImGui_ImplDX12_NewFrame())
	{
		WindowsHook::Instance()->PrepareForOverlay(sc_desc.OutputWindow);

		ImGui::NewFrame();

		OverlayBase::Instance->OverlayProc();

		UINT bufferIndex = pSwapChain3->GetCurrentBackBufferIndex();
		OverlayFrames[bufferIndex].pCmdAlloc->Reset();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = OverlayFrames[bufferIndex].pBackBuffer;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		pCmdList->Reset(OverlayFrames[bufferIndex].pCmdAlloc, NULL);
		pCmdList->ResourceBarrier(1, &barrier);
		pCmdList->OMSetRenderTargets(1, &OverlayFrames[bufferIndex].RenderTarget, FALSE, NULL);
		pCmdList->SetDescriptorHeaps(1, &pSrvDescHeap);

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCmdList);

		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		pCmdList->ResourceBarrier(1, &barrier);
		pCmdList->Close();

		pCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&pCmdList);
	}
	pSwapChain3->Release();
}

HRESULT STDMETHODCALLTYPE DX12Hook::MyPresent(IDXGISwapChain* _this, UINT SyncInterval, UINT Flags)
{
	ID3D12CommandQueue* pCommandQueue = DX12Hook::Instance()->FindCommandQueueFromSwapChain(_this);
	if (pCommandQueue != nullptr) {
		DX12Hook::Instance()->PrepareForOverlay(_this, pCommandQueue);
	}
	return (_this->*DX12Hook::Instance()->Present)(SyncInterval, Flags);
}

HRESULT STDMETHODCALLTYPE DX12Hook::MyResizeTarget(IDXGISwapChain* _this, const DXGI_MODE_DESC* pNewTargetParameters)
{
	DX12Hook::Instance()->ResetRenderState();
	return (_this->*DX12Hook::Instance()->ResizeTarget)(pNewTargetParameters);
}

HRESULT STDMETHODCALLTYPE DX12Hook::MyResizeBuffers(IDXGISwapChain* _this, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	DX12Hook::Instance()->ResetRenderState();
	return (_this->*DX12Hook::Instance()->ResizeBuffers)(BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

void STDMETHODCALLTYPE DX12Hook::MyExecuteCommandLists(ID3D12CommandQueue* _this, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists)
{
	DX12Hook* me = DX12Hook::Instance();
	me->pCmdQueue = _this;

	(_this->*DX12Hook::Instance()->ExecuteCommandLists)(NumCommandLists, ppCommandLists);
}

HRESULT STDMETHODCALLTYPE DX12Hook::MyPresent1(IDXGISwapChain1* _this, UINT SyncInterval, UINT Flags, const DXGI_PRESENT_PARAMETERS* pPresentParameters)
{
	ID3D12CommandQueue* pCommandQueue = DX12Hook::Instance()->FindCommandQueueFromSwapChain(_this);
	if (pCommandQueue != nullptr) {
		DX12Hook::Instance()->PrepareForOverlay(_this, pCommandQueue);
	}
	return (_this->*DX12Hook::Instance()->Present1)(SyncInterval, Flags, pPresentParameters);
}

DX12Hook::DX12Hook() :
	hooked(false),
	initialized(false),
	CommandQueueOffset(0),
	pCmdQueue(nullptr),
	pDevice(nullptr),
	pSrvDescHeap(nullptr),
	pCmdList(nullptr),
	pRtvDescHeap(nullptr),
	Present(nullptr),
	ResizeBuffers(nullptr),
	ResizeTarget(nullptr),
	ExecuteCommandLists(nullptr),
	Present1(nullptr)
{
	_library = LoadLibrary(DX12_DLL);
}

DX12Hook::~DX12Hook()
{
	PRINT_DEBUG("DX12 Hook removed\n");

	if (initialized)
	{
		OverlayFrames.clear();

		pSrvDescHeap->Release();
		pRtvDescHeap->Release();

		ImGui_ImplDX12_InvalidateDeviceObjects();
		ImGui::DestroyContext();

		initialized = false;
	}

	FreeLibrary(reinterpret_cast<HMODULE>(_library));

	_inst = nullptr;
}

DX12Hook* DX12Hook::Instance()
{
	if (_inst == nullptr)
		_inst = new DX12Hook();

	return _inst;
}

const char* DX12Hook::GetLibName() const
{
	return DX12_DLL;
}

void DX12Hook::LoadFunctions(ID3D12CommandQueue* pCommandQueue, IDXGISwapChain* pSwapChain)
{
	void** vTable;

	vTable = *reinterpret_cast<void***>(pCommandQueue);
#define LOAD_FUNC(X) (void*&)X = vTable[(int)ID3D12CommandQueueVTable::X]
	LOAD_FUNC(ExecuteCommandLists);
#undef LOAD_FUNC

	vTable = *reinterpret_cast<void***>(pSwapChain);
#define LOAD_FUNC(X) (void*&)X = vTable[(int)IDXGISwapChainVTable::X]
	LOAD_FUNC(Present);
	LOAD_FUNC(ResizeBuffers);
	LOAD_FUNC(ResizeTarget);
	LOAD_FUNC(Present1);

#undef LOAD_FUNC
}