#include "EndScene.h"

#include "SceneManager.h"

#include "WinAPI.h"

void EndScene::init() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : End");

	input = Input::getInstance();

	spCom.reset(new SpriteCommon());

	// デバッグテキスト用のテクスチャ読み込み
	debugTextTexNumber = spCom->loadTexture(L"Resources/debugfont.png");

	debugText.reset(new DebugText(debugTextTexNumber, spCom.get()));
}

void EndScene::update() {
	if (input->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::TITLE);
	}

	debugText->Print(spCom.get(), "END", 0, 0, 10.f);
}

void EndScene::drawFrontSprite() {
	spCom->drawStart(DirectXCommon::getInstance()->getCmdList());
	debugText->DrawAll(DirectXCommon::getInstance(), spCom.get());
}

void EndScene::fin() {
}
