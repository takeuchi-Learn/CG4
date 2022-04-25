#include "Light.h"
#include <cassert>
#include <d3dx12.h>

using namespace DirectX;

ID3D12Device* Light::dev = nullptr;

void Light::staticInit(ID3D12Device* dev) {
	assert(!Light::dev);
	assert(dev);
	Light::dev = dev;
}

Light::Light() {
	init();
}

void Light::transferConstBuffer() {
	HRESULT result = S_FALSE;
	//定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->dir2light = -lightDir;
		constMap->lightColor = lightColor;
		constBuff->Unmap(0, nullptr);
	}
}

void Light::init() {
	HRESULT result = S_FALSE;

	//定数バッファ生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);
	if (FAILED(result)) { assert(0); }

	// 定数バッファへデータ転送
	transferConstBuffer();
}

void Light::setLightDir(const DirectX::XMVECTOR& lightDir) {
	this->lightDir = XMVector3Normalize(lightDir);
	dirty = true;
}

void Light::setLightColor(const DirectX::XMFLOAT3& lightColor) {
	this->lightColor = lightColor;
	dirty = true;
}

void Light::update() {
	if (dirty) {
		transferConstBuffer();
		dirty = false;
	}
}

void Light::draw(DirectXCommon* dxCom, UINT rootParamIndex) {
	dxCom->getCmdList()->SetGraphicsRootConstantBufferView(rootParamIndex,
														   constBuff->GetGPUVirtualAddress());
}
