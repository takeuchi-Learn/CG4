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

#pragma region 音

	Sound::SoundCommon* soundCommon = nullptr;

	Sound* soundData1 = nullptr;

#pragma endregion 音

#pragma region ビュー変換行列

	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);   // 視点座標
	XMFLOAT3 target(0, 0, 0);   // 注視点座標
	XMFLOAT3 up(0, 1, 0);       // 上方向ベクトル

#pragma endregion ビュー変換行列

#pragma region スプライト

	// --------------------
	// スプライト共通
	// --------------------
	Sprite::SpriteCommon spriteCommon;
	// スプライト共通テクスチャ読み込み
	enum TEX_NUM { TEX1, HOUSE };

	// --------------------
	// スプライト個別
	// --------------------
	constexpr int SPRITES_NUM = 1;
	Sprite sprites[SPRITES_NUM];

	// --------------------
	// デバッグテキスト
	// --------------------
	DebugText debugText;
	// デバッグテキスト用のテクスチャ番号を指定
	constexpr UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;

#pragma endregion スプライト

#pragma region 3Dオブジェクト
	// 3Dオブジェクト用パイプライン生成
	Object3d::PipelineSet object3dPipelineSet;

	constexpr UINT obj3dTexNum = 0U;
	std::unique_ptr<Model> model;
	std::unique_ptr<Object3d> obj3d;
	constexpr float obj3dScale = 10.f;

#pragma endregion 3Dオブジェクト

#pragma region 時間

	std::unique_ptr<Time> timer(new Time());

#pragma endregion 時間
}

void PlayScene::init() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : Play");

#pragma region 音

	soundCommon = new Sound::SoundCommon();
	soundData1 = new Sound("Resources/BGM.wav", soundCommon);

	// ビュー変換行列
	matView = XMMatrixLookAtLH(
		XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up)
	);

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
			TEX_NUM::HOUSE, spriteCommon, { 0,0 }, false, false
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

#pragma endregion 3Dオブジェクト

	timer->reset();
}

void PlayScene::update() {
	if (Input::getInstance()->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::END);
	}



#pragma region mainからそのまま移植

	// 数字の0キーが押された瞬間音を再生しなおす
	if (Input::getInstance()->triggerKey(DIK_0)) {
		//Sound::SoundStopWave(soundData1);

		if (Sound::checkPlaySound(soundData1)) {
			Sound::SoundStopWave(soundData1);
			OutputDebugStringA("STOP\n");
		} else {
			Sound::SoundPlayWave(soundCommon, soundData1, XAUDIO2_LOOP_INFINITE);
			OutputDebugStringA("PLAY\n");
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

	// --------------------
	// マウス
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
	// --------------------
	// マウスここまで
	// --------------------

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

	// --------------------
	// 球2平面更新ここまで
	// --------------------

	if (Input::getInstance()->hitKey(DIK_A) || Input::getInstance()->hitKey(DIK_D)) {
		float angle = 0.f;
		if (Input::getInstance()->hitKey(DIK_D)) { angle += XMConvertToRadians(1.0f); } else if (Input::getInstance()->hitKey(DIK_A)) { angle -= XMConvertToRadians(1.0f); }

		// angleラジアンだけY軸まわりに回転。半径は-100
		eye.x = -100 * sinf(angle);
		eye.z = -100 * cosf(angle);
		matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	}

	if (Input::getInstance()->hitKey(DIK_I)) sprites[0].position.y -= 10; else if (Input::getInstance()->hitKey(DIK_K)) sprites[0].position.y += 10;
	if (Input::getInstance()->hitKey(DIK_J)) sprites[0].position.x -= 10; else if (Input::getInstance()->hitKey(DIK_L)) sprites[0].position.x += 10;





	//// X座標,Y座標を指定して表示
	//debugText.Print(spriteCommon, "Hello,DirectX!!", 200, 100);
	//// X座標,Y座標,縮尺を指定して表示
	//debugText.Print(spriteCommon, "Nihon Kogakuin", 200, 200, 2.0f);

	//sprite.rotation = 45;
	//sprite.position = {1280/2, 720/2, 0};
	//sprite.color = {0, 0, 1, 1};
#pragma endregion
}

void PlayScene::draw() {

#pragma region mainからそのまま移植
	// ４．描画コマンドここから
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
#pragma endregion
}

void PlayScene::fin() {
	Sound::SoundStopWave(soundData1);
	delete soundData1;
	delete soundCommon;
}
