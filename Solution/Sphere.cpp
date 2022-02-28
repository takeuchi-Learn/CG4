#include "Sphere.h"

Sphere::Sphere(ID3D12Device* dev,
			   float r, const wchar_t* texPath, const UINT texNum,
			   DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale) : r(r), pos(pos), scale(scale) {
	model = new Model(dev, texPath, r, WinAPI::window_width, WinAPI::window_height, Object3d::constantBufferNum, texNum);
	obj3d = new Object3d(dev, model, texNum);

}

Sphere::~Sphere() {
	delete obj3d;
	delete model;
}

void Sphere::sphereCommonBeginDraw(Object3d::PipelineSet& ppSet) {
	Object3d::startDraw(DirectXCommon::getInstance()->getCmdList(), ppSet, D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sphere::drawWithUpdate(const DirectX::XMMATRIX & matView, DirectXCommon* dxCom) {
	obj3d->position = pos;
	obj3d->scale.x = scale.x * r;
	obj3d->scale.y = scale.y * r;
	obj3d->scale.z = scale.z * r;

	obj3d->drawWithUpdate(matView, dxCom);
}
