#include "EndScene.h"

#include "SceneManager.h"

#include "Input.h"

void EndScene::init() {
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
