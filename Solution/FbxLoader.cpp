#include "FbxLoader.h"
#include <cassert>

const std::string FbxLoader::baseDir = "Resources/";

FbxLoader* FbxLoader::GetInstance()
{
    static FbxLoader instance;
    return &instance;
}

void FbxLoader::init(ID3D12Device* dev) {
    assert(fbxManager == nullptr);
    this->dev = dev;
    // マネージャーの生成
    fbxManager = FbxManager::Create();
    // マネージャーの入出力設定
    FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
    fbxManager->SetIOSettings(ios);
    // インポーターの設定
    fbxImporter = FbxImporter::Create(fbxManager, "");
}

void FbxLoader::fin() {
    // インスタンスの破棄
    fbxImporter->Destroy();
    fbxManager->Destroy();
}

void FbxLoader::loadModelFromFile(const std::string& modelName) {
    const std::string dirPath = baseDir + modelName + "/";
    // 拡張子を付加
    const std::string fileName = modelName + ".fbx";
    // 繋げてフルパスを得る
    const std::string fullPath = dirPath + fileName;

    // ファイル名を指定してファイル読み込み
    if (!fbxImporter->Initialize(fullPath.c_str(),
                                 -1,
                                 fbxManager->GetIOSettings())) {
        assert(0);
    }

    // シーン生成
    FbxScene* fbxScene = FbxScene::Create(fbxManager, "fbxScene");
    //ロードした情報をインポート
    fbxImporter->Import(fbxScene);
}
