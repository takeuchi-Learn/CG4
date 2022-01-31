#include "EndScene.h"

#include "SceneManager.h"

#include "Input.h"

#include "WinAPI.h"

void EndScene::init() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : End");
}

void EndScene::update() {
	if (Input::getInstance()->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::TITLE);
	}
}

void EndScene::draw() {
}

void EndScene::fin() {
}
