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
		DirectX::XMVECTOR dir2light;	// ���C�g�Ɍ������x�N�g��
		DirectX::XMFLOAT3 lightColor;	// ���C�g�F
	};

private:
	static ID3D12Device* dev;

public:
	static void staticInit(ID3D12Device* dev);

private:
	ComPtr<ID3D12Resource> constBuff;	// �萔�o�b�t�@
	DirectX::XMVECTOR lightDir = { 1, 0, 0, 0 };	// ��������
	DirectX::XMFLOAT3 lightColor = { 1, 1, 1 };	// ���C�g�̐F
	bool dirty = false;

public:
	// ������init�֐����Ăяo���Ă���
	Light();

	//�萔�o�b�t�@�]��
	void transferConstBuffer();

	void init();

	// �����̕������Z�b�g
	void setLightDir(const DirectX::XMVECTOR& lightDir);
	void setLightColor(const DirectX::XMFLOAT3& lightColor);

	void update();

	// @param rootParamIndex : Object3d�N���X��createGraphicsPipeline�֐�����rootParams�̗v�f��
	void draw(DirectXCommon* dxCom, UINT rootParamIndex);
};
