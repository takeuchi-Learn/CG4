#include "PlayScene.h"

#include "SceneManager.h"

#include <sstream>
#include <iomanip>

#include <xaudio2.h>
#include "RandomNum.h"

using namespace DirectX;

namespace {
	// @return 0 <= ret[rad] < 2PI
	float angleRoundRad(float rad) {
		float angle = rad;

		if (angle >= 0.f && angle < XM_2PI) return angle;

		while (angle >= XM_2PI) {
			angle -= XM_2PI;
		}
		while (angle < 0) {
			angle += XM_2PI;
		}
		return angle;
	}

	float nearSin(float rad) {
		constexpr auto a = +0.005859483;
		constexpr auto b = +0.005587939;
		constexpr auto c = -0.171570726;
		constexpr auto d = +0.0018185485;
		constexpr auto e = +0.9997773594;

		double x = angleRoundRad(rad);

		// 0 ~ PI/2���킩��΋��߂���
		if (x < XM_PIDIV2) {
			// ���̂܂�
		} else if (x >= XM_PIDIV2 && x < XM_PI) {
			x = XM_PI - x;
		} else if (x < XM_PI * 1.5f) {
			x = -(x - XM_PI);
		} else if (x < XM_2PI) {
			x = -(XM_2PI - x);
		}

		return x * (x * (x * (x * (a * x + b) + c) + d) + e);
	}

	float nearCos(float rad) {
		return nearSin(rad + XM_PIDIV2);
	}

	float nearTan(float rad) {
		return nearSin(rad) / nearCos(rad);
	}

	float mySin(float rad) {
		float ret = angleRoundRad(rad);

		if (rad < XM_PIDIV2) {
			ret = nearSin(rad);
		} else if (rad < XM_PI) {
			ret = nearSin(XM_PI - rad);
		} else if (rad < XM_PI * 1.5f) {
			ret = -nearSin(rad - XM_PI);
		} else if (rad < XM_2PI) {
			ret = -nearSin(XM_2PI - rad);
		} else {
			ret = nearSin(rad);
		}

		return ret;
	}

	float myCos(float rad) {
		return mySin(rad + XM_PIDIV2);
	}
}

void PlayScene::init() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : Play (SE : OtoLogic)");

	dxCom = DirectXCommon::getInstance();

	input = Input::getInstance();

#pragma region �r���[�ϊ�

	camera.reset(new Camera(WinAPI::window_width, WinAPI::window_height));
	camera->setEye(XMFLOAT3(0, 0, -175));	// ���_���W
	camera->setTarget(XMFLOAT3(0, 0, 0));	// �����_���W
	camera->setUp(XMFLOAT3(0, 1, 0));		// �����

#pragma endregion �r���[�ϊ�

#pragma region ��

	soundCommon.reset(new Sound::SoundCommon());
	soundData1.reset(new Sound("Resources/BGM.wav", soundCommon.get()));

	particleSE.reset(new Sound("Resources/SE/Sys_Set03-click.wav", soundCommon.get()));

#pragma endregion ��

#pragma region �X�v���C�g

	// --------------------
	// �X�v���C�g����
	// --------------------
	spriteCommon = Sprite::createSpriteCommon(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height);

	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	Sprite::commonLoadTexture(spriteCommon, TEX_NUM::TEX1, L"Resources/texture.png", DirectXCommon::getInstance()->getDev());

	// �X�v���C�g�̐���
	for (int i = 0; i < _countof(sprites); i++) {
		sprites[i].create(
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
	Sprite::commonLoadTexture(spriteCommon, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());
	// �f�o�b�O�e�L�X�g������
	debugText.Initialize(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height, debugTextTexNumber, spriteCommon);

	// 3D�I�u�W�F�N�g�p�p�C�v���C������
	object3dPipelineSet = Object3d::createGraphicsPipeline(DirectXCommon::getInstance()->getDev());


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

#pragma region ���C�g

	light = obj3d->position;
	light.x += obj3d->scale.y * 2;	// ��

#pragma endregion ���C�g

	// �p�[�e�B�N��������
	particleMgr.reset(new ParticleManager(dxCom->getDev(), L"Resources/effect1.png", camera.get()));

	// ���ԏ�����
	timer.reset(new Time());
}

void PlayScene::update() {

	// SPACE��END�V�[����
	if (input->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::END);
	}

#pragma region ��

	// ������0�L�[�������ꂽ�u�ԉ����Đ����Ȃ���
	if (input->triggerKey(DIK_0)) {
		//Sound::SoundStopWave(soundData1);

		if (Sound::checkPlaySound(soundData1.get())) {
			Sound::SoundStopWave(soundData1.get());
		} else {
			Sound::SoundPlayWave(soundCommon.get(), soundData1.get(), XAUDIO2_LOOP_INFINITE);
		}
	}

	{
		std::string stateStr = "STOP []";
		if (Sound::checkPlaySound(soundData1.get())) {
			stateStr = "PLAY |>";
		}
		debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 2, 1.f, XMFLOAT4(1, 1, 1, 1), "BGM_STATE : %s", stateStr.c_str());

		debugText.Print(spriteCommon, "0 : Play/Stop BGM", 0, debugText.fontHeight * 3);

		debugText.Print(spriteCommon, "P : create particle(play SE)", 0, debugText.fontHeight * 4);
	}

#pragma endregion ��

#pragma region �}�E�X

	if (input->hitMouseBotton(Input::MOUSE::LEFT)) {
		debugText.Print(spriteCommon, "input mouse left",
		input->getMousePos().x, input->getMousePos().y, 0.75f);
	}
	if (input->hitMouseBotton(Input::MOUSE::RIGHT)) {
		debugText.Print(spriteCommon, "input mouse right",
		input->getMousePos().x,
		input->getMousePos().y + debugText.fontHeight, 0.75f);
	}
	if (input->hitMouseBotton(Input::MOUSE::WHEEL)) {
		debugText.Print(spriteCommon, "input mouse wheel",
		input->getMousePos().x,
		input->getMousePos().y + debugText.fontHeight * 2, 0.75f);
	}
	if (input->hitMouseBotton(VK_LSHIFT)) {
		debugText.Print(spriteCommon, "LSHIFT(WinAPI)", 0, 0, 2);
	}

	// R���������у}�E�X�J�[�\���̕\����\����؂�ւ�
	if (input->triggerKey(DIK_R)) {
		static bool mouseDispFlag = true;
		mouseDispFlag = !mouseDispFlag;
		input->changeDispMouseCursorFlag(mouseDispFlag);
	}

	// M�L�[�Ń}�E�X�J�[�\���ʒu��0,0�Ɉړ�
	if (input->triggerKey(DIK_M)) {
		input->setMousePos(0, 0);
	}

#pragma endregion �}�E�X

#pragma region ����

	debugText.formatPrint(spriteCommon, 0, 0, 1.f,
						  XMFLOAT4(1, 1, 1, 1), "FPS : %f", dxCom->getFPS());

	if (input->hitKey(DIK_R)) timer->reset();

	debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 15, 1.f,
						  XMFLOAT4(1, 1, 1, 1),
						  "Time : %.6f[s]", (long double)timer->getNowTime() / Time::oneSec);

#pragma endregion ����

#pragma region ���\��

	if (input->triggerKey(DIK_T)) {
		debugText.tabSize++;
		if (input->hitKey(DIK_LSHIFT)) debugText.tabSize = 4U;
	}

	debugText.formatPrint(spriteCommon, debugText.fontWidth * 2, debugText.fontHeight * 17, 1.f,
						  XMFLOAT4(1, 1, 1, 1),
						  "newLine\ntab(size %u)\tendString", debugText.tabSize);

	debugText.Print(spriteCommon, "SPACE : end", 0, debugText.fontHeight * 6, 1.f, XMFLOAT4(1, 0.5f, 0.5f, 1));

	debugText.Print(spriteCommon, "WASD : move camera", 0, debugText.fontHeight * 8);
	debugText.Print(spriteCommon, "arrow : rotation camera", 0, debugText.fontHeight * 9);

#pragma endregion ���\��

#pragma region �J�����ړ���]

	{

		const float rotaVal = XM_PIDIV2 / DirectXCommon::getInstance()->getFPS();	// ���b�l����

		if (input->hitKey(DIK_RIGHT)) {
			angle.y += rotaVal;
			if (angle.y > XM_PI * 2) { angle.y = 0; }
		} else if (input->hitKey(DIK_LEFT)) {
			angle.y -= rotaVal;
			if (angle.y < 0) { angle.y = XM_PI * 2; }
		}

		if (input->hitKey(DIK_UP)) {
			if (angle.x + rotaVal < XM_PIDIV2) angle.x += rotaVal;
		} else if (input->hitKey(DIK_DOWN)) {
			if (angle.x - rotaVal > -XM_PIDIV2) angle.x -= rotaVal;
		}

		// angle���W�A������Y���܂��ɉ�]�B���a��-100
		constexpr float camRange = 100.f;	// targetLength
		camera->rotation(camRange, angle.x, angle.y);


		// �ړ���
		const float moveSpeed = 75.f / dxCom->getFPS();
		// ���_�ړ�
		if (input->hitKey(DIK_W)) {
			camera->moveForward(moveSpeed);
		} else if (input->hitKey(DIK_S)) {
			camera->moveForward(-moveSpeed);
		}
		if (input->hitKey(DIK_A)) {
			camera->moveRight(-moveSpeed);
		} else if (input->hitKey(DIK_D)) {
			camera->moveRight(moveSpeed);
		}
	}

#pragma endregion �J�����ړ���]

#pragma region ���C�g
	{
		// ��b�ň��(2PI[rad])
		auto timeAngle = angleRoundRad((float)timer->getNowTime() / Time::oneSec * XM_2PI);

		debugText.formatPrint(spriteCommon,
							  debugText.fontWidth * 16, debugText.fontHeight * 16, 1.f,
							  XMFLOAT4(1, 1, 1, 1),
							  "light angle : %f PI [rad]",
							  timeAngle / XM_PI);

		constexpr float lightR = 20.f;
		light = obj3d->position;
		light.x += mySin(timeAngle) * lightR;
		light.z += myCos(timeAngle) * lightR;

		obj3d->setLightPos(light);

		sphere->pos = light;
	}
#pragma endregion ���C�g

#pragma region �X�v���C�g

	if (input->hitKey(DIK_I)) sprites[0].position.y -= 10; else if (input->hitKey(DIK_K)) sprites[0].position.y += 10;
	if (input->hitKey(DIK_J)) sprites[0].position.x -= 10; else if (input->hitKey(DIK_L)) sprites[0].position.x += 10;

	// P���������уp�[�e�B�N��50���ǉ�
	if (input->triggerKey(DIK_P)) {
		constexpr UINT particleNumMax = 50U, particleNumMin = 20U;
		UINT particleNum = particleNumMin;

		float startScale = 5.f;

		if (input->hitKey(DIK_U)) {
			particleNum = particleNumMax;
			startScale = 10.f;
		}
		createParticle(obj3d->position, particleNum, startScale);

		Sound::SoundPlayWave(soundCommon.get(), particleSE.get());
	}

#pragma endregion �X�v���C�g

	camera->update();
}

void PlayScene::draw() {
	// �S�D�`��R�}���h��������
	// ���̃R�}���h
	Sphere::sphereCommonBeginDraw(object3dPipelineSet);
	sphere->drawWithUpdate(camera->getViewMatrix(), dxCom);
	// 3D�I�u�W�F�N�g�R�}���h
	Object3d::startDraw(dxCom->getCmdList(), object3dPipelineSet);
	obj3d->drawWithUpdate(camera->getViewMatrix(), dxCom);

	ParticleManager::startDraw(dxCom->getCmdList(), object3dPipelineSet);
	particleMgr->drawWithUpdate(dxCom->getCmdList());


	// �X�v���C�g���ʃR�}���h
	Sprite::drawStart(spriteCommon, dxCom->getCmdList());
	// �X�v���C�g�`��
	for (UINT i = 0; i < _countof(sprites); i++) {
		sprites[i].drawWithUpdate(dxCom, spriteCommon);
	}
	// �f�o�b�O�e�L�X�g�`��
	debugText.DrawAll(dxCom, spriteCommon);
	// �S�D�`��R�}���h�����܂�
}

void PlayScene::fin() {
	//Sound::SoundStopWave(soundData1.get());
}

void PlayScene::createParticle(const DirectX::XMFLOAT3 pos, const UINT particleNum, const float startScale) {
	for (UINT i = 0U; i < particleNum; i++) {

		const float theata = RandomNum::getRandf(0, XM_PI);
		const float phi = RandomNum::getRandf(0, XM_PI * 2.f);
		const float r = RandomNum::getRandf(0, 5.f);

		// X,Y,Z�S��[-2.5f,+2.5f]�Ń����_���ɕ��z
		constexpr float rnd_pos = 2.5f;
		XMFLOAT3 generatePos = pos;
		/*generatePos.x += Random::getRandNormallyf(0.f, rnd_pos);
		generatePos.y += Random::getRandNormallyf(0.f, rnd_pos);
		generatePos.z += Random::getRandNormallyf(0.f, rnd_pos);*/

		//constexpr float rnd_vel = 0.0625f;
		const XMFLOAT3 vel{
			r * sin(theata) * cos(phi),
			r * cos(theata),
			r * sin(theata) * sin(phi)
		};

		//constexpr float rnd_acc = 0.05f;
		XMFLOAT3 acc{};

		/*acc.x = 0.f;
		acc.y = -Random::getRandf(rnd_acc, rnd_acc * 2.f);
		acc.z = 0.f;*/


		constexpr auto startCol = XMFLOAT3(1, 1, 0.25f), endCol = XMFLOAT3(1, 0, 1);
		constexpr int life = Time::oneSec / 4;
		constexpr float endScale = 0.f;
		constexpr float startRota = 0.f, endRota = 0.f;

		// �ǉ�
		particleMgr->add(timer.get(),
						 life, generatePos, vel, acc,
						 startScale, endScale,
						 startRota, endRota,
						 startCol, endCol);
	}
}