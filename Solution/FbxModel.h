#pragma once

#include <string>

#include <DirectXMath.h>

#include <vector>

#include <DirectXTex.h>

class FbxModel {
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
};

