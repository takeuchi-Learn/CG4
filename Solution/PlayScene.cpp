#include "PlayScene.h"

#include "SceneManager.h"

#include <sstream>
#include <iomanip>

#include <xaudio2.h>
#include "RandomNum.h"

#include "FbxLoader.h"

#include "FbxObj3d.h"

#include "EndScene.h"

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

	double near_atan2(double _y, double _x) {

		const double x = abs(_x);
		const double y = abs(_y);

		const bool bigX = y < x;

		double slope{};
		if (bigX) slope = (double)y / x;
		else  slope = (double)x / y;

		constexpr auto a = -0.05026472;
		constexpr auto b = +0.26603324;
		constexpr auto c = -0.45255286;
		constexpr auto d = +0.02385002;
		constexpr auto e = +0.99836359;

		auto ret = slope * (slope * (slope * (slope * (a * slope + b) + c) + d) + e); //5���Ȑ��ߎ�

		constexpr auto plane = XM_PI;
		constexpr auto rightAngle = plane / 2;	// ���p

		if (bigX) {
			if (_x > 0) {
				if (_y < 0) ret = -ret;
			} else {
				if (_y > 0) ret = plane - ret;
				if (_y < 0) ret = ret - plane;
			}
		} else {
			if (_x > 0) {
				if (_y > 0) ret = rightAngle - ret;
				if (_y < 0) ret = ret - rightAngle;
			}
			if (_x < 0) {
				if (_y > 0) ret = ret + rightAngle;
				if (_y < 0) ret = -ret - rightAngle;
			}
		}

		return ret;
	}

	float near_atan2(float y, float x) {
		return (float)near_atan2((double)y, (double)x);
	}
}

PlayScene::PlayScene() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : Play (SE : OtoLogic)");
	dxCom = DirectXCommon::getInstance();

	input = Input::getInstance();

	FbxObj3d::setDevice(dxCom->getDev());

#pragma region �r���[�ϊ�

	camera.reset(new Camera(WinAPI::window_width, WinAPI::window_height));
	camera->setFarZ(10000.f);
	camera->setEye(XMFLOAT3(0, 0, -175));	// ���_���W
	camera->setTarget(XMFLOAT3(0, 0, 0));	// �����_���W
	camera->setUp(XMFLOAT3(0, 1, 0));		// �����
	camera->update();

#pragma endregion �r���[�ϊ�

	FbxObj3d::setCamera(camera.get());
	FbxObj3d::createGraphicsPipeline();

#pragma region ���C�g

	light.reset(new Light());

#pragma endregion ���C�g


#pragma region ��

	soundCommon.reset(new Sound::SoundCommon());
	soundData1.reset(new Sound("Resources/BGM.wav", soundCommon.get()));

	particleSE.reset(new Sound("Resources/SE/Sys_Set03-click.wav", soundCommon.get()));

#pragma endregion ��

#pragma region �X�v���C�g

	// --------------------
	// �X�v���C�g����
	// --------------------
	spriteCommon.reset(new SpriteCommon());

	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	texNum = spriteCommon->loadTexture(L"Resources/texture.png");

	// �X�v���C�g�̐���
	for (int i = 0; i < _countof(sprites); i++) {
		sprites[i] = Sprite(texNum, spriteCommon.get(), { 0, 0 });
		// �X�v���C�g�̍��W�ύX
		sprites[i].position.x = 1280.f / 10;
		sprites[i].position.y = 720.f / 10;
		//sprites[i].isInvisible = true;
		//sprites[i].position.x = (float)(rand() % 1280);
		//sprites[i].position.y = (float)(rand() % 720);
		//sprites[i].rotation = (float)(rand() % 360);
		//sprites[i].rotation = 0;
		//sprites[i].size.x = 400.0f;
		//sprites[i].size.y = 100.0f;
	}

	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ǂݍ���
	debugTextTexNumber = spriteCommon->loadTexture(L"Resources/debugfont.png");
	// �f�o�b�O�e�L�X�g������
	debugText.reset(new DebugText(debugTextTexNumber, spriteCommon.get()));


#pragma endregion �X�v���C�g

#pragma region 3D�I�u�W�F�N�g

	// 3D�I�u�W�F�N�g�p�p�C�v���C������
	object3dPipelineSet = Object3d::createGraphicsPipeline(dxCom->getDev());

	backPipelineSet = Object3d::createGraphicsPipeline(dxCom->getDev(), Object3d::BLEND_MODE::ALPHA,
													   L"Resources/Shaders/BackVS.hlsl",
													   L"Resources/Shaders/BackPS.hlsl");



	backModel.reset(new Model("Resources/back/", "back", 1u, true));

	backObj.reset(new Object3d(DirectXCommon::getInstance()->getDev(), camera.get(), backModel.get(), 1u));
	constexpr float backScale = 10.f;
	backObj->scale = { backScale, backScale, backScale };

	/*model.reset(new Model(DirectXCommon::getInstance()->getDev(),
						  L"Resources/model/model.obj", L"Resources/model/tex.png",
						  WinAPI::window_width, WinAPI::window_height,
						  Object3d::constantBufferNum, obj3dTexNum));*/
	model.reset(new Model("Resources/model/", "model", obj3dTexNum, true));

	constexpr UINT obj3dNum = 1;
	for (UINT i = 0; i < obj3dNum; i++) {
		obj3d.emplace_back(Object3d(DirectXCommon::getInstance()->getDev(), camera.get(), model.get(), obj3dTexNum));
		obj3d[i].scale = { obj3dScale, obj3dScale, obj3dScale };
		obj3d[i].position = { i * obj3dScale, 0, 0 };
		obj3d[i].rotation.y = 180.f;
	}

	lightObj.reset(new Object3d(Object3d(DirectXCommon::getInstance()->getDev(), camera.get(), model.get(), obj3dTexNum)));
	const float lightObjScale = obj3dScale * 0.5f;
	lightObj->scale = XMFLOAT3(lightObjScale, lightObjScale, lightObjScale);
	lightObj->position = obj3d[0].position;

#pragma endregion 3D�I�u�W�F�N�g

#pragma region FBX

	constexpr char fbxName[] = "boneTest";
	fbxModel.reset(FbxLoader::GetInstance()->loadModelFromFile(fbxName));

	fbxObj3d.reset(new FbxObj3d(fbxModel.get()/*, false*/));
	fbxObj3d->setScale(XMFLOAT3(0.0725f, 0.0725f, 0.0725f));

	fbxObj3d->playAnimation();

#pragma endregion FBX

	// �p�[�e�B�N��������
	particleMgr.reset(new ParticleManager(dxCom->getDev(), L"Resources/effect1.png", camera.get()));

	// ���ԏ�����
	timer.reset(new Time());
}

void PlayScene::init() {
	// �^�C�}�[�J�n
	timer->reset();
}

void PlayScene::update() {

	// SPACE��END�V�[����
	if (input->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(new EndScene());
	}

	backObj->rotation.y += 0.1f;

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
		debugText->formatPrint(spriteCommon.get(), 0, debugText->fontHeight * 2, 1.f, XMFLOAT4(1, 1, 1, 1), "BGM_STATE : %s", stateStr.c_str());

		debugText->Print(spriteCommon.get(), "0 : Play/Stop BGM", 0, debugText->fontHeight * 3);

		debugText->Print(spriteCommon.get(), "P : create particle(play SE)", 0, debugText->fontHeight * 4);
	}

#pragma endregion ��

#pragma region �}�E�X

	if (input->hitMouseBotton(Input::MOUSE::LEFT)) {
		debugText->Print(spriteCommon.get(), "input mouse left",
						 input->getMousePos().x, input->getMousePos().y, 0.75f);
	}
	if (input->hitMouseBotton(Input::MOUSE::RIGHT)) {
		debugText->Print(spriteCommon.get(), "input mouse right",
						 input->getMousePos().x,
						 input->getMousePos().y + debugText->fontHeight, 0.75f);
	}
	if (input->hitMouseBotton(Input::MOUSE::WHEEL)) {
		debugText->Print(spriteCommon.get(), "input mouse wheel",
						 input->getMousePos().x,
						 input->getMousePos().y + debugText->fontHeight * 2, 0.75f);
	}
	if (input->hitMouseBotton(VK_LSHIFT)) {
		debugText->Print(spriteCommon.get(), "LSHIFT(WinAPI)", 0, 0, 2);
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

	debugText->formatPrint(spriteCommon.get(), 0, 0, 1.f,
						   XMFLOAT4(1, 1, 1, 1), "FPS : %f", dxCom->getFPS());

	if (input->hitKey(DIK_R)) timer->reset();

	debugText->formatPrint(spriteCommon.get(), 0, debugText->fontHeight * 15, 1.f,
						   XMFLOAT4(1, 1, 1, 1),
						   "Time : %.6f[s]", (long double)timer->getNowTime() / Time::oneSec);

#pragma endregion ����

#pragma region ���\��

	if (input->triggerKey(DIK_T)) {
		debugText->tabSize++;
		if (input->hitKey(DIK_LSHIFT)) debugText->tabSize = 4U;
	}

	debugText->formatPrint(spriteCommon.get(), debugText->fontWidth * 2, debugText->fontHeight * 17, 1.f,
						   XMFLOAT4(1, 1, 1, 1),
						   "newLine\ntab(size %u)\tendString", debugText->tabSize);

	debugText->Print(spriteCommon.get(), "SPACE : end", 0, debugText->fontHeight * 6, 1.f, XMFLOAT4(1, 0.5f, 0.5f, 1));

	debugText->Print(spriteCommon.get(), "WASD : move camera", 0, debugText->fontHeight * 8);
	debugText->Print(spriteCommon.get(), "arrow : rotation camera", 0, debugText->fontHeight * 9);

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

	//obj3d[0].rotation.y++;

#pragma endregion �J�����ړ���]

#pragma region ���C�g
	{
		// ��b�ň��(2PI[rad])
		auto timeAngle = (float)timer->getNowTime() / Time::oneSec * XM_2PI;

		debugText->formatPrint(spriteCommon.get(),
							   WinAPI::window_width / 2.f, debugText->fontHeight * 16, 1.f,
							   XMFLOAT4(1, 1, 0, 1),
							   "light angle : %f PI [rad]\n\t\t\t->%f PI [rad]",
							   timeAngle / XM_PI,
							   angleRoundRad(timeAngle) / XM_PI);

		constexpr float lightR = 20.f;
		lightObj->position = obj3d[0].position;
		lightObj->position.x += nearSin(timeAngle) * lightR;
		lightObj->position.y += nearSin(timeAngle) * lightR;
		lightObj->position.z += nearCos(timeAngle) * lightR;

		/*for (auto& i : obj3d) {
			i.setLightPos(lightObj->position);
		}*/

		light->setLightDir(XMVectorSet(obj3d[0].position.x - lightObj->position.x,
									   obj3d[0].position.y - lightObj->position.y,
									   obj3d[0].position.z - lightObj->position.z,
									   1.f));

		light->update();
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
		createParticle(obj3d[0].position, particleNum, startScale);

		Sound::SoundPlayWave(soundCommon.get(), particleSE.get());
	}

#pragma endregion �X�v���C�g

	camera->update();

	// �w�i�I�u�W�F�N�g�̒��S���J�����ɂ���
	backObj->position = camera->getEye();
}

void PlayScene::drawObj3d() {

	Object3d::startDraw(dxCom->getCmdList(), backPipelineSet);
	backObj->drawWithUpdate(camera->getViewMatrix(), dxCom, light.get());

	ParticleManager::startDraw(dxCom->getCmdList(), object3dPipelineSet);
	particleMgr->drawWithUpdate(dxCom->getCmdList());

	Object3d::startDraw(dxCom->getCmdList(), object3dPipelineSet);
	lightObj->drawWithUpdate(camera->getViewMatrix(), dxCom, light.get());
	for (UINT i = 0; i < obj3d.size(); i++) {
		obj3d[i].drawWithUpdate(camera->getViewMatrix(), dxCom, light.get());
	}

	fbxObj3d->drawWithUpdate(dxCom->getCmdList());
}

void PlayScene::drawFrontSprite() {
	spriteCommon->drawStart(dxCom->getCmdList());
	// �X�v���C�g�`��
	for (UINT i = 0; i < _countof(sprites); i++) {
		sprites[i].drawWithUpdate(dxCom, spriteCommon.get());
	}
	// �f�o�b�O�e�L�X�g�`��
	debugText->DrawAll(dxCom, spriteCommon.get());
}

void PlayScene::createParticle(const DirectX::XMFLOAT3 &pos, const UINT particleNum, const float startScale) {
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

PlayScene::~PlayScene() {
	//Sound::SoundStopWave(soundData1.get());
}