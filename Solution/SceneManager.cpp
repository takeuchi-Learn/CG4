#include "SceneManager.h"

#include "TitleScene.h"
#include "PlayScene.h"
#include "EndScene.h"

#include "Input.h"

SceneManager::SceneManager()
	: nextScene(SCENE_NUM::NONE) {

	nowScene = (GameScene*)new TitleScene();
}

SceneManager* SceneManager::getInstange() {
	static SceneManager sm;
	return &sm;
}


void SceneManager::init() {
	nowScene->init();
}

void SceneManager::update() {

	// 次のシーンがあったら
	if (nextScene != SCENE_NUM::NONE) {

		// 今のシーンを削除
		nowScene->fin();
		delete nowScene;

		// undone シーンを追加する際はここのcaseも追加
		switch (nextScene) {
		case SCENE_NUM::TITLE:
			nowScene = new TitleScene();
			break;
		case SCENE_NUM::PLAY:
			nowScene = new PlayScene();
			break;
		case SCENE_NUM::END:
			nowScene = new EndScene();
			break;
		}

		nextScene = SCENE_NUM::NONE;	//次シーンの情報をクリア
		nowScene->init();	//シーンを初期化
	}

	nowScene->update();
}

void SceneManager::drawObj3d() {
	nowScene->drawObj3d();
}

void SceneManager::drawFrontSprite() {
	nowScene->drawFrontSprite();
}

void SceneManager::fin() {
	nowScene->fin();

	if (nowScene != nullptr) {
		delete nowScene;
		nowScene = nullptr;
	}
}

void SceneManager::changeScene(const SCENE_NUM nextScene) {
	this->nextScene = nextScene;
}
