#pragma once
#include "GameScene.h"

// undone シーンを追加する際は此処にシーン管理番号を追加する
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
	// ↓SceneMgr.h↓　シーン管理部

	static SceneManager* getInstange();

	void init() override;
	void update() override;
	void draw() override;
	void fin() override;

	void changeScene(const SCENE_NUM nextScene);
};

