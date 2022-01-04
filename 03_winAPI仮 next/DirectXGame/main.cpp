﻿#include <Windows.h>

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

#pragma region チャンク
// チャンクヘッダ
struct ChunkHeader {
	char id[4]; // チャンク毎のID
	int32_t size;  // チャンクサイズ
};

// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk;   // "RIFF"
	char type[4]; // "WAVE"
};

// FMTチャンク
struct FormatChunk {
	ChunkHeader chunk; // "fmt "
	WAVEFORMATEX fmt; // 波形フォーマット
};
#pragma endregion

#pragma region 音宣言
class XAudio2VoiceCallback : public IXAudio2VoiceCallback {
public:
	// ボイス処理パスの開始時
	STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 BytesRequired) {};
	// ボイス処理パスの終了時
	STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) {};
	// バッファストリームの再生が終了した時
	STDMETHOD_(void, OnStreamEnd) (THIS) {};
	// バッファの使用開始時
	STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) {};
	// バッファの末尾に達した時
	STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext) {};
	// 再生がループ位置に達した時
	STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) {};
	// ボイスの実行エラー時
	STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};
};

// 音声データ
struct SoundData {
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファの先頭アドレス
	BYTE* pBuffer;
	// バッファのサイズ
	unsigned int bufferSize;
};

// 音声読み込み
SoundData SoundLoadWave(const char* filename) {
	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open());

	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK ", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data ", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Waveファイルを閉じる
	file.close();

	// returnする為の音声データ
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

// 音声データ解放
void SoundUnload(SoundData* soundData) {
	// バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

// 音声再生
void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData) {

	HRESULT result;

	// 波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}
#pragma endregion

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//WindowsAPI初期化
	std::unique_ptr<WinAPI> winapi(new WinAPI(L"DirectXクラス化"));

	//DirectX初期化
	std::unique_ptr<DirectXCommon> dxCom(new DirectXCommon(winapi.get()));

	HRESULT result;

#pragma region 音初期化

	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	XAudio2VoiceCallback voiceCallback;

	// XAudioエンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	// マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));

	// 音声読み込み
	SoundData soundData1 = SoundLoadWave("Resources/Alarm01.wav");
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
	// 平面
	Plane plane;

	// --------------------
	// 球2平面初期化
	// --------------------

	// 球の初期値を設定
	sphere.center = XMVectorSet(0, 2, 0, 1);	// 中心座標
	sphere.radius = 1.f;	// 半径

	// 平面の初期値を設定
	plane.normal = XMVectorSet(0, 1, 0, 0);	//法線ベクトル
	plane.distance = 0.f;	// 原点からの距離

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

	// ゲームループ
	while (!winapi->processMessage()) {

#pragma region DirectX毎フレーム処理

		//キー入力更新
		input->updateHitState();
		if (input->hitKey(DIK_ESCAPE)) break;	//ESCで終了



		// 数字の0キーが押された瞬間
		if (input->triggerKey(DIK_0)) {
			OutputDebugStringA("Hit 0\n");  // 出力ウィンドウに「Hit 0」と表示
		}

		float clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f }; // 青っぽい色

		// スペースキーが押されていたら
		if (input->hitKey(DIK_SPACE)) {
			clearColor[1] = 1.0f;	// 画面クリアカラーの数値を書き換える
		}


		// --------------------
		// 球2平面更新
		// --------------------

		// 球移動
		{
			XMVECTOR moveY = XMVectorSet(0, 0.01f, 0, 0);
			if (input->hitKey(DIK_DOWN)) sphere.center += moveY;
			else if (input->hitKey(DIK_UP)) sphere.center -= moveY;

			XMVECTOR moveX = XMVectorSet(0.01f, 0, 0, 0);
			if (input->hitKey(DIK_RIGHT)) sphere.center += moveX;
			else if (input->hitKey(DIK_LEFT)) sphere.center -= moveX;
		}
		//stringstreamで変数の値を読み込み整形
		std::ostringstream spherestr;
		spherestr << "Sphere:("
			<< std::fixed << std::setprecision(2)	// 小数点以下2桁まで
			<< sphere.center.m128_f32[0] << ","		// x
			<< sphere.center.m128_f32[1] << ","		// y
			<< sphere.center.m128_f32[2] << ")";	// z

		debugText.Print(spriteCommon, spherestr.str(), 50, 180, 1.f);

		// 球と平面の当たり判定
		XMVECTOR inter;
		bool hit = Collision::CheckSphere2Plane(sphere, plane, &inter);
		if (hit) {
			debugText.Print(spriteCommon, "HIT", 50, 200, 1.f);
			// stringstreamをリセット、交点座標を埋め込む
			spherestr.str("");
			spherestr.clear();
			spherestr << "("
				<< std::fixed << std::setprecision(2)
				<< inter.m128_f32[0] << ","
				<< inter.m128_f32[1] << ","
				<< inter.m128_f32[2] << ")";

			debugText.Print(spriteCommon, spherestr.str(), 50, 220, 1.f);
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
		dxCom->startDraw();

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

#pragma region 音関連終了処理(クラス化してデストラクタで行う方がよい)
	// XAudio2解放
	xAudio2.Reset();
	// 音声データ解放
	SoundUnload(&soundData1);
#pragma endregion

	return 0;
}