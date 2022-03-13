#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>

class Material {

public:
	// �萔�o�b�t�@�p�f�[�^�\����B1
	struct ConstBufferDataB1 {
		DirectX::XMFLOAT3 ambient; // �A���r�G���g
		float pad1; // �p�f�B���O
		DirectX::XMFLOAT3 diffuse; // �f�B�t���[�Y
		float pad2; // �p�f�B���O
		DirectX::XMFLOAT3 specular; // �X�y�L�����[
		float alpha;	// �A���t�@
	};

public:
	static void staticInit(ID3D12Device* dev);

private:
	static ID3D12Device* dev;



	// �e�N�X�`���o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff;
	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;

public:
	std::string name;
	DirectX::XMFLOAT3 ambient;
	DirectX::XMFLOAT3 diffuse;
	DirectX::XMFLOAT3 specular;
	float alpha;
	std::string texFilePath;

	Material();

	inline ID3D12Resource* getConstBuff() const { return constBuff.Get(); }

	void loadTexture(const std::string& texFilePath, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle);

	void update();

	inline const CD3DX12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() const { return cpuDescHandleSRV; }
	inline const CD3DX12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle() const { return gpuDescHandleSRV; }

private:
	void createConstBuff();
};

