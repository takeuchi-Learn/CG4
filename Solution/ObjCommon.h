#pragma once

#include <DirectXMath.h>
#include <wrl.h>

#include <d3d12.h>

class ObjCommon {

public:
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// 定数バッファ用データ構造体
	struct ConstBufferData {
		XMFLOAT4 color; // 色 (RGBA)
		XMMATRIX mat;   // ３Ｄ変換行列
	};

	// パイプラインセット
	struct PipelineSet {
		// パイプラインステートオブジェクト
		ComPtr<ID3D12PipelineState> pipelinestate;
		// ルートシグネチャ
		ComPtr<ID3D12RootSignature> rootsignature;
	};
};

