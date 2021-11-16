#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl.h>	//ComPtr

#include "WinAPI.h"

class DirectXCommon {
	Microsoft::WRL::ComPtr<ID3D12Device> dev;
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;

	WinAPI* winapi = nullptr;

	void initDevice();
	void initCommand();
	void initSwapchain();
	void initRTV();
	void initDepthBuffer();
	void initFence();

	// 全画面クリア
	void ClearRenderTarget();
	// 深度バッファクリア
	void ClearDepthBuffer();

public:
	DirectXCommon(WinAPI* winapi);
	~DirectXCommon();

	void startDraw();
	void endDraw();

	ID3D12Device* getDev();
	ID3D12GraphicsCommandList* getCmdList();
};

