#pragma once

#include <string>

#include <DirectXMath.h>

#include <vector>

class FbxModel {
public:
	friend class FbxLoader;

	struct Node {
		std::string name;
		// ローカルスケール
		DirectX::XMVECTOR scaling = { 1,1,1,0 };
		// ローカル回転角
		DirectX::XMVECTOR rotation = { 0,0,0,0 };
		// ローカル移動
		DirectX::XMVECTOR translation = { 0,0,0,1 };
		// ローカル変形行列
		DirectX::XMMATRIX transform;
		// グローバル変形行列
		DirectX::XMMATRIX globalTransform;
		// 親ノード
		Node* parent = nullptr;
	};

private:
	std::string name;
	std::vector<Node> nodes;
};

