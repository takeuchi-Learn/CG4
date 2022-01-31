#include "TitleScene.h"

#include "SceneManager.h"

#include "Input.h"

#include "WinAPI.h"

void TitleScene::init() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : Title");
}

void TitleScene::update() {
	if (Input::getInstance()->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::PLAY);
	}
}

void TitleScene::draw() {

}
