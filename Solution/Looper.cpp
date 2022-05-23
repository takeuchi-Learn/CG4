#include "Looper.h"

#include "Input.h"

#include "DirectXCommon.h"
#include "SceneManager.h"

#include <DirectXMath.h>

#include "FbxLoader.h"

#include "Object3d.h"

#include "Light.h"

Looper::Looper() {

	FbxLoader::GetInstance()->init(DirectXCommon::getInstance()->getDev());


	Object3d::staticInit(DirectXCommon::getInstance()->getDev());

	Light::staticInit(DirectXCommon::getInstance()->getDev());

	SceneManager::getInstange()->init();

	postEffect.reset(new PostEffect());
}

Looper::~Looper() {
	SceneManager::getInstange()->fin();

	FbxLoader::GetInstance()->fin();
}

Looper *Looper::getInstance() {
	static Looper lpr;
	return &lpr;
}

bool Looper::loop() {

	// 入力情報の更新
	Input::getInstance()->update();

	// ESCで終了
	if (Input::getInstance()->hitKey(DIK_ESCAPE)) return true;

	// --------------------
	// シーンマネージャーの更新
	// --------------------
	SceneManager::getInstange()->update();

	// --------------------
	// シーンマネージャーの描画
	// --------------------
	postEffect->startDrawScene(DirectXCommon::getInstance());
	SceneManager::getInstange()->drawObj3d();
	postEffect->endDrawScene(DirectXCommon::getInstance());

	constexpr DirectX::XMFLOAT3 clearColor = { 0.1f, 0.25f, 0.5f };	//青っぽい色
	DirectXCommon::getInstance()->startDraw(clearColor);

	postEffect->draw(DirectXCommon::getInstance());
	

	SceneManager::getInstange()->drawFrontSprite();

	DirectXCommon::getInstance()->endDraw();


	return false;
}
