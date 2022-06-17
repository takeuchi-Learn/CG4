#include "Looper.h"

#include "Input.h"

#include "DXBase.h"
#include "SceneManager.h"

#include <DirectXMath.h>

#include "FbxLoader.h"

#include "Object3d.h"

#include "Light.h"

Looper::Looper() {

	FbxLoader::GetInstance()->init(DXBase::getInstance()->getDev());


	Object3d::staticInit(DXBase::getInstance()->getDev());

	Light::staticInit(DXBase::getInstance()->getDev());

	postEffect.reset(new PostEffect());
}

Looper::~Looper() {

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
	postEffect->startDrawScene(DXBase::getInstance());
	SceneManager::getInstange()->drawObj3d();
	postEffect->endDrawScene(DXBase::getInstance());

	constexpr DirectX::XMFLOAT3 clearColor = { 0.1f, 0.25f, 0.5f };	//青っぽい色
	DXBase::getInstance()->startDraw(clearColor);

	postEffect->draw(DXBase::getInstance());
	

	SceneManager::getInstange()->drawFrontSprite();

	DXBase::getInstance()->endDraw();


	return false;
}
