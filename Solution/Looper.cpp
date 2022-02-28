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

	// ���͏��̍X�V
	Input::getInstance()->update();

	// ESC�ŏI��
	if (Input::getInstance()->hitKey(DIK_ESCAPE)) return true;

	// --------------------
	// �V�[���}�l�[�W���[�̍X�V
	// --------------------
	SceneManager::getInstange()->update();

	// --------------------
	// �V�[���}�l�[�W���[�̕`��
	// --------------------
	constexpr DirectX::XMFLOAT3 clearColor = { 0.1f, 0.25f, 0.5f };	//���ۂ��F
	DirectXCommon::getInstance()->startDraw(clearColor);

	SceneManager::getInstange()->draw();

	DirectXCommon::getInstance()->endDraw();
	

	return false;
}
