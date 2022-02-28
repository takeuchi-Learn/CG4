#include "TitleScene.h"

#include "SceneManager.h"

#include "WinAPI.h"

void TitleScene::init() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : Title");

	input = Input::getInstance();

	spCom = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(),
									   WinAPI::window_width, WinAPI::window_height);

	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ǂݍ���
	Sprite::commonLoadTexture(spCom, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());

	debugText.Initialize(DirectXCommon::getInstance()->getDev(),
						 WinAPI::window_width, WinAPI::window_height,
						debugTextTexNumber, spCom);
}

void TitleScene::update() {
	if (input->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::PLAY);
	}
	debugText.Print(spCom, "TITLE", 0, 0, 10.f);
}

void TitleScene::draw() {
	Sprite::drawStart(spCom, DirectXCommon::getInstance()->getCmdList());
	debugText.DrawAll(DirectXCommon::getInstance(), spCom);
}
