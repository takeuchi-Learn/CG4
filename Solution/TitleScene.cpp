#include "TitleScene.h"

#include "SceneManager.h"

#include "WinAPI.h"

#include "PlayScene.h"

TitleScene::TitleScene()
	: titleStrPos(0.f, 0.f),
	update_proc(&TitleScene::update_normal) {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : Title");

	input = Input::getInstance();

	spCom.reset(new SpriteCommon());

	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ǂݍ���
	debugTextTexNumber = spCom->loadTexture(L"Resources/debugfont.png");

	debugText.reset(new DebugText(debugTextTexNumber, spCom.get()));
}

void TitleScene::update() {
	(this->*update_proc)();
	debugText->Print(spCom.get(), "TITLE", titleStrPos.x, titleStrPos.y, 10.f);
}

void TitleScene::update_normal() {
	if (input->triggerKey(DIK_SPACE)) {
		update_proc = &TitleScene::update_end;
	}
}

void TitleScene::update_end() {
	titleStrPos.y += 20.f;

	if (titleStrPos.y > WinAPI::window_height) {
		SceneManager::getInstange()->changeScene(new PlayScene());
	}
}

void TitleScene::drawFrontSprite() {
	spCom->drawStart(DXBase::getInstance()->getCmdList());
	debugText->DrawAll(DXBase::getInstance(), spCom.get());
}
