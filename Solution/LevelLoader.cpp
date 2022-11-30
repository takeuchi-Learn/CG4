#include "LevelLoader.h"

#include "nlohmann/json.hpp"
#include <fstream>
#include <cassert>

const std::string LevelLoader::defLoadDir = "Resources/levels/";
const std::string LevelLoader::extension = ".json";

std::unique_ptr<LevelLoader::LevelData>&& LevelLoader::loadLevelFile(const std::string& fileName) {
	const std::string fullpath = defLoadDir + fileName + extension;

	// ファイルを開く
	std::ifstream jsonFile(fullpath);
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
	std::unique_ptr<LevelData> levelData = std::make_unique<LevelData>();

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

			// todo コライダーのパラメータ読み込み
		}

		// todo オブジェクト走査を再帰関数にまとめ、再帰呼出で枝を走査する
		if (i.contains("children")) {

		}
	}

	return std::move(levelData);
}
