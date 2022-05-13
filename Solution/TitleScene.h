#pragma once
#include "GameScene.h"

#include "DebugText.h"

#include "Input.h"

#include <memory>

class TitleScene :
	public GameScene {

	// --------------------
	// デバッグテキスト
	// --------------------
	std::unique_ptr<SpriteCommon> spCom;
	std::unique_ptr<DebugText> debugText;
	// デバッグテキスト用のテクスチャ番号
	UINT debugTextTexNumber;

	Input* input = nullptr;

public:
	void init() override;
	void update() override;
	void draw() override;
};