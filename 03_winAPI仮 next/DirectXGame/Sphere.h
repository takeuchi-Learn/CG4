#pragma once

#include "Model.h"
#include "Object3d.h"
#include <DirectXMath.h>

class Sphere {
	Model* model = nullptr;
	Object3d* obj3d = nullptr;

public:
	float r;
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 scale;

	Sphere(ID3D12Device* dev, float r, const wchar_t* texPath, const UINT texNum,
		   DirectX::XMFLOAT3 pos = { 0, 0, 0 }, DirectX::XMFLOAT3 scale = { 1.f, 1.f, 1.f });
	~Sphere();

	static void sphereCommonBeginDraw(Object3d::PipelineSet& ppSet);

	void drawWithUpdate(DirectX::XMMATRIX& matView, DirectXCommon* dxCom);
};

