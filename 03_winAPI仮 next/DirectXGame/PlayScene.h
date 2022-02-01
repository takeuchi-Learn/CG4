#pragma once
#include "GameScene.h"

#include <memory>
#include "Time.h"
#include "Sound.h"
#include "Sprite.h"
#include "Object3d.h"
#include "DebugText.h"

#include <DirectXMath.h>

#include "Sphere.h"

class PlayScene :
	public GameScene {

#pragma region ビュー変換行列

	DirectX::XMMATRIX matView;
	DirectX::XMFLOAT3 eye;   // 視点座標
	DirectX::XMFLOAT3 target;   // 注視点座標
	DirectX::XMFLOAT3 up;       // 上方向ベクトル

#pragma endregion ビュー変換行列

#pragma region 音

	Sound::SoundCommon* soundCommon = nullptr;

	Sound* soundData1 = nullptr;

#pragma endregion 音

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
	static const int SPRITES_NUM = 1;
	Sprite sprites[SPRITES_NUM];

	// --------------------
	// デバッグテキスト
	// --------------------
	DebugText debugText{};
	// デバッグテキスト用のテクスチャ番号を指定
	const UINT debugTextTexNumber = Sprite::spriteSRVCount - 1;
#pragma endregion スプライト

#pragma region 3Dオブジェクト

	// 3Dオブジェクト用パイプライン生成
	Object3d::PipelineSet object3dPipelineSet;

	const UINT obj3dTexNum = 0U;
	std::unique_ptr<Model> model;
	std::unique_ptr<Object3d> obj3d;
	const float obj3dScale = 10.f;

	DirectX::XMFLOAT2 angle{};	// 各軸周りの回転角



	std::unique_ptr<Sphere> sphere;

#pragma endregion 3Dオブジェクト


	std::unique_ptr<Time> timer;

public:
	void init() override;
	void update() override;
	void draw() override;
	void fin() override;
};

