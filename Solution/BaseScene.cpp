#include "BaseScene.h"
#include "DX12Base.h"
#include "FbxLoader.h"

BaseScene::BaseScene()
	: spCom(new SpriteCommon()),
	light(new Light()),
	camera(new Camera(WinAPI::window_width, WinAPI::window_height)) {
}

void BaseScene::addFbxModel(const std::string &fbxModelName) {
	fbxModel[fbxModelName].reset(FbxLoader::GetInstance()->loadModelFromFile(fbxModelName));
}

void BaseScene::addFbxModel(const std::string &fbxModelName, bool animLoop) {
	fbxObj[fbxModelName].reset(new FbxObj3d(animLoop));
}

void BaseScene::addObjModel(const std::string &dirPath,
							const std::string &objModelName,
							UINT texNum,
							bool smoothing) {
	objModel[objModelName].reset(new ObjModel(dirPath, objModelName, texNum, smoothing));
}

void BaseScene::addObjObject(const std::string &objModelName, UINT modelTexNum) {
	obj3d[objModelName].reset(new Object3d(DX12Base::getInstance()->getDev(),
										   camera.get(),
										   objModel[objModelName].get(),
										   modelTexNum));
}

UINT BaseScene::addFrontSprite(wchar_t *filename,
							   const DirectX::XMFLOAT2 &anchorPoint,
							   bool isFlipX, bool isFlipY,
							   DirectX::XMFLOAT2 *pTexSize) {
	const UINT texNum = spCom->loadTexture(filename, pTexSize);
	frontSprite[texNum].reset(new Sprite(texNum, spCom.get(), anchorPoint, isFlipX, isFlipY));

	return texNum;
}

void BaseScene::update() {
}

void BaseScene::drawFrontSprite() {
	for (auto &i : frontSprite) {
		i.second->drawWithUpdate(DX12Base::getInstance(), spCom.get());
	}
}

void BaseScene::drawObj3d() {
	camera->update();

	for (auto &i : obj3d) {
		i.second->drawWithUpdate(DX12Base::getInstance(), light.get());
	}

	for (auto &i : fbxObj) {
		i.second->drawWithUpdate(DX12Base::getInstance()->getCmdList(), light.get());
	}
}
