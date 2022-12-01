#include "LevelLoader.h"

#include "nlohmann/json.hpp"
#include <fstream>
#include <cassert>

using namespace DirectX;

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

	// "name"を取得
	const std::string name = jsonData["name"].get<std::string>();
	// 形式が正しいか確認
	assert(name == "scene");

	// レベルデータを入れる場所
	LevelData* levelData = new LevelData();

	// "objects"を全て読む
	for (nlohmann::json& i : jsonData["objects"]) {
		// 形式が正しいか確認
		assert(i.contains("type"));

		// 種別を取得
		const std::string type = i["type"].get<std::string>();

		// MESH
		if (type == "MESH") {
			// オブジェクトを追加
			auto& objectData = levelData->objects.emplace_back(LevelData::ObjectData());

			// ファイル名
			if (i.contains("file_name")) {
				objectData.fileName = i["file_name"];
			}

			// 平行移動
			auto& tmp = i["transform"]["translation"];
			objectData.trans = XMFLOAT3((float)tmp[1],
										(float)tmp[2],
										-(float)tmp[0]);
			// 回転
			tmp = i["transform"]["rotation"];
			objectData.rota = XMFLOAT3(-(float)tmp[1],
									   -(float)tmp[2],
									   (float)tmp[0]);
			// スケール
			tmp = i["transform"]["scaling"];
			objectData.scale = XMFLOAT3(tmp[1],
										tmp[2],
										tmp[0]);
		} else if (type == "CAMERA") {
			auto& camera = levelData->camera;
			camera.reset(new LevelData::ObjectData());

			// ファイル名
			if (i.contains("file_name")) {
				camera->fileName = i["file_name"];
			}

			// 平行移動
			auto& tmp = i["transform"]["translation"];
			camera->trans = XMFLOAT3((float)tmp[1],
									 (float)tmp[2],
									 -(float)tmp[0]);
			// 回転
			tmp = i["transform"]["rotation"];
			camera->rota = XMFLOAT3(-(float)tmp[1],
									-(float)tmp[2],
									(float)tmp[0]);
			// スケール
			tmp = i["transform"]["scaling"];
			camera->scale = XMFLOAT3(tmp[1],
									 tmp[2],
									 tmp[0]);
		}
	}

	return levelData;
}
