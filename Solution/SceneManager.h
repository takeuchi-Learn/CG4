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

	static SceneManager* getInstange();

	void init() override;
	void update() override;
	void drawObj3d() override;
	void drawFrontSprite() override;
	void fin() override;

	void changeScene(const SCENE_NUM nextScene);
};

