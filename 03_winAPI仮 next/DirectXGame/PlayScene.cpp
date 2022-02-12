#include "PlayScene.h"

#include "SceneManager.h"

#include "Input.h"

#include <sstream>
#include <iomanip>

#include <xaudio2.h>
#include <random>

using namespace DirectX;

namespace {
	class MyRand {
	public:
		// ��l����
		static int getRand(const int min, const int max) {
			static std::random_device rnd{};
			static std::mt19937_64 mt(rnd());

			int minLocal = min, maxLocal = max;
			if (max < min) {
				minLocal = max;
				maxLocal = min;
			}
			std::uniform_int_distribution<> myRand(minLocal, maxLocal);	// �͈͎w��̗���
			return myRand(mt);
		}
		static double getRand(const double min, const double max) {
			static std::random_device rnd{};
			static std::mt19937_64 mt(rnd());

			double minLocal = min, maxLocal = max;
			if (max < min) {
				minLocal = max;
				maxLocal = min;
			}
			std::uniform_real_distribution<> myRand(minLocal, maxLocal);	// �͈͎w��̗���
			return myRand(mt);
		}

		// ���K���z����
		static int getRandNormally(const double center, const double range) {
			static std::random_device rnd{};
			static std::mt19937_64 mt(rnd());

			std::normal_distribution<> myRand(center, range);	// �͈͎w��̗���
			return myRand(mt);
		}
	};
}

void PlayScene::init() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : Play");

	dxCom = DirectXCommon::getInstance();

#pragma region �r���[�ϊ�

	camera.reset(new Camera(WinAPI::window_width, WinAPI::window_height));
	camera->setEye(XMFLOAT3(0, 0, -175));	// ���_���W
	camera->setTarget(XMFLOAT3(0, 0, 0));	// �����_���W
	camera->setUp(XMFLOAT3(0, 1, 0));		// �����

#pragma endregion �r���[�ϊ�

#pragma region ��

	soundCommon.reset(new Sound::SoundCommon());
	soundData1.reset(new Sound("Resources/BGM.wav", soundCommon.get()));

#pragma endregion ��

#pragma region �X�v���C�g

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
			TEX_NUM::TEX1, spriteCommon, { 0,0 }, false, false
		);
		// �X�v���C�g�̍��W�ύX
		sprites[i].position.x = 1280 / 10;
		sprites[i].position.y = 720 / 10;
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


#pragma endregion �X�v���C�g

#pragma region 3D�I�u�W�F�N�g

	model.reset(new Model(DirectXCommon::getInstance()->getDev(),
						  L"Resources/model/model.obj", L"Resources/model/tex.png",
						  WinAPI::window_width, WinAPI::window_height,
						  Object3d::constantBufferNum, obj3dTexNum));

	obj3d.reset(new Object3d(DirectXCommon::getInstance()->getDev(), model.get(), obj3dTexNum));
	obj3d->scale = { obj3dScale, obj3dScale, obj3dScale };
	obj3d->position = { 0, 0, obj3dScale };

	sphere.reset(new Sphere(DirectXCommon::getInstance()->getDev(), 2.f, L"Resources/red.png", 0));

#pragma endregion 3D�I�u�W�F�N�g

	particleMgr.reset(new ParticleManager());
	particleMgr->init(dxCom->getDev(), L"Resources/effect1.png");
	particleMgr->setCamera(camera.get());

	timer.reset(new Time());
}

void PlayScene::update() {

	if (Input::getInstance()->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::END);
	}

#pragma region ��

	// ������0�L�[�������ꂽ�u�ԉ����Đ����Ȃ���
	if (Input::getInstance()->triggerKey(DIK_0)) {
		//Sound::SoundStopWave(soundData1);

		if (Sound::checkPlaySound(soundData1.get())) {
			Sound::SoundStopWave(soundData1.get());
		} else {
			Sound::SoundPlayWave(soundCommon.get(), soundData1.get(), XAUDIO2_LOOP_INFINITE);
		}
	}

	{
		static std::string stateStr = "STOP []";
		if (Sound::checkPlaySound(soundData1.get())) {
			stateStr = "PLAY |>";
		} else stateStr = "STOP []";
		debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 2, 1.f, "SOUND_PLAY_STATE : %s", stateStr.c_str());

		debugText.Print(spriteCommon, "Press 0 to Play/Stop Sound", 0, debugText.fontHeight * 3);
	}

#pragma endregion ��

#pragma region �}�E�X

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

#pragma endregion �}�E�X

#pragma region ����


	debugText.formatPrint(spriteCommon, 0, 0, 1.f, "FPS : %f", dxCom->getFPS());

	if (Input::getInstance()->hitKey(DIK_R)) timer->reset();

	debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 5, 1.f,
						  "Time : %.6f[s]", (long double)timer->getNowTime() / Time::oneSec);

#pragma endregion ����

#pragma region �J�����ړ���]

	{

		const float rotaVal = XM_PIDIV2 / DirectXCommon::getInstance()->getFPS();	// ���b�l����

		if (Input::getInstance()->hitKey(DIK_RIGHT)) {
			angle.y += rotaVal;
			if (angle.y > XM_PI * 2) { angle.y = 0; }
		} else if (Input::getInstance()->hitKey(DIK_LEFT)) {
			angle.y -= rotaVal;
			if (angle.y < 0) { angle.y = XM_PI * 2; }
		}

		if (Input::getInstance()->hitKey(DIK_UP)) {
			if (angle.x + rotaVal < XM_PIDIV2) angle.x += rotaVal;
		} else if (Input::getInstance()->hitKey(DIK_DOWN)) {
			if (angle.x - rotaVal > -XM_PIDIV2) angle.x -= rotaVal;
		}

		// angle���W�A������Y���܂��ɉ�]�B���a��-100
		constexpr float camRange = 100.f;	// targetLength
		camera->rotation(camRange, angle.x, angle.y);


		// �ړ���
		const float moveSpeed = 75.f / dxCom->getFPS();
		// ���_�ړ�
		if (Input::getInstance()->hitKey(DIK_W)) {
			camera->moveForward(moveSpeed);
		} else if (Input::getInstance()->hitKey(DIK_S)) {
			camera->moveForward(-moveSpeed);
		}
		if (Input::getInstance()->hitKey(DIK_A)) {
			camera->moveRight(-moveSpeed);
		} else if (Input::getInstance()->hitKey(DIK_D)) {
			camera->moveRight(moveSpeed);
		}
	}

#pragma endregion �J�����ړ���]

	if (Input::getInstance()->hitKey(DIK_I)) sprites[0].position.y -= 10; else if (Input::getInstance()->hitKey(DIK_K)) sprites[0].position.y += 10;
	if (Input::getInstance()->hitKey(DIK_J)) sprites[0].position.x -= 10; else if (Input::getInstance()->hitKey(DIK_L)) sprites[0].position.x += 10;

	// P���������уp�[�e�B�N��20���ǉ�
	constexpr UINT particleNum = 20U;
	if (Input::getInstance()->triggerKey(DIK_P)) createParticle(obj3d->position, particleNum);

	camera->update();
}

void PlayScene::draw() {
	// �S�D�`��R�}���h��������
	// ���̃R�}���h
	Sphere::sphereCommonBeginDraw(object3dPipelineSet);
	sphere->drawWithUpdate(camera->getViewMatrix(), dxCom);
	// 3D�I�u�W�F�N�g�R�}���h
	Object3d::Object3dCommonBeginDraw(dxCom->getCmdList(), object3dPipelineSet);
	obj3d->drawWithUpdate(camera->getViewMatrix(), dxCom);

	particleMgr->drawWithUpdate(dxCom->getCmdList());

	// �X�v���C�g���ʃR�}���h
	Sprite::SpriteCommonBeginDraw(spriteCommon, dxCom->getCmdList());
	// �X�v���C�g�`��
	for (UINT i = 0; i < _countof(sprites); i++) {
		sprites[i].SpriteDrawWithUpdate(dxCom, spriteCommon);
	}
	// �f�o�b�O�e�L�X�g�`��
	debugText.DrawAll(dxCom, spriteCommon);
	// �S�D�`��R�}���h�����܂�
}

void PlayScene::fin() {
	//Sound::SoundStopWave(soundData1.get());
}

void PlayScene::createParticle(const DirectX::XMFLOAT3 pos, const UINT particleNum) {
	for (UINT i = 0U; i < particleNum; i++) {
		// X,Y,Z�S��[-2.5f,+2.5f]�Ń����_���ɕ��z
		constexpr float rnd_pos = 2.5f;
		XMFLOAT3 generatePos = pos;
		/*generatePos.x += MyRand::getRandNormally(0.f, rnd_pos);
		generatePos.y += MyRand::getRandNormally(0.f, rnd_pos);
		generatePos.z += MyRand::getRandNormally(0.f, rnd_pos);*/

		constexpr float rnd_vel = 0.0625f;
		XMFLOAT3 vel{};
		vel.x = (float)MyRand::getRand(-rnd_vel, rnd_vel);
		vel.y = (float)MyRand::getRand(-rnd_vel, rnd_vel);
		vel.z = (float)MyRand::getRand(-rnd_vel, rnd_vel);

		XMFLOAT3 acc{};
		constexpr float rnd_acc = 0.001f;
		acc.y = -MyRand::getRand(rnd_acc, rnd_acc * 2.f);

		// �ǉ�
		particleMgr->add(timer.get(),
						 Time::oneSec / 4, generatePos, vel, acc,
						 0.4f, 0.0f,
						 0.f, 0.f,
						 XMFLOAT3(1, 1, 0.25f), XMFLOAT3(1, 0, 1));
	}
}