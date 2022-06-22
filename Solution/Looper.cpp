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
}

Looper::~Looper() {

	FbxLoader::GetInstance()->fin();
}

Looper *Looper::getInstance() {
	static Looper lpr;
	return &lpr;
}

bool Looper::loopUpdate() {
	// 入力情報の更新
	Input::getInstance()->update();

	// ESCで終了
	if (Input::getInstance()->hitKey(DIK_ESCAPE)) return true;

	// --------------------
	// シーンマネージャーの更新
	// --------------------
	SceneManager::getInstange()->update();

	return false;
}

bool Looper::loopDraw() {
	// --------------------
	// シーンマネージャーの描画
	// --------------------
	PostEffect::getInstance()->startDrawScene(DXBase::getInstance());
	SceneManager::getInstange()->drawObj3d();
	PostEffect::getInstance()->endDrawScene(DXBase::getInstance());

	constexpr DirectX::XMFLOAT3 clearColor = { 0.f, 0.f, 0.f };	//黒色
	DXBase::getInstance()->startDraw(clearColor);

	PostEffect::getInstance()->draw(DXBase::getInstance());


	SceneManager::getInstange()->drawFrontSprite();

	DXBase::getInstance()->endDraw();

	return false;
}

bool Looper::loop() {
	if (loopUpdate()) return true;

	if (loopDraw()) return true;

	return false;
}
