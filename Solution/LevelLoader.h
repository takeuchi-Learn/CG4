#pragma once

#include <vector>
#include <string>
#include <memory>
#include <DirectXMath.h>

/// @brief レベルデータ(json)を読み込むクラス
class LevelLoader {

public:
	/// @brief レベルデータの構造体
	struct LevelData {

		/// @brief レベルデータに含まれるオブジェクト
		struct ObjectData {
			// ファイル名
			std::string fileName = "";
			// 平行移動
			DirectX::XMFLOAT3 trans;
			// 回転角
			DirectX::XMFLOAT3 rota;
			// スケーリング
			DirectX::XMFLOAT3 scale;
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

