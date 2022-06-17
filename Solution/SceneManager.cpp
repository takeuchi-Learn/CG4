#include "SceneManager.h"

#include "TitleScene.h"

#include "Input.h"

SceneManager::SceneManager()
	: nextScene(nullptr) {

	nowScene = (GameScene *)new TitleScene();
	nowScene->init();
}

SceneManager *SceneManager::getInstange() {
	static SceneManager sm;
	return &sm;
}


void SceneManager::update() {

	// ���̃V�[������������
	if (nextScene != nullptr) {

		// ���̃V�[�����폜���A���̃V�[���ɓ���ւ���
		delete nowScene;
		nowScene = nextScene;

		// ���̃V�[���̏���������
		nextScene->init();

		//���V�[���̏����N���A
		nextScene = nullptr;
	}

	nowScene->update();
}

void SceneManager::drawObj3d() {
	nowScene->drawObj3d();
}

void SceneManager::drawFrontSprite() {
	nowScene->drawFrontSprite();
}

SceneManager::~SceneManager() {
	if (nowScene != nullptr) {
		delete nowScene;
		nowScene = nullptr;
	}
}

void SceneManager::changeScene(GameScene* nextScene) {
	this->nextScene = nextScene;
}
