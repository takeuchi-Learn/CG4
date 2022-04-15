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

	struct VertexPosNormalUv {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
	};

private:
	std::string name;
	std::vector<Node> nodes;

	// メッシュを持つノード
	Node* meshNode = nullptr;

	// 頂点データ
	std::vector<VertexPosNormalUv> vertices;

	// 頂点インデックス
	std::vector<unsigned int> indices;

	DirectX::XMFLOAT3 ambient = { 1, 1, 1 };
	DirectX::XMFLOAT3 diffuse = { 1, 1, 1 };
	DirectX::TexMetadata metadata = {};
	DirectX::ScratchImage scratchImg = {};
};

