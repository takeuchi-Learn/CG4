#include "SceneManager.h"

#include "TitleScene.h"

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

	// ���̃V�[������������
	if (nextScene != SCENE_NUM::NONE) {

		// ���̃V�[�����폜
		nowScene->fin();
		delete nowScene;

		// todo �v���C�A�G���h�V�[���ǉ�
		// undone �V�[����ǉ�����ۂ͂�����case���ǉ�
		switch (nextScene) {
		case SCENE_NUM::TITLE:
			nowScene = new TitleScene();
			break;
		case SCENE_NUM::PLAY:
			//nowScene = new PlayScene();
			break;
		case SCENE_NUM::END:
			//nowScene = new EndScene();
			break;
		}

		nextScene = SCENE_NUM::NONE;	//���V�[���̏����N���A
		nowScene->init();	//�V�[����������
	}

	nowScene->update();
}

void SceneManager::draw() {
	nowScene->draw();
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
