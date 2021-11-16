#pragma once

#include "ObjCommon.h"

class Sprite : ObjCommon {
public:
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT2 = DirectX::XMFLOAT2;

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

	// スプライト1枚分のデータ
	struct SpriteData {
		//頂点バッファ;
		ComPtr<ID3D12Resource> vertBuff;
		//頂点バッファビュー;
		D3D12_VERTEX_BUFFER_VIEW vbView{};
		//定数バッファ;
		ComPtr<ID3D12Resource> constBuff;
		// Z軸回りの回転角
		float rotation = 0.0f;
		// 座標
		XMFLOAT3 position = { 0,0,0 };
		// ワールド行列
		XMMATRIX matWorld;
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
	};

public:
	// スプライト用パイプライン生成
	PipelineSet SpriteCreateGraphicsPipeline(ID3D12Device* dev);

	// スプライト共通データ生成
	SpriteCommon SpriteCommonCreate(ID3D12Device* dev, int window_width, int window_height);

	// スプライト共通テクスチャ読み込み
	void SpriteCommonLoadTexture(SpriteCommon& spriteCommon, UINT texnumber, const wchar_t* filename, ID3D12Device* dev);

	// スプライト単体頂点バッファの転送
	void SpriteTransferVertexBuffer(const SpriteData& sprite, const SpriteCommon& spriteCommon);

	// スプライト生成
	SpriteData SpriteCreate(ID3D12Device* dev, int window_width, int window_height,
		UINT texNumber, const SpriteCommon& spriteCommon, XMFLOAT2 anchorpoint = { 0.5f,0.5f },
		bool isFlipX = false, bool isFlipY = false);

	// スプライト共通グラフィックコマンドのセット
	void SpriteCommonBeginDraw(const SpriteCommon& spriteCommon, ID3D12GraphicsCommandList* cmdList);

	// スプライト単体更新
	void SpriteUpdate(SpriteData& sprite, const SpriteCommon& spriteCommon);

	// スプライト単体描画
	void SpriteDraw(const SpriteData& sprite, ID3D12GraphicsCommandList* cmdList, const SpriteCommon& spriteCommon, ID3D12Device* dev);
};

