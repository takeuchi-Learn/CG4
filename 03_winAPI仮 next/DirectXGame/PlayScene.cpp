#include "PlayScene.h"

#include "SceneManager.h"

#include "Input.h"

#include <sstream>
#include <iomanip>

#include <xaudio2.h>
#include <random>

using namespace DirectX;

namespace {
	class MyRand {
	private:
		MyRand(const MyRand& mrnd) = delete;
		MyRand& operator=(const MyRand& mrnd) = delete;

		std::random_device rnd{};
		std::mt19937_64 mt{};

		MyRand() : rnd(), mt(rnd()) {};
		~MyRand() {};

		static MyRand* getInstance() {
			static MyRand myRand{};
			return &myRand;
		}

	private:
		// 一様乱数
		int local_getRand(const int min, const int max) {
			int minLocal = min, maxLocal = max;
			if (max < min) {
				minLocal = max;
				maxLocal = min;
			}
			std::uniform_int_distribution<> myRand(minLocal, maxLocal);	// 範囲指定の乱数
			return myRand(mt);
		}
		double local_getRand(const double min, const double max) {
			double minLocal = min, maxLocal = max;
			if (max < min) {
				minLocal = max;
				maxLocal = min;
			}
			std::uniform_real_distribution<> myRand(minLocal, maxLocal);	// 範囲指定の乱数
			return myRand(mt);
		}

		// 正規分布乱数
		double local_getRandNormally(const double center, const double range) {
			double rangeLocal = range;
			if (range < 0.0) rangeLocal = -rangeLocal;
			else if (range == 0.0) rangeLocal = 1.f;

			std::normal_distribution<> myRand(center, rangeLocal);	// 範囲指定の乱数
			return myRand(mt);
		}

	public:
		// 一様乱数_整数
		static int getRand(const int min, const int max) {
			return getInstance()->local_getRand(min, max);
		}
		// 一様乱数_小数(double)
		static double getRand(const double min, const double max) {
			return getInstance()->local_getRand(min, max);
		}
		// 一様乱数_小数(float)
		static float getRandf(const float min, const float max) {
			return (float)getRand((double)min, (double)max);
		}

		// 正規分布乱数_double
		static double getRandNormally(const double center, const double range) {
			return getInstance()->local_getRandNormally(center, range);
		}
		// 正規分布乱数_float
		static float getRandNormallyf(const float center, const float range) {
			return (float)getRandNormally((double)center, (double)range);
		}
	};
}

void PlayScene::init() {
	WinAPI::getInstance()->setWindowText("Press SPACE to change scene - now : Play");

	dxCom = DirectXCommon::getInstance();

#pragma region ビュー変換

	camera.reset(new Camera(WinAPI::window_width, WinAPI::window_height));
	camera->setEye(XMFLOAT3(0, 0, -175));	// 視点座標
	camera->setTarget(XMFLOAT3(0, 0, 0));	// 注視点座標
	camera->setUp(XMFLOAT3(0, 1, 0));		// 上方向

#pragma endregion ビュー変換

#pragma region 音

	soundCommon.reset(new Sound::SoundCommon());
	soundData1.reset(new Sound("Resources/BGM.wav", soundCommon.get()));

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
		sprites[i].position.x = 1280 / 10;
		sprites[i].position.y = 720 / 10;
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
						  WinAPI::window_width, WinAPI::window_height,
						  Object3d::constantBufferNum, obj3dTexNum));

	obj3d.reset(new Object3d(DirectXCommon::getInstance()->getDev(), model.get(), obj3dTexNum));
	obj3d->scale = { obj3dScale, obj3dScale, obj3dScale };
	obj3d->position = { 0, 0, obj3dScale };

	sphere.reset(new Sphere(DirectXCommon::getInstance()->getDev(), 2.f, L"Resources/red.png", 0));

#pragma endregion 3Dオブジェクト

	particleMgr.reset(new ParticleManager(dxCom->getDev(), L"Resources/effect1.png",camera.get()));

	timer.reset(new Time());
}

void PlayScene::update() {

	if (Input::getInstance()->triggerKey(DIK_SPACE)) {
		SceneManager::getInstange()->changeScene(SCENE_NUM::END);
	}

#pragma region 音

	// 数字の0キーが押された瞬間音を再生しなおす
	if (Input::getInstance()->triggerKey(DIK_0)) {
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
		debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 2, 1.f, "SOUND_PLAY_STATE : %s", stateStr.c_str());

		debugText.Print(spriteCommon, "Press 0 to Play/Stop Sound", 0, debugText.fontHeight * 3);
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
		debugText.Print(spriteCommon, "LSHIFT(WinAPI)", 0, 0, 2);
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


	debugText.formatPrint(spriteCommon, 0, 0, 1.f, "FPS : %f", dxCom->getFPS());

	if (Input::getInstance()->hitKey(DIK_R)) timer->reset();

	debugText.formatPrint(spriteCommon, 0, debugText.fontHeight * 5, 1.f,
						  "Time : %.6f[s]", (long double)timer->getNowTime() / Time::oneSec);

#pragma endregion 時間

#pragma region カメラ移動回転

	{

		const float rotaVal = XM_PIDIV2 / DirectXCommon::getInstance()->getFPS();	// 毎秒四半周

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
		camera->rotation(camRange, angle.x, angle.y);


		// 移動量
		const float moveSpeed = 75.f / dxCom->getFPS();
		// 視点移動
		if (Input::getInstance()->hitKey(DIK_W)) {
			camera->moveForward(moveSpeed);
		} else if (Input::getInstance()->hitKey(DIK_S)) {
			camera->moveForward(-moveSpeed);
		}
		if (Input::getInstance()->hitKey(DIK_A)) {
			camera->moveRight(-moveSpeed);
		} else if (Input::getInstance()->hitKey(DIK_D)) {
			camera->moveRight(moveSpeed);
		}
	}

#pragma endregion カメラ移動回転

	if (Input::getInstance()->hitKey(DIK_I)) sprites[0].position.y -= 10; else if (Input::getInstance()->hitKey(DIK_K)) sprites[0].position.y += 10;
	if (Input::getInstance()->hitKey(DIK_J)) sprites[0].position.x -= 10; else if (Input::getInstance()->hitKey(DIK_L)) sprites[0].position.x += 10;

	// Pを押すたびパーティクル50粒追加
	constexpr UINT particleNum = 50U;
	if (Input::getInstance()->triggerKey(DIK_P)) createParticle(obj3d->position, particleNum);

	camera->update();
}

void PlayScene::draw() {
	// ４．描画コマンドここから
	// 球体コマンド
	Sphere::sphereCommonBeginDraw(object3dPipelineSet);
	sphere->drawWithUpdate(camera->getViewMatrix(), dxCom);
	// 3Dオブジェクトコマンド
	Object3d::Object3dCommonBeginDraw(dxCom->getCmdList(), object3dPipelineSet);
	obj3d->drawWithUpdate(camera->getViewMatrix(), dxCom);

	particleMgr->drawWithUpdate(dxCom->getCmdList());

	// スプライト共通コマンド
	Sprite::SpriteCommonBeginDraw(spriteCommon, dxCom->getCmdList());
	// スプライト描画
	for (UINT i = 0; i < _countof(sprites); i++) {
		sprites[i].SpriteDrawWithUpdate(dxCom, spriteCommon);
	}
	// デバッグテキスト描画
	debugText.DrawAll(dxCom, spriteCommon);
	// ４．描画コマンドここまで
}

void PlayScene::fin() {
	//Sound::SoundStopWave(soundData1.get());
}

void PlayScene::createParticle(const DirectX::XMFLOAT3 pos, const UINT particleNum) {
	for (UINT i = 0U; i < particleNum; i++) {

		const float theata = MyRand::getRandf(0, XM_PI);
		const float phi = MyRand::getRandf(0, XM_PI * 2.f);
		const float r = MyRand::getRandf(0, 5.f);

		// X,Y,Z全て[-2.5f,+2.5f]でランダムに分布
		constexpr float rnd_pos = 2.5f;
		XMFLOAT3 generatePos = pos;
		/*generatePos.x += MyRand::getRandNormallyf(0.f, rnd_pos);
		generatePos.y += MyRand::getRandNormallyf(0.f, rnd_pos);
		generatePos.z += MyRand::getRandNormallyf(0.f, rnd_pos);*/

		//constexpr float rnd_vel = 0.0625f;
		const XMFLOAT3 vel{
			r * sin(theata) * cos(phi),
			r * cos(theata),
			r * sin(theata) * sin(phi)
		};

		//constexpr float rnd_acc = 0.05f;
		XMFLOAT3 acc{};

		/*acc.x = 0.f;
		acc.y = -MyRand::getRandf(rnd_acc, rnd_acc * 2.f);
		acc.z = 0.f;*/


		constexpr auto startCol = XMFLOAT3(1, 1, 0.25f), endCol = XMFLOAT3(1, 0, 1);
		constexpr int life = Time::oneSec / 4;
		constexpr float startScale = 10.f, endScale = 0.f;
		constexpr float startRota = 0.f, endRota = 0.f;

		// 追加
		particleMgr->add(timer.get(),
						 life, generatePos, vel, acc,
						 startScale, endScale,
						 startRota, endRota,
						 startCol, endCol);
	}
}