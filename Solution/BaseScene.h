#pragma once
#include "GameScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include "FbxObj3d.h"
#include "ObjModel.h"
#include "FbxModel.h"
#include "Light.h"
#include "DX12Base.h"

#include <unordered_map>
#include <string>

#include <memory>
#include <DirectXMath.h>

class BaseScene
	: public GameScene {

protected:
	std::unordered_map<UINT, std::unique_ptr<Sprite>> frontSprite;

	std::unordered_map<std::string, std::unique_ptr<Object3d>> obj3d;
	std::unordered_map<std::string, std::unique_ptr<ObjModel>> objModel;

	std::unordered_map<std::string, std::unique_ptr<FbxObj3d>> fbxObj;
	std::unordered_map<std::string, std::unique_ptr<FbxModel>> fbxModel;

	std::unique_ptr<Light> light;

	std::unique_ptr<SpriteCommon> spCom;

	std::unique_ptr<Camera> camera;

public:
	BaseScene();

	void addFbxModel(const std::string &fbxModelName);

	void addFbxModel(const std::string &fbxModelName,
					 bool animLoop = true);

	void addObjModel(const std::string &dirPath,
					 const std::string &objModelName,
					 UINT texNum = 0u,
					 bool smoothing = false);

	void addObjObject(const std::string &objModelName,
					  UINT modelTexNum = 0u);

	// @return 画像の識別番号
	// @param pTexSize : 読み込んだ画像サイズを出力する変数のポインタ
	UINT addFrontSprite(wchar_t *filename,
						const DirectX::XMFLOAT2 &anchorPoint = { 0.5f, 0.5f },
						bool isFlipX = false,
						bool isFlipY = false,
						DirectX::XMFLOAT2 *pTexSize = nullptr);

	void update() override;
	void drawFrontSprite() override;
	void drawObj3d() override;
};

