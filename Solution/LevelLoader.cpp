#include "LevelLoader.h"

#include "nlohmann/json.hpp"
#include <fstream>
#include <cassert>

LevelLoader::LevelData* LevelLoader::loadLevelFile(const std::string& fileDir,
												   const std::string& fileName) {
	// ファイルを開く
	std::ifstream jsonFile(fileDir + fileName);
	assert(!jsonFile.fail());

	// JSONデータ
	nlohmann::json jsonData{};
	// ファイルの情報を入れる
	jsonFile >> jsonData;

	// 形式が正しいか確認
	assert(jsonData.is_object());
	assert(jsonData.contains("name"));
	assert(jsonData["name"].is_string());

	// "name"を文字列として取得
	const std::string name = jsonData["name"].get<std::string>();
	// 形式が正しいか確認
	assert(name == "scene");

	// レベルデータを入れる場所
	LevelData* levelData = new LevelData();

	// "objects"の全オブジェクトを走査
	for (nlohmann::json& i : jsonData["objects"]) {
		// 形式が正しいか確認
		assert(i.contains("type"));

		// 種別を取得
		const std::string type = i["type"].get<std::string>();

		// MESH
		if (type == "MESH") {
			// 要素追加
			auto& objectData = levelData->objects.emplace_back(LevelData::ObjectData());

			if (i.contains("file_name")) {
				// ファイル名
				objectData.fileName = i["file_name"];
			}

			// 平行移動
			objectData.trans.m128_f32[0] = (float)i["transform"]["translation"][1];
			objectData.trans.m128_f32[1] = (float)i["transform"]["translation"][2];
			objectData.trans.m128_f32[2] = -(float)i["transform"]["translation"][0];
			objectData.trans.m128_f32[3] = 1.f;
			// 回転
			objectData.rota.m128_f32[0] = -(float)i["transform"]["rotation"][1];
			objectData.rota.m128_f32[1] = -(float)i["transform"]["rotation"][2];
			objectData.rota.m128_f32[2] = (float)i["transform"]["rotation"][0];
			objectData.rota.m128_f32[3] = 0.f;
			// スケール
			objectData.scale.m128_f32[0] = (float)i["transform"]["scaling"][1];
			objectData.scale.m128_f32[1] = (float)i["transform"]["scaling"][2];
			objectData.scale.m128_f32[2] = (float)i["transform"]["scaling"][0];
			objectData.scale.m128_f32[3] = 0.f;
		} else if (type == "CAMERA") {
			// 要素追加
			auto& camera = levelData->camera;
			camera.reset(new LevelData::ObjectData());

			if (i.contains("file_name")) {
				// ファイル名
				camera->fileName = i["file_name"];
			}

			// 平行移動
			camera->trans.m128_f32[0] = (float)i["transform"]["translation"][1];
			camera->trans.m128_f32[1] = (float)i["transform"]["translation"][2];
			camera->trans.m128_f32[2] = -(float)i["transform"]["translation"][0];
			camera->trans.m128_f32[3] = 1.f;
			// 回転
			camera->rota.m128_f32[0] = -(float)i["transform"]["rotation"][1];
			camera->rota.m128_f32[1] = -(float)i["transform"]["rotation"][2];
			camera->rota.m128_f32[2] = (float)i["transform"]["rotation"][0];
			camera->rota.m128_f32[3] = 0.f;
			// スケール
			camera->scale.m128_f32[0] = (float)i["transform"]["scaling"][1];
			camera->scale.m128_f32[1] = (float)i["transform"]["scaling"][2];
			camera->scale.m128_f32[2] = (float)i["transform"]["scaling"][0];
		}
	}

	return levelData;
}
