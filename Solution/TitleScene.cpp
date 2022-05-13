#include "TitleScene.h"

#include "SceneManager.h"

#include "WinAPI.h"

void TitleScene::init() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : Title");

	input = Input::getInstance();

	spCom.reset(new SpriteCommon());

	// デバッグテキスト用のテクスチャ読み込み
	debugTextTexNumber = spCom->loadTexture(L"Resources/debugfont.png");

	debugText.reset(new DebugText(debugTextTexNumber, spCom.get()));
}

void TitleScene::update() {
	if (input->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::PLAY);
	}
	debugText->Print(spCom.get(), "TITLE", 0, 0, 10.f);
}

void TitleScene::draw() {
	spCom->drawStart(DirectXCommon::getInstance()->getCmdList());
	debugText->DrawAll(DirectXCommon::getInstance(), spCom.get());
}
