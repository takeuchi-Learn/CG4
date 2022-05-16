#include "Looper.h"

#include "Input.h"

#include "DirectXCommon.h"
#include "SceneManager.h"

#include <DirectXMath.h>

#include "FbxLoader.h"

#include "Object3d.h"

#include "Light.h"

namespace {
	UINT whiteTexNum = 0u;
}

Looper::Looper() {

	FbxLoader::GetInstance()->init(DirectXCommon::getInstance()->getDev());


	Object3d::staticInit(DirectXCommon::getInstance()->getDev());

	Light::staticInit(DirectXCommon::getInstance()->getDev());

	SceneManager::getInstange()->init();

	spCom.reset(new SpriteCommon());

	whiteTexNum = spCom->loadTexture(L"Resources/white.png");

	postEffect.reset(new PostEffect(whiteTexNum, spCom.get(), { 0, 0 }));
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
	postEffect->startDrawScene(DirectXCommon::getInstance());
	SceneManager::getInstange()->draw();
	postEffect->endDrawScene(DirectXCommon::getInstance());

	constexpr DirectX::XMFLOAT3 clearColor = { 0.1f, 0.25f, 0.5f };	//���ۂ��F
	DirectXCommon::getInstance()->startDraw(clearColor);

	postEffect->draw(DirectXCommon::getInstance(), spCom.get());
	// todo �폜����
	//SceneManager::getInstange()->draw();

	DirectXCommon::getInstance()->endDraw();


	return false;
}
