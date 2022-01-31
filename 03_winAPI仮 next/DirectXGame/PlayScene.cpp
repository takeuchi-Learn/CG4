#include "PlayScene.h"

#include "SceneManager.h"

#include "Input.h"

#include <DirectXMath.h>

#include <memory>

#include "Sound.h"

#include "Sprite.h"

#include "DebugText.h"

#include "Object3d.h"

#include "Collision.h"

#include "Time.h"

#include <sstream>
#include <iomanip>

#include<xaudio2.h>

using namespace DirectX;

namespace {
	bool firstFlag = true;

#pragma region main���炻�̂܂܈ڐA
	std::unique_ptr<Sound::SoundCommon> soundCommon;

	std::unique_ptr<Sound> soundData1;

	// --------------------
	// �r���[�ϊ��s��
	// --------------------
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);   // ���_���W
	XMFLOAT3 target(0, 0, 0);   // �����_���W
	XMFLOAT3 up(0, 1, 0);       // ������x�N�g��

	// --------------------
	// �X�v���C�g����
	// --------------------
	Sprite::SpriteCommon spriteCommon;
	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	enum TEX_NUM { TEX1, HOUSE };

	// --------------------
	// �X�v���C�g��
	// --------------------
	constexpr int SPRITES_NUM = 1;
	Sprite sprites[SPRITES_NUM];

	// --------------------
	// �f�o�b�O�e�L�X�g
	// --------------------
	DebugText debugText;
	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ����w��
	constexpr UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;

	// 3D�I�u�W�F�N�g�p�p�C�v���C������
	Object3d::PipelineSet object3dPipelineSet;

	// --------------------
	// �����蔻��
	// --------------------
	// ��
	Sphere sphere;
	//// ����
	Plane plane;
	// �O�p�`
	Triangle triangle;
	// ���C
	Ray ray;

	// --------------------
	// ���[�v�O�錾��`
	// --------------------
	int animFrameCount = 0; // �A�j���[�V�����̌o�ߎ��ԃJ�E���^�[
	constexpr UINT obj3dTexNum = 0U;
	std::unique_ptr<Model> model;
	std::unique_ptr<Object3d> obj3d;
	constexpr float obj3dScale = 10.f;


	// --------------------
	// ����
	// --------------------
	std::unique_ptr<Time> timer(new Time());


#pragma endregion
}

void PlayScene::init() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : Play");

#pragma region main���炻�̂܂܈ڐA
	if (firstFlag) {
		soundCommon.reset(new Sound::SoundCommon());
		soundData1.reset(
			new Sound("Resources/BGM.wav", soundCommon.get())
		);

		// �r���[�ϊ��s��
		matView = XMMatrixLookAtLH(
			XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up)
		);
	}

	// --------------------
	// �X�v���C�g����
	// --------------------
	spriteCommon = Sprite::SpriteCommonCreate(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height);

	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	Sprite::SpriteCommonLoadTexture(spriteCommon, TEX_NUM::TEX1, L"Resources/texture.png", DirectXCommon::getInstance()->getDev());
	Sprite::SpriteCommonLoadTexture(spriteCommon, TEX_NUM::HOUSE, L"Resources/house.png", DirectXCommon::getInstance()->getDev());

	// �X�v���C�g�̐���
	for (int i = 0; i < _countof(sprites); i++) {
		sprites[i].SpriteCreate(
			DirectXCommon::getInstance()->getDev(),
			WinAPI::window_width, WinAPI::window_height,
			TEX_NUM::HOUSE, spriteCommon, { 0,0 }, false, false
		);
		// �X�v���C�g�̍��W�ύX
		sprites[i].position.x = 1280 / 4;
		sprites[i].position.y = 720 / 4;
		//sprites[i].isInvisible = true;
		//sprites[i].position.x = (float)(rand() % 1280);
		//sprites[i].position.y = (float)(rand() % 720);
		//sprites[i].rotation = (float)(rand() % 360);
		//sprites[i].rotation = 0;
		//sprites[i].size.x = 400.0f;
		//sprites[i].size.y = 100.0f;
		// ���_�o�b�t�@�ɔ��f
		sprites[i].SpriteTransferVertexBuffer(spriteCommon);
	}

	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ǂݍ���
	Sprite::SpriteCommonLoadTexture(spriteCommon, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());
	// �f�o�b�O�e�L�X�g������
	debugText.Initialize(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height, debugTextTexNumber, spriteCommon);

	// 3D�I�u�W�F�N�g�p�p�C�v���C������
	object3dPipelineSet = Object3d::Object3dCreateGraphicsPipeline(DirectXCommon::getInstance()->getDev());

	// --------------------
	// ��2���ʏ�����
	// --------------------
	// ���̏����l��ݒ�
	sphere.center = XMVectorSet(0, 2, 0, 1);	// ���S���W
	sphere.radius = 2.f;	// ���ac
	// ���ʂ̏����l��ݒ�
	plane.normal = XMVectorSet(0, 1, 0, 0);	//�@���x�N�g��
	plane.distance = 0.f;	// ���_����̋���
	// �O�p�`�̏����l��ݒ�
	triangle.p0 = XMVectorSet(-1.f, 0, -1.f, 1);	//����O
	triangle.p1 = XMVectorSet(-1.f, 0, +1.f, 1);	//����
	triangle.p2 = XMVectorSet(+1.f, 0, -1.f, 1);	//�E��O
	triangle.normal = XMVectorSet(0, 1, -0, 0);		//�@��
	// ���C�̏����l��ݒ�
	ray.start = XMVectorSet(0, 1, 0, 1);	//���_����
	ray.dir = XMVectorSet(0, -1, 0, 0);		//������


	// --------------------
	// ���[�v�O�錾��`
	// --------------------
	model.reset(new Model(DirectXCommon::getInstance()->getDev(),
						  L"Resources/model/model.obj", L"Resources/model/tex.png",
						  WinAPI::window_width, WinAPI::window_height, Object3d::constantBufferNum, obj3dTexNum)
	);
	obj3d.reset(new Object3d(DirectXCommon::getInstance()->getDev(), model.get(), obj3dTexNum));
	obj3d->scale = { obj3dScale, obj3dScale, obj3dScale };
	obj3d->position = { 0, 0, obj3dScale };


	// --------------------
	// �����Đ�
	// --------------------
	if (Sound::checkPlaySound(soundData1.get()) == false) {
		Sound::SoundPlayWave(soundCommon.get(), soundData1.get(), XAUDIO2_LOOP_INFINITE);
		OutputDebugStringA("PLAAAAAAAAAAAAAAAAY!\n");
	} else {
		Sound::SoundStopWave(soundData1.get());
		OutputDebugStringA("STOOOOOOOOOOOOOOP!\n");
	}
#pragma endregion

	if (firstFlag) firstFlag = false;
}

void PlayScene::update() {
	if (Input::getInstance()->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::END);
	}



#pragma region main���炻�̂܂܈ڐA

	// ������0�L�[�������ꂽ�u�ԉ����Đ����Ȃ���
	if (Input::getInstance()->triggerKey(DIK_0)) {
		//Sound::SoundStopWave(soundData1);

		if (Sound::checkPlaySound(soundData1.get())) {
			Sound::SoundStopWave(soundData1.get());
			OutputDebugStringA("STOP\n");
		} else {
			Sound::SoundPlayWave(soundCommon.get(), soundData1.get(), XAUDIO2_LOOP_INFINITE);
			OutputDebugStringA("PLAY\n");
		}
	}

	{
		std::string stateStr = "STOP []";
		if (Sound::checkPlaySound(soundData1.get())) {
			stateStr = "PLAY |>";
		}
		debugText.Print(spriteCommon, "SOUND_PLAY_STATE : " + stateStr, 0, debugText.fontHeight * 2);

		stateStr = "Press 0 to Play/Stop Sound";
		debugText.Print(spriteCommon, stateStr, 0, debugText.fontHeight * 3);
	}

	// --------------------
	// �}�E�X
	// --------------------
	if (Input::getInstance()->hitMouseBotton(Input::MOUSE::LEFT)) {
		debugText.Print(spriteCommon, "input mouse left",
		Input::getInstance()->getMousePos().x, Input::getInstance()->getMousePos().y, 0.75f);
	}
	if (Input::getInstance()->hitMouseBotton(Input::MOUSE::RIGHT)) {
		debugText.Print(spriteCommon, "input mouse right",
		Input::getInstance()->getMousePos().x,
		Input::getInstance()->getMousePos().y + debugText.fontHeight, 0.75f);
	}
	if (Input::getInstance()->hitMouseBotton(Input::MOUSE::WHEEL)) {
		debugText.Print(spriteCommon, "input mouse wheel",
		Input::getInstance()->getMousePos().x,
		Input::getInstance()->getMousePos().y + debugText.fontHeight * 2, 0.75f);
	}
	if (Input::getInstance()->hitMouseBotton(VK_LSHIFT)) {
		debugText.Print(spriteCommon, "LSHIFT", 0, 0, 2);
	}

	// R���������у}�E�X�J�[�\���̕\����\����؂�ւ�
	if (Input::getInstance()->triggerKey(DIK_R)) {
		static bool mouseDispFlag = true;
		mouseDispFlag = !mouseDispFlag;
		Input::getInstance()->changeDispMouseCursorFlag(mouseDispFlag);
	}

	// M�L�[�Ń}�E�X�J�[�\���ʒu��0,0�Ɉړ�
	if (Input::getInstance()->triggerKey(DIK_M)) {
		Input::getInstance()->setMousePos(0, 0);
	}
	// --------------------
	// �}�E�X�����܂�
	// --------------------

	// --------------------
	// ��2���ʍX�V
	// --------------------

	// ���C�ړ�
	{
		XMVECTOR moveZ = XMVectorSet(0, 0, 0.01f, 0);
		if (Input::getInstance()->hitKey(DIK_DOWN)) ray.start += moveZ;
		else if (Input::getInstance()->hitKey(DIK_UP)) ray.start -= moveZ;

		XMVECTOR moveX = XMVectorSet(0.01f, 0, 0, 0);
		if (Input::getInstance()->hitKey(DIK_RIGHT)) ray.start += moveX;
		else if (Input::getInstance()->hitKey(DIK_LEFT)) ray.start -= moveX;
	}
	//stringstream�ŕϐ��̒l��ǂݍ��ݐ��`
	std::ostringstream raystr;
	raystr << "lay.start("
		<< std::fixed << std::setprecision(2)	// �����_�ȉ�2���܂�
		<< ray.start.m128_f32[0] << ","		// x
		<< ray.start.m128_f32[1] << ","		// y
		<< ray.start.m128_f32[2] << ")";	// z

	debugText.Print(spriteCommon, raystr.str(), debugText.fontWidth * 2, debugText.fontHeight * 5, 1.f);

	raystr.str("");
	raystr.clear();
	raystr << "FPS : " << DirectXCommon::getInstance()->getFPS();
	debugText.Print(spriteCommon, raystr.str(), 0, 0);

	// ���C�Ƌ��̓����蔻��
	XMVECTOR inter;
	float distance;
	bool hit = Collision::CheckRay2Sphere(ray, sphere, &distance, &inter);
	if (hit) {
		debugText.Print(spriteCommon, "HIT", debugText.fontWidth * 2, debugText.fontHeight * 7, 1.f);
		// stringstream�����Z�b�g�A��_���W�𖄂ߍ���
		raystr.str("");
		raystr.clear();
		raystr << "inter:("
			<< std::fixed << std::setprecision(2)
			<< inter.m128_f32[0] << ","
			<< inter.m128_f32[1] << ","
			<< inter.m128_f32[2] << ")";

		debugText.Print(spriteCommon, raystr.str(), debugText.fontWidth * 2, debugText.fontHeight * 8, 1.f);

		raystr.str("");
		raystr.clear();
		raystr << "distance:("
			<< std::fixed << std::setprecision(2)
			<< distance << ")";

		debugText.Print(spriteCommon, raystr.str(), debugText.fontWidth * 2, debugText.fontHeight * 9, 1.f);
	}

	{
		if (Input::getInstance()->hitKey(DIK_R)) timer->reset();

		std::ostringstream tmpStr{};
		tmpStr << std::fixed << std::setprecision(6) <<
			(long double)timer->getNowTime() / Time::oneSec << "[s]";
		debugText.Print(spriteCommon, tmpStr.str(), 0, debugText.fontHeight * 11);
	}

	// --------------------
	// ��2���ʍX�V�����܂�
	// --------------------

	if (Input::getInstance()->hitKey(DIK_A) || Input::getInstance()->hitKey(DIK_D)) {
		float angle = 0.f;
		if (Input::getInstance()->hitKey(DIK_D)) { angle += XMConvertToRadians(1.0f); } else if (Input::getInstance()->hitKey(DIK_A)) { angle -= XMConvertToRadians(1.0f); }

		// angle���W�A������Y���܂��ɉ�]�B���a��-100
		eye.x = -100 * sinf(angle);
		eye.z = -100 * cosf(angle);
		matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	}

	if (Input::getInstance()->hitKey(DIK_I)) sprites[0].position.y -= 10; else if (Input::getInstance()->hitKey(DIK_K)) sprites[0].position.y += 10;
	if (Input::getInstance()->hitKey(DIK_J)) sprites[0].position.x -= 10; else if (Input::getInstance()->hitKey(DIK_L)) sprites[0].position.x += 10;





	//// X���W,Y���W���w�肵�ĕ\��
	//debugText.Print(spriteCommon, "Hello,DirectX!!", 200, 100);
	//// X���W,Y���W,�k�ڂ��w�肵�ĕ\��
	//debugText.Print(spriteCommon, "Nihon Kogakuin", 200, 200, 2.0f);

	//sprite.rotation = 45;
	//sprite.position = {1280/2, 720/2, 0};
	//sprite.color = {0, 0, 1, 1};
#pragma endregion
}

void PlayScene::draw() {

#pragma region main���炻�̂܂܈ڐA
	// �S�D�`��R�}���h��������
	Object3d::Object3dCommonBeginDraw(DirectXCommon::getInstance()->getCmdList(), object3dPipelineSet);
	obj3d->drawWithUpdate(matView, DirectXCommon::getInstance());
	// �X�v���C�g���ʃR�}���h
	Sprite::SpriteCommonBeginDraw(spriteCommon, DirectXCommon::getInstance()->getCmdList());
	// �X�v���C�g�`��
	for (UINT i = 0; i < _countof(sprites); i++) {
		sprites[i].SpriteDrawWithUpdate(DirectXCommon::getInstance(), spriteCommon);
	}
	// �f�o�b�O�e�L�X�g�`��
	debugText.DrawAll(DirectXCommon::getInstance(), spriteCommon);
	// �S�D�`��R�}���h�����܂�
#pragma endregion
}

void PlayScene::fin() {
	Sound::SoundStopWave(soundData1.get());
}
