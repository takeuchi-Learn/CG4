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
	postEffect->startDrawScene(DXBase::getInstance());
	SceneManager::getInstange()->drawObj3d();
	postEffect->endDrawScene(DXBase::getInstance());

	constexpr DirectX::XMFLOAT3 clearColor = { 0.1f, 0.25f, 0.5f };	//���ۂ��F
	DXBase::getInstance()->startDraw(clearColor);

	postEffect->draw(DXBase::getInstance());
	

	SceneManager::getInstange()->drawFrontSprite();

	DXBase::getInstance()->endDraw();


	return false;
}
