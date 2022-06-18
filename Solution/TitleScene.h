#pragma once
#include "GameScene.h"

#include "DebugText.h"

#include "Input.h"

#include <memory>

#include <DirectXMath.h>

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

	DirectX::XMFLOAT2 titleStrPos{};

	// update_���Ƃ��֐����i�[����
	void (TitleScene:: *update_proc)();

	void update_end();
	void update_normal();

public:
	TitleScene();
	void update() override;
	void drawFrontSprite() override;
};