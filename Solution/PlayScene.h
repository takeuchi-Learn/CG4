#pragma once
#include "GameScene.h"

#include <memory>
#include "Time.h"
#include "Sound.h"
#include "Sprite.h"
#include "Object3d.h"
#include "DebugText.h"

#include <DirectXMath.h>

#include "Camera.h"

#include "ParticleManager.h"

#include "DirectXCommon.h"

#include "Input.h"

#include <vector>

#include "Light.h"

#include "FbxObj3d.h"

class PlayScene :
	public GameScene {

#pragma region ビュー変換行列

	//DirectX::XMMATRIX matView;
	DirectX::XMFLOAT3 eye_local;   // 視点座標
	DirectX::XMFLOAT3 target_local;   // 注視点座標
	DirectX::XMFLOAT3 up_local;       // 上方向ベクトル

#pragma endregion ビュー変換行列

#pragma region 音

	std::unique_ptr<Sound::SoundCommon> soundCommon;

	std::unique_ptr<Sound> soundData1;

	std::unique_ptr<Sound> particleSE;

#pragma endregion 音

#pragma region スプライト
	// --------------------
	// スプライト共通
	// --------------------
	std::unique_ptr<SpriteCommon> spriteCommon;
	// スプライト共通テクスチャ読み込み
	UINT texNum = 0u;

	// --------------------
	// スプライト個別
	// --------------------
	static const int SPRITES_NUM = 1;
	Sprite sprites[SPRITES_NUM]{};

	// --------------------
	// デバッグテキスト
	// --------------------
	std::unique_ptr<DebugText> debugText;
	// デバッグテキスト用のテクスチャ番号
	UINT debugTextTexNumber;
#pragma endregion スプライト

#pragma region ライト

	std::unique_ptr<Light> light;

#pragma endregion ライト


#pragma region 3Dオブジェクト

	// 3Dオブジェクト用パイプライン生成
	Object3d::PipelineSet object3dPipelineSet;
	Object3d::PipelineSet backPipelineSet;

	const UINT obj3dTexNum = 0U;
	std::unique_ptr<Model> model;
	std::vector<Object3d> obj3d;
	const float obj3dScale = 10.f;

	DirectX::XMFLOAT2 angle{};	// 各軸周りの回転角

	std::unique_ptr<Object3d> lightObj;

	std::unique_ptr<Model> backModel;
	std::unique_ptr<Object3d> backObj;

#pragma endregion 3Dオブジェクト

#pragma region FBXオブジェクト

	std::unique_ptr<FbxModel> fbxModel;
	std::unique_ptr<FbxObj3d> fbxObj3d;

#pragma endregion FBXオブジェクト


	Input* input = nullptr;

	std::unique_ptr<Time> timer;

	std::unique_ptr<Camera> camera;

	std::unique_ptr<ParticleManager> particleMgr;

	DirectXCommon* dxCom = nullptr;

private:
	void createParticle(const DirectX::XMFLOAT3& pos, const UINT particleNum = 10U, const float startScale = 1.f);

public:
	PlayScene();
	void init() override;
	void update() override;
	void drawObj3d() override;
	void drawFrontSprite() override;

	~PlayScene() override;
};

