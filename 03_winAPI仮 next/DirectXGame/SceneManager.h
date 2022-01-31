#pragma once
#include "GameScene.h"

// undone �V�[����ǉ�����ۂ͍����ɃV�[���Ǘ��ԍ���ǉ�����
enum class SCENE_NUM : unsigned short {
	NONE,
	TITLE, PLAY, END
};

class SceneManager
	: public GameScene {
private:
	SceneManager(const SceneManager& sm) = delete;
	SceneManager& operator=(const SceneManager& sm) = delete;
	SceneManager();

	GameScene* nowScene = nullptr;
	SCENE_NUM nextScene;

public:
	// https://dixq.net/g/sp_06.html
	// ��SceneMgr.h���@�V�[���Ǘ���

	static SceneManager* getInstange();

	void init() override;
	void update() override;
	void draw() override;
	void fin() override;

	void changeScene(const SCENE_NUM nextScene);
};

