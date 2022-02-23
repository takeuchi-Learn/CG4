#pragma once
#include "GameScene.h"

#include "DebugText.h"

#include "Input.h"

class EndScene :
    public GameScene {

	// --------------------
	// �f�o�b�O�e�L�X�g
	// --------------------
	Sprite::SpriteCommon spCom;
	DebugText debugText{};
	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ����w��
	const UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;

	Input* input = nullptr;

public:
    void init() override;
    void update() override;
    void draw() override;
    void fin() override;
};

