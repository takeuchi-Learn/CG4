#include "LevelLoadScene.h"
#include <DirectXMath.h>

#include "Input.h"
#include "PostEffect.h"

using namespace DirectX;

LevelLoadScene::LevelLoadScene()
{
	camera = std::make_unique<Camera>(WinAPI::window_width, WinAPI::window_height);
	camera->setEye(XMFLOAT3(0, 10, -10));
	camera->setTarget(XMFLOAT3(0, 0, 10));
	camera->setUp(XMFLOAT3(0, 1, 0));

	light = std::make_unique<Light>();

	std::unique_ptr<LevelLoader::LevelData> levelData(LevelLoader::loadLevelFile("Resources/levels/", "level.json"));

	if (levelData->camera) {
		XMFLOAT3 pos{};
		XMStoreFloat3(&pos, levelData->camera->trans);
		camera->setEye(pos);
	}

	for (auto& i : levelData->objects) {

		std::string path = "Resources/" + i.fileName;
		std::string name = i.fileName;

		if (i.fileName == "") {
			path = "Resources/sphere/";
			name = "sphere";
		}

		auto& model = models.emplace_back(std::make_unique<ObjModel>(path, name));
		auto& object = objects.emplace_back(std::make_unique<Object3d>(DX12Base::getInstance()->getDev(),
																	   camera.get(),
																	   model.get(),
																	   0U));

		XMStoreFloat3(&object->position, i.trans);
		XMStoreFloat3(&object->rotation, i.rota);
		XMStoreFloat3(&object->scale, i.scale);
	}

	light->setLightPos(camera->getEye());
}

LevelLoadScene::~LevelLoadScene() {

}

void LevelLoadScene::update()
{
	const bool hitW = Input::getInstance()->hitKey(DIK_W);
	const bool hitA = Input::getInstance()->hitKey(DIK_A);
	const bool hitS = Input::getInstance()->hitKey(DIK_S);
	const bool hitD = Input::getInstance()->hitKey(DIK_D);

	if (hitW || hitA || hitS || hitD) {
		const float speed = eyeSpeed / DX12Base::getInstance()->getFPS();

		XMFLOAT3 eye = camera->getEye();

		if (hitW) {
			eye.y += speed;
		} else if (hitS) {
			eye.y -= speed;
		}
		if (hitA) {
			eye.x -= speed;
		} else if (hitD) {
			eye.x += speed;
		}

		camera->setEye(eye);
		light->setLightPos(camera->getEye());
	}

	light->update();
	camera->update();
}

void LevelLoadScene::drawObj3d()
{
	Object3d::startDraw(DX12Base::getInstance()->getCmdList(),
						Object3d::getGraphicsPipeline());

	for (auto& i : objects) {
		i->drawWithUpdate(DX12Base::getInstance(), light.get());
	}
}

void LevelLoadScene::drawFrontSprite()
{
	ImGui::Begin("move camera param",
				 nullptr,
				 ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings
				 | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::SliderFloat("move speed", &eyeSpeed, 0.1f, 10.f, "%.2f",
					   ImGuiSliderFlags_::ImGuiSliderFlags_ClampOnInput);
	ImGui::Text("WASD : move camera");
	ImGui::Text("pos : %.2f,%.2f,%.2f",
				camera->getEye().x,
				camera->getEye().y,
				camera->getEye().z);
	ImGui::End();
}
