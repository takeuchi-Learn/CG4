#pragma once

#include "ObjCommon.h"

class Sprite : ObjCommon {
private:
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT2 = DirectX::XMFLOAT2;

public:
	// 頂点データ
	struct VertexPosUv {
		XMFLOAT3 pos; // xyz座標
		XMFLOAT2 uv;  // uv座標
	};

	// テクスチャの最大枚数
	static const int spriteSRVCount = 512;

	// スプライトの共通データ
	struct SpriteCommon {
		// パイプラインセット
		PipelineSet pipelineSet;
		// 射影行列
		XMMATRIX matProjection{};
		// テクスチャ用デスクリプタヒープの生成
		ComPtr<ID3D12DescriptorHeap> descHeap;
		// テクスチャリソース（テクスチャバッファ）の配列
		ComPtr<ID3D12Resource> texBuff[spriteSRVCount];
	};

	// --------------------
	// スプライト1枚分のデータ
	// --------------------
private:
	//頂点バッファ;
	ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//定数バッファ;
	ComPtr<ID3D12Resource> constBuff;
	// ワールド行列
	XMMATRIX matWorld;

public:
	// Z軸回りの回転角
	float rotation = 0.0f;
	// 座標
	XMFLOAT3 position = { 0,0,0 };
	// 色(RGBA)
	XMFLOAT4 color = { 1, 1, 1, 1 };
	// テクスチャ番号
	UINT texNumber = 0;
	// 大きさ
	XMFLOAT2 size = { 100, 100 };
	// アンカーポイント
	XMFLOAT2 anchorpoint = { 0.5f, 0.5f };
	// 左右反転
	bool isFlipX = false;
	// 上下反転
	bool isFlipY = false;
	// テクスチャ左上座標
	XMFLOAT2 texLeftTop = { 0, 0 };
	// テクスチャ切り出しサイズ
	XMFLOAT2 texSize = { 100, 100 };
	// 非表示
	bool isInvisible = false;

public:
	// --------------------
	// 共通
	// --------------------

	// スプライト用パイプライン生成
	static PipelineSet SpriteCreateGraphicsPipeline(ID3D12Device* dev,
		const wchar_t* vsPath = L"Resources/Shaders/SpriteVS.hlsl",
		const wchar_t* psPath = L"Resources/Shaders/SpritePS.hlsl");

	// スプライト共通データ生成
	static SpriteCommon SpriteCommonCreate(ID3D12Device* dev, int window_width, int window_height);

	// スプライト共通テクスチャ読み込み
	static void SpriteCommonLoadTexture(SpriteCommon& spriteCommon, UINT texnumber, const wchar_t* filename, ID3D12Device* dev);

	// スプライト共通グラフィックコマンドのセット
	static void SpriteCommonBeginDraw(const SpriteCommon& spriteCommon, ID3D12GraphicsCommandList* cmdList);

	// --------------------
	// 個別
	// --------------------

	// スプライト単体頂点バッファの転送
	void SpriteTransferVertexBuffer(const SpriteCommon& spriteCommon);

	// スプライト生成
	void SpriteCreate(ID3D12Device* dev, int window_width, int window_height,
		UINT texNumber, const SpriteCommon& spriteCommon, XMFLOAT2 anchorpoint = { 0.5f,0.5f },
		bool isFlipX = false, bool isFlipY = false);

	// スプライト単体更新
	void SpriteUpdate(const SpriteCommon& spriteCommon);

	// スプライト単体描画
	void SpriteDraw(ID3D12GraphicsCommandList* cmdList, const SpriteCommon& spriteCommon, ID3D12Device* dev);

	// 更新と描画を同時に行う
	void Sprite::SpriteDrawWithUpdate(ID3D12GraphicsCommandList* cmdList, const SpriteCommon& spriteCommon, ID3D12Device* dev);
};

