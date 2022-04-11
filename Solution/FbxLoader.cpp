#include "FbxLoader.h"
#include <cassert>

using namespace DirectX;

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

    // モデル生成
    FbxModel* model = new FbxModel();
    model->name = modelName;

    // ノードの数を取得
    int nodeCount = fbxScene->GetNodeCount();
    // 予め必要分のメモリを確保し、アドレスのずれを予防
    model->nodes.reserve(nodeCount);

    // ルートノードから順に解析しモデルに流し込む
    parseNodeRecursive(model, fbxScene->GetRootNode());

    // シーン解放
    fbxScene->Destroy();
}

void FbxLoader::parseNodeRecursive(FbxModel* model,
                                   FbxNode* fbxNode,
                                   FbxModel::Node* parent) {
    // ノード名取得
    std::string name = fbxNode->GetName();

    // モデルにノードを追加
    model->nodes.emplace_back();
    FbxModel::Node& node = model->nodes.back();
    // ノード名を取得
    node.name = fbxNode->GetName();
    // ノードのローカル移動情報
    FbxDouble3 rotation = fbxNode->LclRotation.Get();
    FbxDouble3 scaling = fbxNode->LclScaling.Get();
    FbxDouble3 translation = fbxNode->LclTranslation.Get();
    // 形式変換して代入
    node.rotation = { (float)rotation[0], (float)rotation[1], (float)rotation[2], 0.f };
    node.scaling = { (float)scaling[0], (float)scaling[1], (float)scaling[2], 0.f };
    node.translation = { (float)translation[0], (float)translation[1], (float)translation[2], 1.f };

    // 回転角を弧度法に変換
    node.rotation.m128_f32[0] = XMConvertToRadians(node.rotation.m128_f32[0]);
    node.rotation.m128_f32[1] = XMConvertToRadians(node.rotation.m128_f32[1]);
    node.rotation.m128_f32[2] = XMConvertToRadians(node.rotation.m128_f32[2]);

    // スケール、回転、平行移動行列の計算
    XMMATRIX matScaling, matRotation, matTranslation;
    matScaling = XMMatrixScalingFromVector(node.scaling);
    matRotation = XMMatrixRotationRollPitchYawFromVector(node.rotation);
    matTranslation = XMMatrixTranslationFromVector(node.translation);

    // ローカル変形行列の計算
    node.transform = XMMatrixIdentity();
    node.transform *= matScaling;
    node.transform *= matRotation;
    node.transform *= matTranslation;

    // グローバル変形行列の計算
    node.globalTransform = node.transform;
    if (parent) {
        node.parent = parent;
        node.globalTransform *= parent->globalTransform;
    }

    // todo ノードの情報を解析してノードに記録

    // todo fbxノードのメッシュ構造を解析

    // 子ノードに対して再帰呼び出し
    for (int i = 0; i < fbxNode->GetChildCount(); i++) {
        parseNodeRecursive(model, fbxNode->GetChild(i), &node);
    }
}
