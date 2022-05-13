#pragma once
#include "GameScene.h"

#include "DebugText.h"

#include "Input.h"

#include <memory>

class TitleScene :
	public GameScene {

	// --------------------
	// �f�o�b�O�e�L�X�g
	// --------------------
	std::unique_ptr<SpriteCommon> spCom;
	std::unique_ptr<DebugText> debugText;
	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ�
	UINT debugTextTexNumber;

	Input* input = nullptr;

public:
	void init() override;
	void update() override;
	void draw() override;
};