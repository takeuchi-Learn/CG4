#include "EndScene.h"

#include "SceneManager.h"

#include "WinAPI.h"

#include "TitleScene.h"

EndScene::EndScene() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : End");

	input = Input::getInstance();

	spCom.reset(new SpriteCommon());

	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ǂݍ���
	debugTextTexNumber = spCom->loadTexture(L"Resources/debugfont.png");

	debugText.reset(new DebugText(debugTextTexNumber, spCom.get()));
}

void EndScene::update() {
	if (input->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(new TitleScene());
	}

	debugText->Print(spCom.get(), "END", 0, 0, 10.f);
}

void EndScene::drawFrontSprite() {
	spCom->drawStart(DirectXCommon::getInstance()->getCmdList());
	debugText->DrawAll(DirectXCommon::getInstance(), spCom.get());
}
