#pragma once

#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <d3d12.h>

class Model {
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

	// 頂点データ構造体
	struct Vertex {
		XMFLOAT3 pos; // xyz座標
		XMFLOAT3 normal; // 法線ベクトル
		XMFLOAT2 uv; // uv座標
	};

	// --------------------
	// 2d3d共通構造体
	// --------------------

	// パイプラインセット
	struct PipelineSet {
		// パイプラインステートオブジェクト
		ComPtr<ID3D12PipelineState> pipelinestate;
		// ルートシグネチャ
		ComPtr<ID3D12RootSignature> rootsignature;
	};



	// --------------------
	// static
	// --------------------
public:


	// --------------------
	// メンバ変数
	// --------------------
private:
	// --------------------
	// obj3d分け 不明(多分モデル)
	// --------------------
	// 定数バッファ用デスクリプターヒープ(デスクリプタを管理する)
	ComPtr<ID3D12DescriptorHeap> descHeap = nullptr;

	// --------------------
	// obj3d分け モデル
	// --------------------
	std::vector<Vertex> vertices{};
	ComPtr<ID3D12Resource> vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	Vertex* vertMap{};

	std::vector<unsigned short> indices{};
	ComPtr<ID3D12Resource> indexBuff;
	D3D12_INDEX_BUFFER_VIEW ibView;

	std::vector<ComPtr<ID3D12Resource>> texBuff;

	// 射影変換行列
	XMMATRIX matProjection;


	void loadModel(ID3D12Device* dev, std::vector<Vertex>& vertices, std::vector<unsigned short>& indices, const wchar_t* objPath,
		const int window_width, const int window_height,
		ComPtr<ID3D12Resource>& vertBuff, Vertex* vertMap, D3D12_VERTEX_BUFFER_VIEW& vbView,
		ComPtr<ID3D12Resource>& indexBuff, D3D12_INDEX_BUFFER_VIEW& ibView,
		XMMATRIX& matProjection);

	void loadSphere(ID3D12Device* dev, const float r, const int window_width, const int window_height);

public:

	void loadTexture(ID3D12Device* dev, const wchar_t* texPath, const UINT texNum);

	void setTexture(ID3D12Device* dev, UINT newTexNum);

	Model(ID3D12Device* dev,
		const wchar_t* objPath, const wchar_t* texPath,
		const int window_width, const int window_height,
		const unsigned int constantBufferNum,
		const int texNum);

	// 球体
	Model(ID3D12Device* dev,
		  const wchar_t* texPath,
		  const float r,
		  const int window_width, const int window_height,
		  const unsigned int constantBufferNum,
		  const int texNum);

	XMMATRIX getMatProjection();

	void update(XMMATRIX& matView);

	void draw(ID3D12Device* dev, ID3D12GraphicsCommandList* cmdList, ComPtr<ID3D12Resource> constBuff, const int constantBufferNum, const UINT texNum);
};

