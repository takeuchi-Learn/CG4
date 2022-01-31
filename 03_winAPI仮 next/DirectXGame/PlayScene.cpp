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

#pragma region mainからそのまま移植
	std::unique_ptr<Sound::SoundCommon> soundCommon;

	std::unique_ptr<Sound> soundData1;

	// --------------------
	// ビュー変換行列
	// --------------------
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);   // 視点座標
	XMFLOAT3 target(0, 0, 0);   // 注視点座標
	XMFLOAT3 up(0, 1, 0);       // 上方向ベクトル

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

	// 3Dオブジェクト用パイプライン生成
	Object3d::PipelineSet object3dPipelineSet;

	// --------------------
	// 当たり判定
	// --------------------
	// 球
	Sphere sphere;
	//// 平面
	Plane plane;
	// 三角形
	Triangle triangle;
	// レイ
	Ray ray;

	// --------------------
	// ループ前宣言定義
	// --------------------
	int animFrameCount = 0; // アニメーションの経過時間カウンター
	constexpr UINT obj3dTexNum = 0U;
	std::unique_ptr<Model> model;
	std::unique_ptr<Object3d> obj3d;
	constexpr float obj3dScale = 10.f;


	// --------------------
	// 時間
	// --------------------
	std::unique_ptr<Time> timer(new Time());


#pragma endregion
}

void PlayScene::init() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : Play");

#pragma region mainからそのまま移植
	if (firstFlag) {
		soundCommon.reset(new Sound::SoundCommon());
		soundData1.reset(
			new Sound("Resources/BGM.wav", soundCommon.get())
		);

		// ビュー変換行列
		matView = XMMatrixLookAtLH(
			XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up)
		);
	}

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

	// --------------------
	// 球2平面初期化
	// --------------------
	// 球の初期値を設定
	sphere.center = XMVectorSet(0, 2, 0, 1);	// 中心座標
	sphere.radius = 2.f;	// 半径c
	// 平面の初期値を設定
	plane.normal = XMVectorSet(0, 1, 0, 0);	//法線ベクトル
	plane.distance = 0.f;	// 原点からの距離
	// 三角形の初期値を設定
	triangle.p0 = XMVectorSet(-1.f, 0, -1.f, 1);	//左手前
	triangle.p1 = XMVectorSet(-1.f, 0, +1.f, 1);	//左奥
	triangle.p2 = XMVectorSet(+1.f, 0, -1.f, 1);	//右手前
	triangle.normal = XMVectorSet(0, 1, -0, 0);		//法線
	// レイの初期値を設定
	ray.start = XMVectorSet(0, 1, 0, 1);	//原点やや上
	ray.dir = XMVectorSet(0, -1, 0, 0);		//下向き


	// --------------------
	// ループ前宣言定義
	// --------------------
	model.reset(new Model(DirectXCommon::getInstance()->getDev(),
						  L"Resources/model/model.obj", L"Resources/model/tex.png",
						  WinAPI::window_width, WinAPI::window_height, Object3d::constantBufferNum, obj3dTexNum)
	);
	obj3d.reset(new Object3d(DirectXCommon::getInstance()->getDev(), model.get(), obj3dTexNum));
	obj3d->scale = { obj3dScale, obj3dScale, obj3dScale };
	obj3d->position = { 0, 0, obj3dScale };


	// --------------------
	// 音声再生
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



#pragma region mainからそのまま移植

	// 数字の0キーが押された瞬間音を再生しなおす
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

	// --------------------
	// 球2平面更新
	// --------------------

	// レイ移動
	{
		XMVECTOR moveZ = XMVectorSet(0, 0, 0.01f, 0);
		if (Input::getInstance()->hitKey(DIK_DOWN)) ray.start += moveZ;
		else if (Input::getInstance()->hitKey(DIK_UP)) ray.start -= moveZ;

		XMVECTOR moveX = XMVectorSet(0.01f, 0, 0, 0);
		if (Input::getInstance()->hitKey(DIK_RIGHT)) ray.start += moveX;
		else if (Input::getInstance()->hitKey(DIK_LEFT)) ray.start -= moveX;
	}
	//stringstreamで変数の値を読み込み整形
	std::ostringstream raystr;
	raystr << "lay.start("
		<< std::fixed << std::setprecision(2)	// 小数点以下2桁まで
		<< ray.start.m128_f32[0] << ","		// x
		<< ray.start.m128_f32[1] << ","		// y
		<< ray.start.m128_f32[2] << ")";	// z

	debugText.Print(spriteCommon, raystr.str(), debugText.fontWidth * 2, debugText.fontHeight * 5, 1.f);

	raystr.str("");
	raystr.clear();
	raystr << "FPS : " << DirectXCommon::getInstance()->getFPS();
	debugText.Print(spriteCommon, raystr.str(), 0, 0);

	// レイと球の当たり判定
	XMVECTOR inter;
	float distance;
	bool hit = Collision::CheckRay2Sphere(ray, sphere, &distance, &inter);
	if (hit) {
		debugText.Print(spriteCommon, "HIT", debugText.fontWidth * 2, debugText.fontHeight * 7, 1.f);
		// stringstreamをリセット、交点座標を埋め込む
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
	Sound::SoundStopWave(soundData1.get());
}
