#include "PlayScene.h"

#include "SceneManager.h"

#include "Input.h"

#include <sstream>
#include <iomanip>

#include <xaudio2.h>

using namespace DirectX;

namespace {
	// 正規化
	inline XMFLOAT3 normalVec(const XMFLOAT3& vec) {
		// 視線ベクトル
		XMFLOAT3 look = vec;
		// XMVECTORを経由して正規化
		const XMVECTOR normalLookVec = XMVector3Normalize(XMLoadFloat3(&look));
		//XMFLOAT3に戻す
		XMStoreFloat3(&look, normalLookVec);

		return look;
	}

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

	// @return pos -> targetの単位ベクトル
	XMFLOAT3 getLook(const XMFLOAT3& target, const XMFLOAT3& pos) {
		return normalVec(target - pos);
	}

	XMFLOAT3 getCameraMoveVal(const float moveSpeed, const XMFLOAT3& eye, const XMFLOAT3& target) {
		// 視線ベクトル
		const XMFLOAT3 look = getLook(target, eye);

		const XMFLOAT3 moveVal{
			moveSpeed * look.x,
			moveSpeed * look.y,
			moveSpeed * look.z
		};

		return moveVal;
	}

	/// <summary>
	/// カメラを回転
	/// </summary>
	/// <param name="target">注視点座標の変数(入出力)</param>
	/// <param name="eye">カメラの位置</param>
	/// <param name="targetlength">カメラから注視点までの距離</param>
	/// <param name="angleX">X軸周りの回転角</param>
	/// <param name="angleY">Y軸周りの回転角</param>
	void cameraRotation(XMFLOAT3& target, const XMFLOAT3& eye, const float targetlength,
						const float angleX, const float angleY) {
		// 視線ベクトル
		const XMFLOAT3 look = getLook(target, eye);

		constexpr float lookLen = 50.f;
		target = eye;
		target.x += targetlength * sinf(angleY) + look.x * lookLen;
		target.y += targetlength * sinf(angleX) + look.y * lookLen;
		target.z += targetlength * cosf(angleY) + look.z * lookLen;
	}

	// 視線方向に移動
	void cameraMoveForward(const float moveSpeed, XMFLOAT3& eye, const XMFLOAT3& target) {
		const XMFLOAT3 moveVal = getCameraMoveVal(moveSpeed, eye, target);

		eye.x += moveVal.x;
		eye.y += moveVal.y;
		eye.z += moveVal.z;
	}

	// 視線方向を前進とする向きで右に移動
	void cameraMoveRight(const float moveSpeed, XMFLOAT3& eye, const XMFLOAT3& target) {
		const XMFLOAT3 moveVal = getCameraMoveVal(moveSpeed, eye, target);

		eye.z -= moveVal.x;
		eye.y += moveVal.y;
		eye.x += moveVal.z;
	}

	// 視線方向を前進とする向きで上に移動(無くてもいいかも)
	void cameraMoveUp(const float moveSpeed, XMFLOAT3& eye, const XMFLOAT3& target) {
		const XMFLOAT3 moveVal = getCameraMoveVal(moveSpeed, eye, target);

		eye.y -= moveVal.y;
	}

	// matViewを更新する
	void updateMatView(XMMATRIX& matView, const XMFLOAT3& eye, const XMFLOAT3& target, const XMFLOAT3& up) {
		matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	}
}

void PlayScene::init() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : Play");

#pragma region ビュー変換

	eye = XMFLOAT3(0, 0, -100);   // 視点座標
	target = XMFLOAT3(0, 0, 0);   // 注視点座標
	up = XMFLOAT3(0, 1, 0);       // 上方向ベクトル
	matView = XMMatrixLookAtLH(
		XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up)
	);

#pragma endregion ビュー変換

#pragma region 音

	soundCommon = new Sound::SoundCommon();
	soundData1 = new Sound("Resources/BGM.wav", soundCommon);

#pragma endregion 音

#pragma region スプライト

	// --------------------
	// スプライト共通
	// --------------------
	spriteCommon = Sprite::SpriteCommonCreate(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height);

	// スプライト共通テクスチャ読み込み
	Sprite::SpriteCommonLoadTexture(spriteCommon, TEX_NUM::TEX1, L"Resources/texture.png", DirectXCommon::getInstance()->getDev());
	Sprite::SpriteCommonLoadTexture(spriteCommon, TEX_NUM::HOUSE, L"Resources/house.png", DirectXCommon::getInstance()->getDev());

	// スプライトの生成
	for (int i = 0; i < _countof(sprites); i++) {
		sprites[i].SpriteCreate(
			DirectXCommon::getInstance()->getDev(),
			WinAPI::window_width, WinAPI::window_height,
			TEX_NUM::TEX1, spriteCommon, { 0,0 }, false, false
		);
		// スプライトの座標変更
		sprites[i].position.x = 1280 / 4;
		sprites[i].position.y = 720 / 4;
		//sprites[i].isInvisible = true;
		//sprites[i].position.x = (float)(rand() % 1280);
		//sprites[i].position.y = (float)(rand() % 720);
		//sprites[i].rotation = (float)(rand() % 360);
		//sprites[i].rotation = 0;
		//sprites[i].size.x = 400.0f;
		//sprites[i].size.y = 100.0f;
		// 頂点バッファに反映
		sprites[i].SpriteTransferVertexBuffer(spriteCommon);
	}

	// デバッグテキスト用のテクスチャ読み込み
	Sprite::SpriteCommonLoadTexture(spriteCommon, debugTextTexNumber, L"Resources/debugfont.png", DirectXCommon::getInstance()->getDev());
	// デバッグテキスト初期化
	debugText.Initialize(DirectXCommon::getInstance()->getDev(), WinAPI::window_width, WinAPI::window_height, debugTextTexNumber, spriteCommon);

	// 3Dオブジェクト用パイプライン生成
	object3dPipelineSet = Object3d::Object3dCreateGraphicsPipeline(DirectXCommon::getInstance()->getDev());


#pragma endregion スプライト

#pragma region 3Dオブジェクト

	model.reset(new Model(DirectXCommon::getInstance()->getDev(),
						  L"Resources/model/model.obj", L"Resources/model/tex.png",
						  WinAPI::window_width, WinAPI::window_height, Object3d::constantBufferNum, obj3dTexNum)
	);
	obj3d.reset(new Object3d(DirectXCommon::getInstance()->getDev(), model.get(), obj3dTexNum));
	obj3d->scale = { obj3dScale, obj3dScale, obj3dScale };
	obj3d->position = { 0, 0, obj3dScale };

	sphere.reset(new Sphere(DirectXCommon::getInstance()->getDev(), 2.f, L"Resources/red.png", 0));

#pragma endregion 3Dオブジェクト

	timer.reset(new Time());
	timer->reset();
}

void PlayScene::update() {

	if (Input::getInstance()->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::END);
	}

#pragma region 音

	// 数字の0キーが押された瞬間音を再生しなおす
	if (Input::getInstance()->triggerKey(DIK_0)) {
		//Sound::SoundStopWave(soundData1);

		if (Sound::checkPlaySound(soundData1)) {
			Sound::SoundStopWave(soundData1);
			//OutputDebugStringA("STOP\n");
		} else {
			Sound::SoundPlayWave(soundCommon, soundData1, XAUDIO2_LOOP_INFINITE);
			//OutputDebugStringA("PLAY\n");
		}
	}

	{
		std::string stateStr = "STOP []";
		if (Sound::checkPlaySound(soundData1)) {
			stateStr = "PLAY |>";
		}
		debugText.Print(spriteCommon, "SOUND_PLAY_STATE : " + stateStr, 0, debugText.fontHeight * 2);

		stateStr = "Press 0 to Play/Stop Sound";
		debugText.Print(spriteCommon, stateStr, 0, debugText.fontHeight * 3);
	}

#pragma endregion 音

#pragma region マウス

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

	// Rを押すたびマウスカーソルの表示非表示を切り替え
	if (Input::getInstance()->triggerKey(DIK_R)) {
		static bool mouseDispFlag = true;
		mouseDispFlag = !mouseDispFlag;
		Input::getInstance()->changeDispMouseCursorFlag(mouseDispFlag);
	}

	// Mキーでマウスカーソル位置を0,0に移動
	if (Input::getInstance()->triggerKey(DIK_M)) {
		Input::getInstance()->setMousePos(0, 0);
	}

#pragma endregion マウス

#pragma region 時間

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
		debugText.Print(spriteCommon, tmpStr.str(), 0, debugText.fontHeight * 11);
	}

#pragma endregion 時間

#pragma region カメラ移動回転

	if (Input::getInstance()->hitKey(DIK_A) || Input::getInstance()->hitKey(DIK_D) ||
		Input::getInstance()->hitKey(DIK_S) || Input::getInstance()->hitKey(DIK_W) ||
		Input::getInstance()->hitKey(DIK_E) || Input::getInstance()->hitKey(DIK_C) ||
		Input::getInstance()->hitKey(DIK_UP) || Input::getInstance()->hitKey(DIK_DOWN) ||
		Input::getInstance()->hitKey(DIK_LEFT) || Input::getInstance()->hitKey(DIK_RIGHT)) {

		const float rotaVal = XM_PI / DirectXCommon::getInstance()->getFPS();	// 毎秒半周

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

		// angleラジアンだけY軸まわりに回転。半径は-100
		constexpr float camRange = 100.f;	// targetLength
		cameraRotation(target, eye, camRange, angle.x, angle.y);


		// todo カメラの移動も関数化する(カメラ以外にも使えそう)
		// 移動量
		constexpr float moveSpeed = 1.25f;
		const XMFLOAT3 moveVal = getCameraMoveVal(moveSpeed, eye, target);
		XMFLOAT3 moveDiff{};
		// 視点移動
		if (Input::getInstance()->hitKey(DIK_W)) {
			cameraMoveForward(moveSpeed, eye, target);
		} else if (Input::getInstance()->hitKey(DIK_S)) {
			cameraMoveForward(-moveSpeed, eye, target);
		}
		if (Input::getInstance()->hitKey(DIK_A)) {
			cameraMoveRight(-moveSpeed, eye, target);
		} else if (Input::getInstance()->hitKey(DIK_D)) {
			cameraMoveRight(moveSpeed, eye, target);
		}
		if (Input::getInstance()->hitKey(DIK_E)) {
			cameraMoveUp(moveSpeed, eye, target);
		} else if (Input::getInstance()->hitKey(DIK_C)) {
			cameraMoveUp(-moveSpeed, eye, target);
		}

		updateMatView(matView, eye, target, up);
	}

#pragma endregion カメラ移動回転

	if (Input::getInstance()->hitKey(DIK_I)) sprites[0].position.y -= 10; else if (Input::getInstance()->hitKey(DIK_K)) sprites[0].position.y += 10;
	if (Input::getInstance()->hitKey(DIK_J)) sprites[0].position.x -= 10; else if (Input::getInstance()->hitKey(DIK_L)) sprites[0].position.x += 10;
}

void PlayScene::draw() {
	// ４．描画コマンドここから
	// 球体コマンド
	Sphere::sphereCommonBeginDraw(object3dPipelineSet);
	sphere->drawWithUpdate(matView, DirectXCommon::getInstance());
	// 3Dオブジェクトコマンド
	Object3d::Object3dCommonBeginDraw(DirectXCommon::getInstance()->getCmdList(), object3dPipelineSet);
	obj3d->drawWithUpdate(matView, DirectXCommon::getInstance());
	// スプライト共通コマンド
	Sprite::SpriteCommonBeginDraw(spriteCommon, DirectXCommon::getInstance()->getCmdList());
	// スプライト描画
	for (UINT i = 0; i < _countof(sprites); i++) {
		sprites[i].SpriteDrawWithUpdate(DirectXCommon::getInstance(), spriteCommon);
	}
	// デバッグテキスト描画
	debugText.DrawAll(DirectXCommon::getInstance(), spriteCommon);
	// ４．描画コマンドここまで
}

void PlayScene::fin() {
	Sound::SoundStopWave(soundData1);
	delete soundData1;
	delete soundCommon;
}
