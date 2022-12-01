#pragma once
#include "GameScene.h"
#include "Object3d.h"
#include "ObjModel.h"
#include "LevelLoader.h"

class LevelLoadScene :
	public GameScene
{
private:
	std::vector<std::unique_ptr<ObjModel>> models;
	std::vector<std::unique_ptr<Object3d>> objects;

	std::unique_ptr<Camera> camera;
	std::unique_ptr<Light> light;

	float eyeSpeed = 1.f;

public:
	LevelLoadScene();
	~LevelLoadScene();
	void update() override;
	void drawObj3d() override;
	void drawFrontSprite();
};

