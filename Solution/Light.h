#pragma once

#include <wrl.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include "DirectXCommon.h"

class Light {
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	struct ConstBufferData {
		DirectX::XMVECTOR dir2light;	// ライトに向かうベクトル
		DirectX::XMFLOAT3 lightColor;	// ライト色
	};

private:
	static ID3D12Device* dev;

public:
	static void staticInit(ID3D12Device* dev);

private:
	ComPtr<ID3D12Resource> constBuff;	// 定数バッファ
	DirectX::XMVECTOR lightDir = { 1, 0, 0, 0 };	// 光線方向
	DirectX::XMFLOAT3 lightColor = { 1, 1, 1 };	// ライトの色
	bool dirty = false;

public:
	// 内部でinit関数を呼び出している
	Light();

	//定数バッファ転送
	void transferConstBuffer();

	void init();

	// 光線の方向をセット
	void setLightDir(const DirectX::XMVECTOR& lightDir);
	void setLightColor(const DirectX::XMFLOAT3& lightColor);

	void update();

	// @param rootParamIndex : Object3dクラスのcreateGraphicsPipeline関数内のrootParamsの要素数
	void draw(DirectXCommon* dxCom, UINT rootParamIndex);
};

