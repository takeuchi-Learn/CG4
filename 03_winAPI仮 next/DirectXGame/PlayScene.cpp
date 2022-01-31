#include "PlayScene.h"

#include "SceneManager.h"

#include "Input.h"

void PlayScene::init() {
}

void PlayScene::update() {
	if (Input::getInstance()->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::END);
	}
}

void PlayScene::draw() {
}

void PlayScene::fin() {
}
