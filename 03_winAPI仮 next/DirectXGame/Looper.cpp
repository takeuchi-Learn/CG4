#include "Looper.h"

#include "Input.h"

#include "DirectXCommon.h"
#include "SceneManager.h"

#include <DirectXMath.h>

Looper::Looper() {
	SceneManager::getInstange()->init();
}

Looper::~Looper() {
	SceneManager::getInstange()->fin();
}

Looper* Looper::getInstance() {
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
	static constexpr DirectX::XMFLOAT3 clearColor = { 0.1f, 0.25f, 0.5f };	//青っぽい色
	DirectXCommon::getInstance()->startDraw(clearColor);

	SceneManager::getInstange()->draw();

	DirectXCommon::getInstance()->endDraw();
	

	return false;
}
