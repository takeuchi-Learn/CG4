#pragma once

#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <d3d12.h>

#include "Model.h"
#include "DirectXCommon.h"

class Object3d {
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	enum class BLEND_MODE : short {
		ALPHA,
		ADD,
		SUB,
		REVERSE
	};

	//パイプラインセット
	struct PipelineSet {
		//パイプラインステートオブジェクト
		ComPtr<ID3D12PipelineState> pipelinestate;
		//ルートシグネチャ
		ComPtr<ID3D12RootSignature> rootsignature;
	};

	// 頂点データ構造体
	//struct Vertex {
	//	XMFLOAT3 pos; // xyz座標
	//	XMFLOAT3 normal; // 法線ベクトル
	//	XMFLOAT2 uv; // uv座標
	//};

	// 定数バッファ用データ構造体
	struct ConstBufferData {
		XMFLOAT4 color; // 色
		XMMATRIX mat; // 行列
	};

	// --------------------
	// staticメンバ
	// --------------------
private:
	static void createTransferBuffer(ID3D12Device* dev, ComPtr<ID3D12Resource>& constBuff);

	inline XMFLOAT3 subFloat3(const XMFLOAT3& left, const XMFLOAT3& right) {
		return XMFLOAT3(left.x - right.x,
						left.y - right.y,
						left.z - right.z);
	}

public:
	// 頂点バッファの最大数
	static const int constantBufferNum = 128;

	static void startDraw(ID3D12GraphicsCommandList* cmdList, Object3d::PipelineSet& ppSet,
										D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//3Dオブジェクト用パイプライン生成
	// シェーダーモデル指定は "*s_5_0"
	static Object3d::PipelineSet createGraphicsPipeline(ID3D12Device* dev,
														BLEND_MODE blendMode = BLEND_MODE::ALPHA,
														const wchar_t* vsShaderPath = L"Resources/Shaders/BasicVS.hlsl",
														const wchar_t* psShaderPath = L"Resources/Shaders/BasicPS.hlsl");

	// --------------------
	// (動的)メンバ
	// --------------------
private:
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	// ワールド変換行列
	XMMATRIX matWorld;

public:
	UINT texNum = 0;

	XMFLOAT4 color = { 1, 1, 1, 1 };

	// アフィン変換情報
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };
	// 親オブジェクトへのポインタ
	Object3d* parent = nullptr;

	//モデルデータ
	Model* model = nullptr;

	XMMATRIX getMatWorld() const;

	//void setTexture(ID3D12Device* dev, const UINT newTexNum);


	// モデルは後から手動で読み込む(deleteも手動)
	Object3d(ID3D12Device* dev);

	// モデルデータもここで渡す(deleteは手動)
	Object3d(ID3D12Device* dev, Model* model, const UINT texNum);

	// ライト->オブジェクト
	inline void setLightDir(XMFLOAT3 light) { model->setLightDir(light); }
	inline XMFLOAT3 getLightDir() { return model->getLightDir(); }

	inline void setLightPos(XMFLOAT3 lightPos) {
		setLightDir(subFloat3(position, lightPos));
	}

	void update(const XMMATRIX& matView);

	void draw(DirectXCommon* dxCom);

	void drawWithUpdate(const XMMATRIX& matView, DirectXCommon* dxCom);


	~Object3d();
};

