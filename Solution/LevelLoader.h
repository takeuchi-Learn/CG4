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
			std::string fileName;
			// 平行移動
			DirectX::XMVECTOR trans;
			// 回転角
			DirectX::XMVECTOR rota;
			// スケーリング
			DirectX::XMVECTOR scale;
		};

		// オブジェクト配列
		std::vector<ObjectData> objects;
	};

public:
	// 既定の読み込みフォルダ
	static const std::string defLoadDir;
	// 拡張子
	static const std::string extension;

public:

	/// @brief レベルデータのファイル読み込み
	/// @param fileName ファイル名
	/// @return 読み込んだデータ
	static std::unique_ptr<LevelData>&& loadLevelFile(const std::string& fileName);
};

