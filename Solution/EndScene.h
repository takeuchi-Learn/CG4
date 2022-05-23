#pragma once
#include "GameScene.h"

#include "DebugText.h"

#include "Input.h"

#include <memory>

class EndScene :
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
	EndScene();
    void update() override;
    void drawFrontSprite() override;
};

