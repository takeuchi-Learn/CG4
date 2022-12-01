#pragma once

#include <string>
#include <vector>
#include <memory>
#include <DirectXMath.h>

/// @brief レベルデータのローダー
class LevelLoader {

public:
	/// @brief レベルデータ
	struct LevelData {

		struct ObjectData {
			// ファイル名
			std::string fileName = "";
			// 平行移動
			DirectX::XMVECTOR trans;
			// 回転角
			DirectX::XMVECTOR rota;
			// スケーリング
			DirectX::XMVECTOR scale;
		};

		// オブジェクト配列
		std::vector<ObjectData> objects;
		std::unique_ptr<ObjectData> camera;
	};

public:

	/// @brief レベルデータのファイル読み込み
	/// @param fileDir ファイルのあるフォルダ
	/// @param fileName ファイル名(拡張子を含む)
	/// @return 読み込んだデータ
	static LevelLoader::LevelData* loadLevelFile(const std::string& fileDir,
													  const std::string& fileName);
};

