#pragma once
#include "GameScene.h"

class TitleScene :
	public GameScene {
public:
	void init() override;
	void update() override;
	void draw() override;
};