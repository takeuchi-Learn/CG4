#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl.h>	//ComPtr

#include "WinAPI.h"

#include <DirectXMath.h>

class DirectXCommon {
	DirectXCommon(const DirectXCommon& dxcom) = delete;
	DirectXCommon& operator=(const DirectXCommon& dxcom) = delete;

	DirectXCommon();
	~DirectXCommon();

#pragma region private�����o�ϐ�

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

#pragma endregion

#pragma region FPS
	static const USHORT divNum = 8;
	LONGLONG fpsTime[divNum]{};
	float fps;

	void flipTimeFPS();
	void updateFPS();

public:
	float getFPS();

private:

#pragma endregion


	void initDevice();
	void initCommand();
	void initSwapchain();
	void initRTV();
	void initDepthBuffer();
	void initFence();

	// �S��ʃN���A
	void ClearRenderTarget(const DirectX::XMFLOAT3& clearColor);
	// �[�x�o�b�t�@�N���A
	void ClearDepthBuffer();

public:
	static DirectXCommon* getInstance();

	// @param clearColor �����Ȃ��ꏊ�̕`��F�B��������͈Â����F���ۂ��F
	void startDraw(const DirectX::XMFLOAT3& clearColor = DirectX::XMFLOAT3(0.5f, 0.5f, 0.f));
	void endDraw();

	ID3D12Device* getDev();
	ID3D12GraphicsCommandList* getCmdList();
};

