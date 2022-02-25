#pragma once
#include "GameScene.h"

#include <memory>
#include "Time.h"
#include "Sound.h"
#include "Sprite.h"
#include "Object3d.h"
#include "DebugText.h"

#include <DirectXMath.h>

#include "Sphere.h"

#include "Camera.h"

#include "ParticleManager.h"

#include "DirectXCommon.h"

#include "Input.h"

class PlayScene :
	public GameScene {

#pragma region �r���[�ϊ��s��

	//DirectX::XMMATRIX matView;
	DirectX::XMFLOAT3 eye_local;   // ���_���W
	DirectX::XMFLOAT3 target_local;   // �����_���W
	DirectX::XMFLOAT3 up_local;       // ������x�N�g��

#pragma endregion �r���[�ϊ��s��

#pragma region ��

	std::unique_ptr<Sound::SoundCommon> soundCommon;

	std::unique_ptr<Sound> soundData1;

	std::unique_ptr<Sound> particleSE;

#pragma endregion ��

#pragma region �X�v���C�g
	// --------------------
	// �X�v���C�g����
	// --------------------
	SpriteCommon spriteCommon;
	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	enum TEX_NUM { TEX1 };

	// --------------------
	// �X�v���C�g��
	// --------------------
	static const int SPRITES_NUM = 1;
	Sprite sprites[SPRITES_NUM]{};

	// --------------------
	// �f�o�b�O�e�L�X�g
	// --------------------
	DebugText debugText{};
	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ����w��
	const UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;
#pragma endregion �X�v���C�g

#pragma region 3D�I�u�W�F�N�g

	// 3D�I�u�W�F�N�g�p�p�C�v���C������
	Object3d::PipelineSet object3dPipelineSet;

	const UINT obj3dTexNum = 0U;
	std::unique_ptr<Model> model;
	std::unique_ptr<Object3d> obj3d;
	const float obj3dScale = 10.f;

	DirectX::XMFLOAT2 angle{};	// �e������̉�]�p



	std::unique_ptr<Sphere> sphere;

#pragma endregion 3D�I�u�W�F�N�g

	DirectX::XMFLOAT3 light{};

	Input* input = nullptr;

	std::unique_ptr<Time> timer;

	std::unique_ptr<Camera> camera;

	std::unique_ptr<ParticleManager> particleMgr;

	DirectXCommon* dxCom = nullptr;

private:
	void createParticle(const DirectX::XMFLOAT3 pos, const UINT particleNum = 10U, const float startScale = 1.f);

public:
	void init() override;
	void update() override;
	void draw() override;
	void fin() override;
};

