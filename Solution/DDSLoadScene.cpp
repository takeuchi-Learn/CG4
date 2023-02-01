#include "DDSLoadScene.h"
#include "PostEffect.h"
#include "Input.h"
#include "SceneManager.h"
#include "TitleScene.h"

#include <DirectXMath.h>
using namespace DirectX;

DDSLoadScene::DDSLoadScene() :
	camera(std::make_unique<Camera>(WinAPI::window_width,
									WinAPI::window_height)),
	light(std::make_unique<Light>())
{
	ppset = Object3d::createGraphicsPipeline(DX12Base::getInstance()->getDev());

	camera->setEye(XMFLOAT3(0, 0, 0));
	camera->setTarget(XMFLOAT3(0, 0, 1));
	camera->setUp(XMFLOAT3(0, 1, 0));

	cubeModel.reset(new ObjModel("Resources/model", "model"));
	cubeObj.reset(new Object3d(DX12Base::getInstance()->getDev(), camera.get(), cubeModel.get(), 0U));

	cubeObj->position = XMFLOAT3(0, 0, 10);
	cubeObj->rotation.y += 180.f;

	auto ps = PostEffect::getInstance();
	ps->setMosaicNum(XMFLOAT2(WinAPI::window_width, WinAPI::window_height));
	ps->setNoiseIntensity(0.f);
	ps->setVignetteVal(0.f);
}

DDSLoadScene::~DDSLoadScene()
{
}

void DDSLoadScene::update()
{
	if (Input::getInstance()->triggerKey(DIK_SPACE))
	{
		SceneManager::getInstange()->changeScene(new TitleScene());
	}

	light->setLightPos(camera->getEye());
	light->update();
	camera->update();
	cubeObj->update(DX12Base::getInstance()->getDev());
}

void DDSLoadScene::drawObj3d()
{
	Object3d::startDraw(DX12Base::getInstance()->getCmdList(), ppset);
	cubeObj->draw(DX12Base::getInstance(), light.get());
}
