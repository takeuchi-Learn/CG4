#include "TitleScene.h"

#include "SceneManager.h"

#include "Input.h"

void TitleScene::update() {
	if (Input::getInstance()->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::PLAY);
	}
}

void TitleScene::draw() {

}
