#pragma once

#include <string>

#include <DirectXMath.h>

#include <vector>

#include <DirectXTex.h>

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

class FbxModel {
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;
	using string = std::string;
	template <class T> using vector = std::vector<T>;

public:
	friend class FbxLoader;

	struct Node {
		std::string name;
		// ���[�J���X�P�[��
		DirectX::XMVECTOR scaling = { 1,1,1,0 };
		// ���[�J����]�p
		DirectX::XMVECTOR rotation = { 0,0,0,0 };
		// ���[�J���ړ�
		DirectX::XMVECTOR translation = { 0,0,0,1 };
		// ���[�J���ό`�s��
		DirectX::XMMATRIX transform;
		// �O���[�o���ό`�s��
		DirectX::XMMATRIX globalTransform;
		// �e�m�[�h
		Node* parent = nullptr;
	};

	struct VertexPosNormalUv {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
	};

private:
	std::string name;
	std::vector<Node> nodes;

	// ���b�V�������m�[�h
	Node* meshNode = nullptr;

	// ���_�f�[�^
	std::vector<VertexPosNormalUv> vertices;

	// ���_�C���f�b�N�X
	std::vector<unsigned int> indices;

	DirectX::XMFLOAT3 ambient = { 1, 1, 1 };
	DirectX::XMFLOAT3 diffuse = { 1, 1, 1 };
	DirectX::TexMetadata metadata = {};
	DirectX::ScratchImage scratchImg = {};

	ComPtr<ID3D12Resource> vertBuff;
	ComPtr<ID3D12Resource> indexBuff;
	ComPtr<ID3D12Resource> texBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

public:
	void createBuffers(ID3D12Device* dev);

	void draw(ID3D12GraphicsCommandList* cmdList);

	const XMMATRIX& GetModelTransform() { return meshNode->globalTransform; }
};

