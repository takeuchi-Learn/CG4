#include <Windows.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")

#include <d3dx12.h>

#include <vector>
#include <string>
#include <fstream>
#include <DirectXMath.h>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <DirectXTex.h>
#include <wrl.h>

#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

#include "Input.h"

#include "WinAPI.h"

#include "DirectXCommon.h"

#include "ObjCommon.h"

#include "Sprite.h"

#include "DebugText.h"

#include <sstream>
#include <fstream>

#include "Model.h"
#include "Object3d.h"
#include <memory>

#include "CollisionPrimitive.h"

#include "Collision.h"
#include <iomanip>

#include "Sound.h"

#include "Time.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//WindowsAPI初期化
	std::unique_ptr<WinAPI> winapi(new WinAPI(L"DirectXクラス化"));

	//DirectX初期化
	std::unique_ptr<DirectXCommon> dxCom(new DirectXCommon(winapi.get()));

#pragma region 音初期化

	std::unique_ptr<Sound::SoundCommon> soundCommon(new Sound::SoundCommon());

	// 音声読み込み
	std::unique_ptr <Sound> soundData1(new Sound("Resources/BGM.wav", soundCommon.get()));
#pragma endregion 音初期化

#pragma region 描画初期化処理

	// --------------------
	// ビュー変換行列
	// --------------------
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);   // 視点座標
	XMFLOAT3 target(0, 0, 0);   // 注視点座標
	XMFLOAT3 up(0, 1, 0);       // 上方向ベクトル
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));




	// --------------------
	// スプライト共通
	// --------------------

	Sprite::SpriteCommon spriteCommon = Sprite::SpriteCommonCreate(dxCom->getDev(), WinAPI::window_width, WinAPI::window_height);

	// スプライト共通テクスチャ読み込み
	enum TEX_NUM { TEX1, HOUSE };
	Sprite::SpriteCommonLoadTexture(spriteCommon, TEX_NUM::TEX1, L"Resources/texture.png", dxCom->getDev());
	Sprite::SpriteCommonLoadTexture(spriteCommon, TEX_NUM::HOUSE, L"Resources/house.png", dxCom->getDev());

	// --------------------
	// スプライト個別
	// --------------------

	const int SPRITES_NUM = 1;
	Sprite sprites[SPRITES_NUM];

	// スプライトの生成
	for (int i = 0; i < _countof(sprites); i++) {
		sprites[i].SpriteCreate(dxCom->getDev(), WinAPI::window_width, WinAPI::window_height, TEX_NUM::HOUSE, spriteCommon, { 0,0 }, false, false);

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



	// --------------------
	// デバッグテキスト
	// --------------------

	DebugText debugText;

	// デバッグテキスト用のテクスチャ番号を指定
	const int debugTextTexNumber = 2;
	// デバッグテキスト用のテクスチャ読み込み
	Sprite::SpriteCommonLoadTexture(spriteCommon, debugTextTexNumber, L"Resources/debugfont.png", dxCom->getDev());
	// デバッグテキスト初期化
	debugText.Initialize(dxCom->getDev(), WinAPI::window_width, WinAPI::window_height, debugTextTexNumber, spriteCommon);


	// 3Dオブジェクト用パイプライン生成
	Object3d::PipelineSet object3dPipelineSet = Object3d::Object3dCreateGraphicsPipeline(dxCom->getDev());

#pragma endregion 描画初期化処理

#pragma region 当たり判定
	// 球
	Sphere sphere;
	//// 平面
	Plane plane;
	// 三角形
	Triangle triangle;
	// レイ
	Ray ray;

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

#pragma endregion



#pragma region 入力初期化

	std::unique_ptr<Input> input(new Input(winapi->getW().hInstance, winapi->getHwnd()));

#pragma endregion 入力初期化


#pragma region ループ前定義宣言

	int animFrameCount = 0; // アニメーションの経過時間カウンター

	std::unique_ptr<Model> model(new Model(dxCom->getDev(),
		L"Resources/model/model.obj", L"Resources/model/tex.png",
		WinAPI::window_width, WinAPI::window_height, Object3d::constantBufferNum));

	std::unique_ptr<Object3d> obj3d(new Object3d(dxCom->getDev(), model.get()));
	const float obj3dScale = 10.f;
	obj3d->scale = { obj3dScale, obj3dScale, obj3dScale };
	obj3d->position = { 0, 0, obj3dScale };

#pragma endregion



#pragma region 時間
	std::unique_ptr<Time> timer(new Time());
#pragma endregion



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


	// ゲームループ
	while (!winapi->processMessage()) {

#pragma region DirectX毎フレーム処理

		//キー入力更新
		input->updateHitState();
		if (input->hitKey(DIK_ESCAPE)) break;	//ESCで終了



			// 数字の0キーが押された瞬間音を再生しなおす
		if (input->triggerKey(DIK_0)) {
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
			const std::string tmp = "SOUND_PLAY_STATE : ";
			std::string stateStr = "STOP []";
			if (Sound::checkPlaySound(soundData1.get())) {
				stateStr = "PLAY |>";
			}
			debugText.Print(spriteCommon, tmp + stateStr, 0, debugText.fontHeight * 2);

			stateStr = "Press 0 to Play/Stop Sound";
			debugText.Print(spriteCommon, stateStr, 0, debugText.fontHeight * 3);
		}



		// --------------------
		// 球2平面更新
		// --------------------

		// レイ移動
		{
			XMVECTOR moveZ = XMVectorSet(0, 0, 0.01f, 0);
			if (input->hitKey(DIK_DOWN)) ray.start += moveZ;
			else if (input->hitKey(DIK_UP)) ray.start -= moveZ;

			XMVECTOR moveX = XMVectorSet(0.01f, 0, 0, 0);
			if (input->hitKey(DIK_RIGHT)) ray.start += moveX;
			else if (input->hitKey(DIK_LEFT)) ray.start -= moveX;
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
		raystr << "FPS : " << dxCom->getFPS();
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
			if (input->hitKey(DIK_SPACE)) timer->reset();

			std::ostringstream tmpStr{};
			tmpStr << std::fixed << std::setprecision(6) <<
				(long double)timer->getNowTime() / Time::oneSec << "[s]";
			debugText.Print(spriteCommon, tmpStr.str(), 0, debugText.fontHeight * 11);
		}

		// --------------------
		// 球2平面更新ここまで
		// --------------------

		if (input->hitKey(DIK_A) || input->hitKey(DIK_D)) {
			float angle = 0.f;
			if (input->hitKey(DIK_D)) { angle += XMConvertToRadians(1.0f); } else if (input->hitKey(DIK_A)) { angle -= XMConvertToRadians(1.0f); }

			// angleラジアンだけY軸まわりに回転。半径は-100
			eye.x = -100 * sinf(angle);
			eye.z = -100 * cosf(angle);
			matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
		}

		if (input->hitKey(DIK_I))sprites[0].position.y -= 10; else if (input->hitKey(DIK_K))sprites[0].position.y += 10;
		if (input->hitKey(DIK_J))sprites[0].position.x -= 10; else if (input->hitKey(DIK_L))sprites[0].position.x += 10;

		// --------------------
		// 3Dオブジェクト更新
		// --------------------
		obj3d->update(matView);





		//// X座標,Y座標を指定して表示
		//debugText.Print(spriteCommon, "Hello,DirectX!!", 200, 100);
		//// X座標,Y座標,縮尺を指定して表示
		//debugText.Print(spriteCommon, "Nihon Kogakuin", 200, 200, 2.0f);

		//sprite.rotation = 45;
		//sprite.position = {1280/2, 720/2, 0};
		//sprite.color = {0, 0, 1, 1};

#pragma endregion DirectX毎フレーム処理

#pragma region グラフィックスコマンド
		const XMFLOAT3 clearColor = { 0.1f, 0.25f, 0.5f };	//青っぽい色
		dxCom->startDraw(clearColor);

		// ４．描画コマンドここから

		Object3d::Object3dCommonBeginDraw(dxCom->getCmdList(), object3dPipelineSet.pipelinestate.Get(), object3dPipelineSet.rootsignature.Get());



		obj3d->draw(dxCom->getCmdList(), dxCom->getDev());

		// スプライト共通コマンド
		Sprite::SpriteCommonBeginDraw(spriteCommon, dxCom->getCmdList());
		// スプライト描画
		for (int i = 0; i < _countof(sprites); i++) {
			sprites[i].SpriteDrawWithUpdate(dxCom->getCmdList(), spriteCommon, dxCom->getDev());
		}
		// デバッグテキスト描画
		debugText.DrawAll(dxCom->getCmdList(), spriteCommon, dxCom->getDev());

		// ４．描画コマンドここまで

		dxCom->endDraw();
#pragma endregion グラフィックスコマンド

	}

	return 0;
}