#include "PlayScene.h"

#include "SceneManager.h"

#include "Input.h"

#include <sstream>
#include <iomanip>

#include <xaudio2.h>


using namespace DirectX;

namespace {
#pragma region XMFLOAT3operator

	XMFLOAT3 operator-(const XMFLOAT3& num1, const XMFLOAT3& num2) {
		return XMFLOAT3(
		num1.x - num2.x,
		num1.y - num2.y,
		num1.z - num2.z);
	}
	XMFLOAT3 operator+(const XMFLOAT3& num1, const XMFLOAT3& num2) {
		return XMFLOAT3(
		num1.x + num2.x,
		num1.y + num2.y,
		num1.z + num2.z);
	}

#pragma endregion XMFLOAT3operator

#pragma region �J�����֐��n

	// ���K��
	inline XMFLOAT3 normalVec(const XMFLOAT3& vec) {
		// �����x�N�g��
		XMFLOAT3 look = vec;
		// XMVECTOR���o�R���Đ��K��
		const XMVECTOR normalLookVec = XMVector3Normalize(XMLoadFloat3(&look));
		//XMFLOAT3�ɖ߂�
		XMStoreFloat3(&look, normalLookVec);

		return look;
	}

	// @return pos -> target�̒P�ʃx�N�g��
	XMFLOAT3 getLook(const XMFLOAT3& target_local, const XMFLOAT3& pos) {
		return normalVec(target_local - pos);
	}

	XMFLOAT3 getCameraMoveVal(const float moveSpeed, const XMFLOAT3& eye_local, const XMFLOAT3& target_local) {
		// �����x�N�g��
		const XMFLOAT3 look = getLook(target_local, eye_local);

		const XMFLOAT3 moveVal{
			moveSpeed * look.x,
			moveSpeed * look.y,
			moveSpeed * look.z
		};

		return moveVal;
	}

	/// <summary>
	/// �J��������]
	/// </summary>
	/// <param name="target">�����_���W�̕ϐ�(���o��)</param>
	/// <param name="eye">�J�����̈ʒu</param>
	/// <param name="targetlength">�J�������璍���_�܂ł̋���</param>
	/// <param name="angleX">X������̉�]�p(-PI/2 ~ PI/2�͈̔͂ő���)</param>
	/// <param name="angleY">Y������̉�]�p(0 ~ 2PI�͈̔͂ő���)</param>
	void cameraRotation(XMFLOAT3& target_local, const XMFLOAT3& eye_local, const float targetlength,
						const float angleX, const float angleY) {
		// �����x�N�g��
		const XMFLOAT3 look = getLook(target_local, eye_local);

		constexpr float lookLen = 50.f;
		target_local = eye_local;
		target_local.x += targetlength * sinf(angleY) + look.x * lookLen;
		target_local.y += targetlength * sinf(angleX) + look.y * lookLen;
		target_local.z += targetlength * cosf(angleY) + look.z * lookLen;
	}

	// ���������Ɉړ�
	void cameraMoveForward(const float moveSpeed, XMFLOAT3& eye_local, const XMFLOAT3& target_local) {
		const XMFLOAT3 moveVal = getCameraMoveVal(moveSpeed, eye_local, target_local);

		eye_local.x += moveVal.x;
		eye_local.y += moveVal.y;
		eye_local.z += moveVal.z;
	}

	// ����������O�i�Ƃ�������ŉE�Ɉړ�
	void cameraMoveRight(const float moveSpeed, XMFLOAT3& eye_local, const XMFLOAT3& target_local) {
		const XMFLOAT3 moveVal = getCameraMoveVal(moveSpeed, eye_local, target_local);

		eye_local.z -= moveVal.x;
		eye_local.y += moveVal.y;
		eye_local.x += moveVal.z;
	}

	// ����������O�i�Ƃ�������ŏ�Ɉړ�(�����Ă���������)
	void cameraMoveUp(const float moveSpeed, XMFLOAT3& eye_local, const XMFLOAT3& target_local) {
		const XMFLOAT3 moveVal = getCameraMoveVal(moveSpeed, eye_local, target_local);

		eye_local.y -= moveVal.y;
	}

	// matView���X�V����
	void updateMatView(XMMATRIX& matView, const XMFLOAT3& eye_local, const XMFLOAT3& target_local, const XMFLOAT3& up_local) {
		matView = XMMatrixLookAtLH(XMLoadFloat3(&eye_local), XMLoadFloat3(&target_local), XMLoadFloat3(&up_local));
	}

#pragma endregion �J�����֐��n
}

void PlayScene::init() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : Play");

#pragma region �r���[�ϊ�

	eye_local = XMFLOAT3(0, 0, -100);   // ���_���W
	target_local = XMFLOAT3(0, 0, 0);   // �����_���W
	up_local = XMFLOAT3(0, 1, 0);       // ������x�N�g��

	camera.reset(new Camera(WinAPI::window_width, WinAPI::window_height));
	camera->setEye(eye_local);
	camera->setTarget(target_local);
	camera->setUp(up_local);
	camera->setUp(up_local);

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
			//OutputDebugStringA("STOP\n");
		} else {
			Sound::SoundPlayWave(soundCommon.get(), soundData1.get(), XAUDIO2_LOOP_INFINITE);
			//OutputDebugStringA("PLAY\n");
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

	{
		std::ostringstream raystr{};
		raystr << "FPS : " << DirectXCommon::getInstance()->getFPS();
		debugText.Print(spriteCommon, raystr.str(), 0, 0);
	}

	{
		if (Input::getInstance()->hitKey(DIK_R)) timer->reset();

		std::ostringstream tmpStr{};
		tmpStr << "Time : "
			<< std::fixed << std::setprecision(6)
			<< (long double)timer->getNowTime() / Time::oneSec << "[s]";
		debugText.Print(spriteCommon, tmpStr.str(), 0, debugText.fontHeight * 5);
	}

#pragma endregion ����

#pragma region �J�����ړ���]

	if (Input::getInstance()->hitKey(DIK_A) || Input::getInstance()->hitKey(DIK_D) ||
		Input::getInstance()->hitKey(DIK_S) || Input::getInstance()->hitKey(DIK_W) ||
		Input::getInstance()->hitKey(DIK_E) || Input::getInstance()->hitKey(DIK_C) ||
		Input::getInstance()->hitKey(DIK_UP) || Input::getInstance()->hitKey(DIK_DOWN) ||
		Input::getInstance()->hitKey(DIK_LEFT) || Input::getInstance()->hitKey(DIK_RIGHT)) {

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
		cameraRotation(target_local, eye_local, camRange, angle.x, angle.y);


		// todo �J�����̈ړ����֐�������(�J�����ȊO�ɂ��g������)
		// �ړ���
		constexpr float moveSpeed = 1.25f;
		// ���_�ړ�
		if (Input::getInstance()->hitKey(DIK_W)) {
			cameraMoveForward(moveSpeed, eye_local, target_local);
		} else if (Input::getInstance()->hitKey(DIK_S)) {
			cameraMoveForward(-moveSpeed, eye_local, target_local);
		}
		if (Input::getInstance()->hitKey(DIK_A)) {
			cameraMoveRight(-moveSpeed, eye_local, target_local);
		} else if (Input::getInstance()->hitKey(DIK_D)) {
			cameraMoveRight(moveSpeed, eye_local, target_local);
		}
		if (Input::getInstance()->hitKey(DIK_E)) {
			cameraMoveUp(moveSpeed, eye_local, target_local);
		} else if (Input::getInstance()->hitKey(DIK_C)) {
			cameraMoveUp(-moveSpeed, eye_local, target_local);
		}

		//updateMatView(matView, eye, target, up);
		camera->setEye(eye_local);
		camera->setTarget(target_local);
		camera->setUp(up_local);
	}

#pragma endregion �J�����ړ���]

	if (Input::getInstance()->hitKey(DIK_I)) sprites[0].position.y -= 10; else if (Input::getInstance()->hitKey(DIK_K)) sprites[0].position.y += 10;
	if (Input::getInstance()->hitKey(DIK_J)) sprites[0].position.x -= 10; else if (Input::getInstance()->hitKey(DIK_L)) sprites[0].position.x += 10;


	camera->update();
}

void PlayScene::draw() {
	// �S�D�`��R�}���h��������
	// ���̃R�}���h
	Sphere::sphereCommonBeginDraw(object3dPipelineSet);
	sphere->drawWithUpdate(camera->getViewMatrix(), DirectXCommon::getInstance());
	// 3D�I�u�W�F�N�g�R�}���h
	Object3d::Object3dCommonBeginDraw(DirectXCommon::getInstance()->getCmdList(), object3dPipelineSet);
	obj3d->drawWithUpdate(camera->getViewMatrix(), DirectXCommon::getInstance());
	// �X�v���C�g���ʃR�}���h
	Sprite::SpriteCommonBeginDraw(spriteCommon, DirectXCommon::getInstance()->getCmdList());
	// �X�v���C�g�`��
	for (UINT i = 0; i < _countof(sprites); i++) {
		sprites[i].SpriteDrawWithUpdate(DirectXCommon::getInstance(), spriteCommon);
	}
	// �f�o�b�O�e�L�X�g�`��
	debugText.DrawAll(DirectXCommon::getInstance(), spriteCommon);
	// �S�D�`��R�}���h�����܂�
}

void PlayScene::fin() {
	//Sound::SoundStopWave(soundData1.get());
}
