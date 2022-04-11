#pragma once

#include <string>

#include <DirectXMath.h>

#include <vector>

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

private:
	std::string name;
	std::vector<Node> nodes;
};

